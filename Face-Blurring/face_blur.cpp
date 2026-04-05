#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

// ============================================================
// KUYRUK YAPILARI
// ============================================================
struct KareKuyrugu {
    std::queue<cv::Mat>     kuyruk;
    std::mutex              mtx;
    std::condition_variable cv;
    bool                    bitti = false;
};

struct SonucKuyrugu {
    std::queue<cv::Mat>     kuyruk;
    std::mutex              mtx;
    std::condition_variable cv;
};

// ============================================================
// workerThread()
// İş kuyruğundan kare alır, yüzleri tespit eder, blurlar,
// sonuç kuyruğuna koyar.
// ============================================================
void workerThread(KareKuyrugu& isKuyrugu, SonucKuyrugu& sonucKuyrugu,
                  const std::string& protoPath, const std::string& modelPath) {

    // Her thread kendi modelini yükler (thread-safe değil paylaşmak)
    cv::dnn::Net net = cv::dnn::readNetFromCaffe(protoPath, modelPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    while (true) {
        cv::Mat frame;

        // --- İş kuyruğundan kare al ---
        {
            std::unique_lock<std::mutex> lock(isKuyrugu.mtx);
            isKuyrugu.cv.wait(lock, [&] {
                return !isKuyrugu.kuyruk.empty() || isKuyrugu.bitti;
            });
            if (isKuyrugu.kuyruk.empty() && isKuyrugu.bitti) break;
            frame = isKuyrugu.kuyruk.front();
            isKuyrugu.kuyruk.pop();
        }

        // Hız için kareyi %50 küçült → DNN daha hızlı çalışır
        // Koordinatlar normalize (0-1) olduğu için orijinal boyuta otomatik ölçeklenir
        cv::Mat kucukFrame;
        cv::resize(frame, kucukFrame, cv::Size(), 0.5, 0.5);

        cv::Mat blob = cv::dnn::blobFromImage(
            kucukFrame, 1.0, cv::Size(300, 300),
            cv::Scalar(104.0, 177.0, 123.0)
        );
        net.setInput(blob);
        cv::Mat tespitler = net.forward();
        cv::Mat tespitMat(tespitler.size[2], tespitler.size[3], CV_32F, tespitler.ptr<float>());

        for (int i = 0; i < tespitMat.rows; i++) {
            float guven = tespitMat.at<float>(i, 2);

            // 0.4 → daha hassas tespit, yüz kaçırma azalır
            if (guven > 0.4) {
                int x1 = static_cast<int>(tespitMat.at<float>(i, 3) * frame.cols);
                int y1 = static_cast<int>(tespitMat.at<float>(i, 4) * frame.rows);
                int x2 = static_cast<int>(tespitMat.at<float>(i, 5) * frame.cols);
                int y2 = static_cast<int>(tespitMat.at<float>(i, 6) * frame.rows);

                cv::Rect yuz(cv::Point(x1, y1), cv::Point(x2, y2));
                yuz &= cv::Rect(0, 0, frame.cols, frame.rows);

                if (yuz.width > 0 && yuz.height > 0) {
                    // Yüzü %40 genişlet → saçlar, çene, yanak kaçmasın
                    int gx = static_cast<int>(yuz.width  * 0.4);
                    int gy = static_cast<int>(yuz.height * 0.4);
                    cv::Rect genisYuz(
                        std::max(0, yuz.x - gx),
                        std::max(0, yuz.y - gy),
                        std::min(frame.cols - std::max(0, yuz.x - gx), yuz.width  + 2 * gx),
                        std::min(frame.rows - std::max(0, yuz.y - gy), yuz.height + 2 * gy)
                    );
                    cv::GaussianBlur(frame(genisYuz), frame(genisYuz), cv::Size(99, 99), 30);
                }
            }
        }

        // İşlenmiş kareyi sonuç kuyruğuna ekle ve ana thread'i uyandır
        {
            std::lock_guard<std::mutex> lock(sonucKuyrugu.mtx);
            sonucKuyrugu.kuyruk.push(frame);
        }
        sonucKuyrugu.cv.notify_one();
    }
}

// ============================================================
// MAIN
// ============================================================
int main() {
    const std::string protoPath = "deploy.prototxt";
    const std::string modelPath = "res10_300x300_ssd_iter_140000.caffemodel";

    cv::VideoCapture video("https://raw.githubusercontent.com/intel-iot-devkit/sample-videos/master/face-demographics-walking-and-pause.mp4");
    if (!video.isOpened()) {
        std::cerr << "Video acilamadi!" << std::endl;
        return -1;
    }

    double fps = video.get(cv::CAP_PROP_FPS);
    std::cout << "Video FPS: " << fps << std::endl;

    // Thread sayısı: çekirdek - 1
    unsigned int threadSayisi = std::thread::hardware_concurrency();
    threadSayisi = (threadSayisi > 1) ? threadSayisi - 1 : 1;
    std::cout << "Thread Sayisi: " << threadSayisi << std::endl;

    KareKuyrugu  isKuyrugu;
    SonucKuyrugu sonucKuyrugu;

    // Worker thread'leri başlat
    std::vector<std::thread> threadHavuzu;
    for (unsigned int i = 0; i < threadSayisi; i++) {
        threadHavuzu.push_back(std::thread(workerThread,
            std::ref(isKuyrugu), std::ref(sonucKuyrugu),
            protoPath, modelPath));
    }

    std::cout << "Cikis icin 'q' tusuna basin." << std::endl;

    while (true) {
        cv::Mat hamKare;
        video >> hamKare;
        if (hamKare.empty()) break;

        // Kareyi iş kuyruğuna ekle
        {
            std::lock_guard<std::mutex> lock(isKuyrugu.mtx);
            isKuyrugu.kuyruk.push(hamKare.clone());
        }
        isKuyrugu.cv.notify_one();

        // Worker bitene kadar bekle → ham yüz hiç görünmez
        cv::Mat islenmis;
        {
            std::unique_lock<std::mutex> lock(sonucKuyrugu.mtx);
            sonucKuyrugu.cv.wait(lock, [&] {
                return !sonucKuyrugu.kuyruk.empty();
            });
            islenmis = sonucKuyrugu.kuyruk.front();
            sonucKuyrugu.kuyruk.pop();
        }

        // Sadece işlenmiş kareyi göster
        cv::imshow("Blurlanmis Video", islenmis);

        if (cv::waitKey(1) == 'q') break;
    }

    // Temizlik
    {
        std::lock_guard<std::mutex> lock(isKuyrugu.mtx);
        isKuyrugu.bitti = true;
    }
    isKuyrugu.cv.notify_all();
    for (auto& t : threadHavuzu) if (t.joinable()) t.join();

    video.release();
    cv::destroyAllWindows();
    return 0;
}
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

    // Bunlar global değişkenler; her thread bunlara ulaşabilecek.

int errorCount = 0;
int warningCount = 0;
int infoCount = 0;

// Bu ise bizim kapı görevlimiz (kilidimiz).
std::mutex mtx;
void logAnaliz(std::string line) {
    // 1. ERROR kontrolü
    if (line.find("ERROR") != std::string::npos) {
        std::lock_guard<std::mutex> lock(mtx); // Kilidi tak (Kritik Bölge Başlangıcı)
        errorCount++;
    } // Süslü parantez bittiği an kilit otomatik olarak AÇILIR.

    // 2. WARNING kontrolü
    else if (line.find("WARN") != std::string::npos) {
        std::lock_guard<std::mutex> lock(mtx);
        warningCount++;
    }

    // 3. INFO kontrolü
    else if (line.find("INFO") != std::string::npos) {
        std::lock_guard<std::mutex> lock(mtx);
        infoCount++;
    }
}
int main() {
    // Sistemdeki çekirdek (core) sayısını bulur
    unsigned int n = std::thread::hardware_concurrency(); 
    const std::string dosyaAdi = "Hadoop_2k.log";
    // Ödev isterine göre n-1 thread limiti belirliyoruz
    unsigned int threadLimit = (n > 1) ? n - 1 : 1; 

    std::cout << "Sistem Çekirdek Sayısı: " << n << std::endl;
    std::cout << "Kullanılacak Thread Sayısı: " << threadLimit << std::endl;

    std::cout << "Single-Thread analiz baslatiliyor..." << std::endl;
    // ---------------------------------------------------------
    // 1. AŞAMA: SINGLE-THREAD (Sıralı İşleme)
    // ---------------------------------------------------------
    auto startSingle = std::chrono::high_resolution_clock::now();
    
    std::ifstream fileS(dosyaAdi);
    if (!fileS.is_open()) { std::cerr << "Dosya acilamadi!"; return 1; }
    
    std::string line;
    while (std::getline(fileS, line)) {
        logAnaliz(line); // Direkt çağırıyoruz, thread yok
    }
    fileS.close();
    auto endSingle = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSingle = endSingle - startSingle;
    std::cout << "Single-Thread Sure: " << durationSingle.count() << " saniye.\n" << std::endl;
    // ---------------------------------------------------------
    // 2. AŞAMA: MULTI-THREAD (Paralel İşleme)
    // ---------------------------------------------------------
    std::cout << "Multi-Thread analiz baslatiliyor..." << std::endl;

    errorCount = 0;
    warningCount = 0;
    infoCount = 0;
    
    auto startMulti = std::chrono::high_resolution_clock::now();
    
    std::ifstream fileM(dosyaAdi);
    std::vector<std::thread> threadHavuzu;
    
    while (std::getline(fileM, line)) {
        threadHavuzu.push_back(std::thread(logAnaliz, line));

        if (threadHavuzu.size() >= threadLimit) {
            std::cout<<line<< std::em
            for (auto& t : threadHavuzu) {
                if (t.joinable()) t.join();
            }
            threadHavuzu.clear();
        }
    }
    // Son kalan thread'leri bekle
    for (auto& t : threadHavuzu) {
        if (t.joinable()) t.join();
    }
    fileM.close();
    auto endMulti = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationMulti = endMulti - startMulti;
    std::cout << "Multi-Thread Sure: " << durationMulti.count() << " saniye.\n" << std::endl;
    // ---------------------------------------------------------
    // 3. AŞAMA: ÇIKTI DOSYASI OLUŞTURMA
    // ---------------------------------------------------------
    std::ofstream output("output.txt");
    if (output.is_open()) {
        output << "ERROR : " << errorCount << "\n";
        output << "WARNING : " << warningCount << "\n";
        output << "INFO : " << infoCount << "\n";
        output.close();
        std::cout << "Sonuclar output.txt dosyasina kaydedildi." << std::endl;
    }

    return 0;
}


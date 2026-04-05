
🚀 Multithreaded C++ Performance Lab
Bu depo, Konya Teknik Üniversitesi bünyesindeki yüksek lisans çalışmalarım kapsamında geliştirdiğim, C++ dilinde paralel programlama ve multithreading mimarileri üzerine odaklanan projelerimi içermektedir.

📊 1. Multi-Threaded Log Analyzer
Bu çalışma, büyük ölçekli sistem log dosyalarını (Hadoop_2k.log) analiz ederek Single-Thread ve Multi-Thread yöntemleri arasındaki performans farklarını karşılaştırır.

Teknik: std::mutex ve lock_guard kullanılarak thread-safe veri yönetimi sağlanmıştır.

Kapsam: Dosya okuma ve metin tabanlı veri işleme süreçlerinde paralelleştirme başarımı ölçülmüştür.

📸 2. Real-Time Face Blurring (Gelişmiş Seviye)
Log analizindeki multithreading tecrübesini bir adım öteye taşıyarak, yoğun veri akışı olan video işleme alanına odaklanılmıştır. Bu proje, OpenCV DNN modülünü kullanarak videolardaki yüzleri gerçek zamanlı tespit eder ve bulanıklaştırır.

🛠 Öne Çıkan Özellikler
Mimarî: Görüntü yakalama ve işleme süreçleri Producer-Consumer (Üretici-Tüketici) tasarım deseniyle std::condition_variable kullanılarak senkronize edilmiştir.

Yapay Zeka: Caffe tabanlı ResNet-10 SSD modeli kullanılarak yüksek doğruluklu yüz tespiti yapılır.

Optimizasyon: MacBook M-serisi (8-thread) üzerinde sıfır gecikme (Zero Latency) sağlamak için özel bir kuyruk (queue) temizleme mantığı geliştirilmiştir.

# Projeyi derlemek için:
g++ -std=c++17 Face-Blurring/video_processor.cpp -o blur_exe `pkg-config --cflags --libs opencv4`

# Çalıştırmak için:
./blur_exe

🎓 Akademik Not
Bu projeler, Yazılım Mühendisliği temelleri üzerine inşa edilen Veri Bilimi ve Yapay Zeka uzmanlık sürecindeki teknik gelişimimi temsil etmektedir.

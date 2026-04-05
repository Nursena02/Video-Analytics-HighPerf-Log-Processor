# 🚀 Multi-Threaded Log Analyzer (C++)

Bu proje, büyük ölçekli sistem log dosyalarını hem **Single-Thread** hem de **Multi-Thread** yöntemleriyle analiz ederek performans farklarını karşılaştırır.

## 🛠 Teknik Özellikler
- **Dil:** C++11
- **Kütüphaneler:** `<thread>`, `<mutex>`, `<chrono>`
- **İşleyiş:** `std::mutex` ve `lock_guard` kullanılarak thread-safe sayaç yönetimi yapılmıştır.

## 📊 Performans Ölçümü
| Yöntem | Analiz Süresi |
| :--- | :--- |
| **Single-Thread** | ~0.002s |
| **Multi-Thread** | ~0.001s |

## 🚀 Gelecek Hedefleri
- **Video Frame Processing:** OpenCV kullanarak videoları paralel thread'lerle işlemek. (Yakında!)

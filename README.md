# 🚀 Multithreaded C++ Performance Lab

This repository showcases high-performance C++ projects developed during my Master's in Data Science and Artificial Intelligence. It focuses on parallel programming architectures, moving from text-based log analysis to real-time computer vision streams.

---

## 📸 1. Real-Time Face Blurring (Computer Vision)

This system utilizes a **Caffe-based ResNet-10 SSD** model and a **Producer-Consumer** architecture to achieve zero-latency face anonymization.

| Before (Input) | After (Output) | Performance Comparison |
| :---: | :---: | :---: |
| ![Original](./Face-Blurring/original_frame.jpg) | ![Blurred](./Face-Blurring/blurred_frame.jpg) | ![Graph](./Face-Blurring/performance_graph.png) |

> **Technical Insight:** The multithreaded architecture optimizes frame processing by utilizing an 8-threaded producer-consumer system on Apple Silicon. By synchronizing threads with `std::condition_variable` and skipping stale frames, processing time was reduced from **20ms** to **8ms**, maintaining a steady 60+ FPS.

---

## 📊 2. Multi-Threaded Log Analyzer

A high-performance tool that analyzes large-scale system logs (e.g., `Hadoop_2k.log`) using both single-threaded and multi-threaded approaches to compare efficiency.

- **Efficiency:** Parallel processing reduces execution time by approximately 50% compared to the single-threaded baseline.
- **Thread-Safety:** Implements robust counter management using `std::mutex` and `std::lock_guard`.
- **Core Stack:** Developed with C++11, utilizing `<thread>` and `<chrono>` libraries for precise performance benchmarking.

---

## 🎓 Academic Context

These projects represent my technical evolution within the software engineering domain, bridging the gap between theoretical parallel computing and practical AI implementation.










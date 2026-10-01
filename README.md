# Metanoia: ESP32 Tabanlı Lider-Takipçi M2M Otomasyon Hücresi

Endüstriyel "Pick and Place" (Al ve Bırak) süreçlerini simüle etmek amacıyla geliştirilmiş, iki adet ESP32 mikrodenetleyicisinin MQTT protokolü üzerinden haberleştiği 4 eksenli (4-DOF) robot kol ve konveyör bant sistemidir. Proje, bağımsız makinelerin internet üzerinden senkronize çalışmasını (M2M) hedefler.

* **Proje Ekibi:** Emirhan ERTÜRK, Yusuf Rasim ERTÜRK, Savaş MESTER, İbrahim Buğrahan KARATEPE
* **Geliştirme Ortamı:** Arduino IDE 2.3.7, C++

## Kullanılan Teknolojiler ve Donanımlar
* **Mikrodenetleyici:** ESP32 DevKit V1 (Uç bilişim ve Wi-Fi/MQTT haberleşmesi)
* **Haberleşme & Arayüz:** MQTT Protokolü, HiveMQ Broker, Node-RED (SCADA Dashboard)
* **Tahrik ve Sensörler:** 4x Servo Motor (MG996R), L298N Motor Sürücü, 12V DC Motor, HC-SR04 Mesafe Sensörü, 10K Potansiyometreler

## Öne Çıkan Mühendislik Çözümleri
* **Sürü Mimarisi:** Takipçi düğüm (konveyör) nesneyi algıladığında otonom olarak Lider düğüme (robot kol) bulut üzerinden görev emri iletir.
* **Sinyal Filtreleme:** Analog potansiyometrelerden kaynaklanan elektronik parazitlenmeleri (jitter) ve ağ spam'ini önlemek için 5 derecelik yazılımsal ölü bant (deadband) algoritması geliştirilmiştir.
* **İzole Güç Yönetimi:** Servo motorların yüksek demeraj akımı çekerek işlemciyi kilitlemesini önlemek için "Ortak GND, Ayrık VCC" stratejisi uygulanmıştır.

## Detaylı Proje Raporları
Sistemin elektriksel şemaları, MQTT haberleşme mimarisi (IoT) ve robot kol mekanik tasarım detaylarının yer aldığı detaylı akademik raporları incelemek için:
👉 [Nesnelerin İnterneti (IoT) Proje Raporu (PDF)](./Dokumanlar/IoT_Proje_Rapor.pdf)
👉 [Robotik Kodlama Proje Raporu (PDF)](./Dokumanlar/Robotik_Sistem_Raporu.pdf)

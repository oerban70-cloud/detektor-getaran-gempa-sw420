# Detektor Getaran Gempa SW-420 — ESP32 + Sensor Getaran + Buzzer + LED

Alat kecil yang mendeteksi getaran — gempa kecil, meja terguncang,
pintu terbanting — dan langsung membunyikan alarm supaya penghuni
rumah sadar lebih cepat. Sensor SW-420 merasakan getaran, ESP32
membaca sinyalnya, lalu buzzer berbunyi 3 detik dan LED merah menyala.
LED hijau menyala terus sebagai tanda alat sedang siaga.

Video penjelasan (60 detik):
link menyusul setelah upload

## Komponen

| Bagian | Keterangan |
|---|---|
| ESP32 DevKit | Otak alat |
| Modul sensor getaran SW-420 | Saklar getar + komparator LM393, output digital DO (LOW saat getar) |
| Buzzer aktif | Bunyi saat pin HIGH |
| LED merah + resistor 220 ohm | LED alarm |
| LED hijau + resistor 220 ohm | LED siaga |
| Catu USB 5V | Adaptor atau port laptop |

## Wiring

| Dari | Ke |
|---|---|
| SW-420 DO | ESP32 GPIO27 |
| SW-420 VCC | ESP32 3V3 |
| SW-420 GND | GND |
| Buzzer + | ESP32 GPIO26 (aktif HIGH) |
| Buzzer - | GND |
| LED merah anoda | ESP32 GPIO25, lewat resistor 220 ohm ke GND |
| LED hijau anoda | ESP32 GPIO33, lewat resistor 220 ohm ke GND |
| USB 5V | ESP32 VIN (lewat konektor USB board) |
| Semua GND | Digabung (sensor, ESP32, buzzer, LED, USB -) |

Skema lengkap: [schematic.png](schematic.png)

Perhatian daya: modul SW-420 dicatu 3V3. Jangan beri 5V ke pin DO —
pin GPIO ESP32 tidak tahan 5V. Sensitivitas getaran diatur dengan
potensiometer di modul SW-420 (putar searah jarum jam = makin sensitif).

## Cara kerja

ESP32 membaca pin DO tiap 10 milidetik. DO LOW berarti modul merasakan
getaran. Alarm hanya menyala kalau getaran bertahan minimal 50 ms
berturut-turut (5 bacaan LOW) — ketukan sekali yang sangat singkat
diabaikan (anti-bounce). Saat alarm menyala 3 detik, buzzer berbunyi,
LED merah menyala, dan LED hijau siaga padam. Setelah alarm berhenti,
ada jeda tenang 2 detik sebelum alat kembali siaga, supaya satu
guncangan tidak memicu alarm berkali-kali. Setiap kejadian yang memicu
alarm dihitung dan dicetak ke Serial Monitor (115200 baud).

## Konfigurasi (di awal sketch)

- `INTERVAL_BACA_MS` (10) — seberapa sering sensor dibaca.
- `JUMLAH_BACA_MIN` (5) — bacaan LOW berturut-turut yang dibutuhkan
  (5 x 10 ms = 50 ms) sebelum alarm menyala.
- `DURASI_ALARM_MS` (3000) — lamanya alarm menyala.
- `JEDA_TENANG_MS` (2000) — jeda tenang setelah alarm sebelum siaga lagi.

Kalau alarm terlalu mudah berbunyi, putar potensiometer modul ke arah
kurang sensitif atau naikkan `JUMLAH_BACA_MIN`; kalau getaran kecil
tidak ketahuan, putar potensiometer ke arah lebih sensitif.

## Library yang dibutuhkan

Tidak ada — sketch ini hanya memakai fungsi bawaan ESP32 core
(`digitalRead`, `millis`). Tidak perlu install library tambahan.

## Compile

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 DetektorGetaranGempaSW420
```

Terverifikasi 23 Nov 2026: sketch 292.013 byte (22% flash),
RAM 23.232 byte (7%), ESP32 core 4.0.0.

## Lisensi

MIT — lihat [LICENSE](LICENSE).

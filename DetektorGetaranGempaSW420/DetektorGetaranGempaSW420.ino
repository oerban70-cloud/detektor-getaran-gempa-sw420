// Detektor Getaran Gempa SW-420 — ESP32 + sensor getaran + buzzer + 2 LED
//
// Alat kecil yang mendeteksi getaran (mis. gempa kecil, meja terguncang)
// dan langsung membunyikan alarm supaya penghuni rumah sadar lebih cepat.
// Sensor SW-420 memakai saklar getar + komparator LM393: begitu ada
// getaran, keluarannya (DO) turun ke LOW. ESP32 membaca DO tiap 10 ms;
// alarm hanya menyala kalau getaran bertahan minimal 50 ms berturut-turut
// (anti-bounce: ketukan sekali yang sangat singkat diabaikan).
//
// Saat alarm menyala (3 detik): buzzer berbunyi, LED merah menyala,
// LED hijau siaga padam. Setelah itu ada jeda tenang 2 detik sebelum
// alat kembali siaga, supaya satu guncangan tidak memicu alarm berkali-kali.
//
// Pin (sama persis dengan schematic.png):
//   SW-420  DO  -> GPIO27, VCC -> 3V3, GND -> GND
//   Buzzer  +   -> GPIO26 (aktif HIGH), - -> GND
//   LED merah (alarm) -> GPIO25, lewat resistor 220 ohm ke GND
//   LED hijau (siaga) -> GPIO33, lewat resistor 220 ohm ke GND
//
// Sensitivitas getaran diatur dengan potensiometer di modul SW-420
// (putar searah jarum jam = makin sensitif). Modul cukup dicatu 3V3;
// jangan beri 5V ke pin DO karena pin GPIO ESP32 tidak tahan 5V.

// ---------- Pin ----------
const uint8_t PIN_SENSOR_GETAR = 27;  // DO modul SW-420 (LOW = ada getaran)
const uint8_t PIN_BUZZER       = 26;  // buzzer aktif, HIGH = bunyi
const uint8_t PIN_LED_MERAH    = 25;  // LED alarm, lewat resistor 220 ohm
const uint8_t PIN_LED_HIJAU    = 33;  // LED siaga, lewat resistor 220 ohm

// ---------- Konstanta ----------
const uint16_t INTERVAL_BACA_MS = 10;      // baca sensor tiap 10 ms
const uint8_t  JUMLAH_BACA_MIN = 5;         // 5 bacaan LOW berturut-turut = 50 ms
const uint16_t DURASI_ALARM_MS  = 3000;     // alarm menyala 3 detik
const uint16_t JEDA_TENANG_MS   = 2000;     // jeda tenang 2 detik setelah alarm

// ---------- Status ----------
bool alarmMenyala = false;                  // true selama durasi alarm
unsigned long waktuBacaTerakhir = 0;        // jadwal baca sensor berikutnya
unsigned long waktuAlarmMulai = 0;          // awal alarm yang sedang berjalan
unsigned long waktuTenangSampai = 0;        // akhir jeda tenang
uint8_t hitungGetarBerurutan = 0;           // bacaan LOW berturut-turut
uint32_t jumlahKejadian = 0;                // total getaran yang memicu alarm

void setup() {
  Serial.begin(115200);

  pinMode(PIN_SENSOR_GETAR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);

  matikanAlarm();
  digitalWrite(PIN_LED_HIJAU, HIGH);  // siaga: LED hijau menyala
  Serial.println("Detektor getaran gempa siap. LED hijau = siaga.");
}

void loop() {
  unsigned long sekarang = millis();

  // Selama alarm menyala, tunggu sampai durasinya habis.
  if (alarmMenyala) {
    if (sekarang - waktuAlarmMulai >= DURASI_ALARM_MS) {
      matikanAlarm();
      waktuTenangSampai = sekarang + JEDA_TENANG_MS;
      Serial.println("Alarm berhenti. Kembali siaga setelah jeda tenang.");
    }
    return;
  }

  // Selama jeda tenang, abaikan getaran supaya tidak retrigger.
  if (sekarang < waktuTenangSampai) {
    return;
  }

  if (sekarang - waktuBacaTerakhir < INTERVAL_BACA_MS) {
    return;  // belum waktunya baca sensor lagi
  }
  waktuBacaTerakhir = sekarang;

  // DO LOW = modul merasakan getaran.
  if (digitalRead(PIN_SENSOR_GETAR) == LOW) {
    hitungGetarBerurutan++;
  } else {
    hitungGetarBerurutan = 0;
  }

  // Baru nyalakan alarm kalau getaran bertahan cukup lama (anti-bounce).
  if (hitungGetarBerurutan >= JUMLAH_BACA_MIN) {
    hitungGetarBerurutan = 0;
    jumlahKejadian++;
    nyalakanAlarm();
    Serial.print("Getaran terdeteksi! Kejadian ke-");
    Serial.println(jumlahKejadian);
  }
}

// ---------- Alarm ----------

// Nyalakan buzzer + LED merah, padamkan LED hijau siaga.
void nyalakanAlarm() {
  alarmMenyala = true;
  waktuAlarmMulai = millis();
  digitalWrite(PIN_BUZZER, HIGH);
  digitalWrite(PIN_LED_MERAH, HIGH);
  digitalWrite(PIN_LED_HIJAU, LOW);
}

// Matikan buzzer + LED merah, nyalakan lagi LED hijau siaga.
void matikanAlarm() {
  alarmMenyala = false;
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_MERAH, LOW);
  digitalWrite(PIN_LED_HIJAU, HIGH);
}

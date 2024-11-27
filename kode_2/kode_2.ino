#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD (alamat I2C biasanya 0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin sensor kelembapan tanah dan relay
const int sensorPin = A0;   // Pin analog untuk sensor kelembapan tanah
const int relayPompa = 10;  // Pin digital untuk relay pompa air
const int relayValve = 11;  // Pin digital untuk relay solenoid valve

// Pin untuk sensor ultrasonik
const int trigPin = 8;      // Pin trigger ultrasonik
const int echoPin = 9;      // Pin echo ultrasonik

// Kalibrasi nilai minimum dan maksimum sensor kelembapan tanah
const int sensorMin = 300;  // Nilai analog saat tanah benar-benar basah
const int sensorMax = 1023; // Nilai analog saat tanah benar-benar kering

void setup() {
  // Konfigurasi pin relay
  pinMode(relayPompa, OUTPUT);
  pinMode(relayValve, OUTPUT);
  digitalWrite(relayPompa, LOW); // Matikan relay pompa di awal
  digitalWrite(relayValve, LOW); // Matikan relay valve di awal

  // Konfigurasi pin ultrasonik
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);

  // Inisialisasi Serial Monitor
  Serial.begin(9600);
  Serial.println("Sistem mulai. Membaca kelembapan tanah dan jarak...");

  // Inisialisasi LCD
  lcd.begin();
  lcd.backlight(); // Hidupkan backlight
  lcd.setCursor(0, 0);
  lcd.print("Sistem Mulai...");
  delay(2000); // Tunggu 2 detik
}

void loop() {
  // *** Membaca kelembapan tanah ***
  int moistureValue = analogRead(sensorPin);

  // Konversi kelembapan menjadi persen
  int moisturePercent = map(moistureValue, sensorMin, sensorMax, 100, 0);
  moisturePercent = constrain(moisturePercent, 0, 100); // Rentang 0-100

  // Tampilkan kelembapan ke Serial Monitor
  Serial.print("Kelembapan tanah: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // *** Membaca jarak menggunakan sensor ultrasonik ***
  long duration, distance;
  
  // Kirim sinyal trigger selama 10 mikrodetik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Hitung durasi pantulan dalam mikrodetik
  duration = pulseIn(echoPin, HIGH);
  
  // Konversi durasi ke jarak (cm)
  distance = duration * 0.034 / 2;

  // Tampilkan jarak ke Serial Monitor
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  // *** Logika kontrol relay pertama (pompa air berdasarkan kelembapan tanah) ***
  if (moisturePercent < 30) { 
    digitalWrite(relayPompa, LOW); 
    Serial.println("Pompa air ON (Tanah kering)");
  } else {
    digitalWrite(relayPompa, HIGH); 
    Serial.println("Pompa air OFF (Tanah cukup basah)");
  }

  // *** Logika kontrol relay kedua (solenoid valve berdasarkan jarak) ***
  if (distance <= 4) { 
    digitalWrite(relayValve, LOW); 
    Serial.println("Solenoid valve ON (Objek dekat)");
  } else {
    digitalWrite(relayValve, HIGH); 
    Serial.println("Solenoid valve OFF (Objek jauh)");
  }

  // *** Tampilkan data ke LCD ***
  lcd.clear(); // Bersihkan layar
  lcd.setCursor(0, 0); // Baris pertama
  lcd.print("Kelembapan:");
  lcd.print(moisturePercent);
  lcd.print("%");

  lcd.setCursor(0, 1); // Baris kedua
  lcd.print("Jarak:");
  lcd.print(distance);
  lcd.print(" cm");

  // Delay untuk stabilitas pembacaan
  delay(1000);
}

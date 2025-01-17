#include <Servo.h>

const int trigPin = 10;   // Mesafe sensörünün trig pini
const int echoPin = 9;    // Mesafe sensörünün echo pini
Servo servoMotor;         // Servo motor objesi

const int stepAngle = 5;      // Servo motorun tarama adımı (açı)
const int servoDelay = 100;   // Servo motorun hareket gecikmesi (ms)
const int maxDistance = 75;  // Maksimum mesafe (cm)

void setup() {
  Serial.begin(9600);          // Seri iletişim başlat
  pinMode(trigPin, OUTPUT);    // Trig pinini çıkış olarak ayarla
  pinMode(echoPin, INPUT);     // Echo pinini giriş olarak ayarla
  servoMotor.attach(8);        // Servo motoru dijital pin 8'e bağla
  servoMotor.write(0);         // Servo motoru başlangıç açısına getir
}

void loop() {

  for (int angle = 0; angle <= 180; angle += stepAngle) {
    moveServoAndMeasure(angle);
  }

  for (int angle = 180; angle >= 0; angle -= stepAngle) {
    moveServoAndMeasure(angle);
  }
}

// Servo motoru hareket ettir ve mesafeyi ölç
void moveServoAndMeasure(int angle) {
  servoMotor.write(angle);  // Servo motoru belirli bir açıya döndür
  delay(servoDelay);        // Hareket için gecikme

  int distance = measureDistance();  // Mesafeyi ölç

  if (distance > maxDistance ) {
    distance = 0;
  }

  // Seri porta açı ve mesafeyi yazdır
  Serial.print(angle);
  Serial.print(",");
  Serial.println(distance);
}

// Mesafe ölçüm fonksiyonu
int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // Echo süresini ölç, timeout ile
  int distance = duration * 0.034 / 2;           // Süreyi mesafeye dönüştür (cm)

  return distance;  // Ölçülen mesafeyi döndür
}

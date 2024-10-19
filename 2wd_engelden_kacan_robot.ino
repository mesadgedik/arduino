#define enA 11   // Motor A hız kontrol pini
#define enB 6    // Motor B hız kontrol pini
#define in1 10   // Motor A yön kontrol pini 1
#define in2 9    // Motor A yön kontrol pini 2
#define in3 8    // Motor B yön kontrol pini 1
#define in4 7    // Motor B yön kontrol pini 2

#define trigPin 13  // Mesafe sensörünün trig pini
#define echoPin 12  // Mesafe sensörünün eko pini

long duration; // Süre
int distance;  // Mesafe
int obstacleCount = 0;  // Engel sayısını takip etmek için

void setup() {

  // Motor pinlerini çıkış olarak ayarlayın
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Mesafe sensörü pinlerini ayarlayın
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600); // Seri iletişim başlat

}

void loop() {
  // Mesafe ölçümü yap
  measureDistance();
  
  // Mesafe güvenli mi kontrol et
  if (distance < 20) { // Mesafe 20 cm'den az ise

    obstacleCount++; // Engel sayısını artır
    makeTurn(); // Dönüş yap

  } else {

    obstacleCount = 0; // Engel yoksa sayacı sıfırla
    moveForward(); // İleri git

  }

  // Engel sayısı 30'a ulaştıysa durdur
  if (obstacleCount >= 30) {

    stopMotors(); // Motorları durdur

    while (distance < 20) {
      measureDistance(); // Güvenli mesafe sağlanana kadar mesafe ölç
    }
  }
}

// Mesafeyi ölçen fonksiyon
void measureDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // cm cinsinden mesafe

  Serial.print("Mesafe: ");
  Serial.println(distance);
}

// İleri hareket eden fonksiyon (motor yönü ters)
void moveForward() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); // Motor A'yı geriye hareket ettir
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); // Motor B'yi geriye hareket ettir
  
  analogWrite(enA, 255); // Hız ayarı
  analogWrite(enB, 255);

}

// Dönüş yapan fonksiyon
void makeTurn() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); // Motor A'yı geriye hareket ettir
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); // Motor B'yi ileri hareket ettir
  
  analogWrite(enA, 150); // Dönüş hızı ayarı
  analogWrite(enB, 150);

  delay(300); // Kısa bir süre döner

}

// Motorları durduran fonksiyon
void stopMotors() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}

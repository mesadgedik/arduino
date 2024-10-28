#define enA 11   // Motor A hız kontrol pini
#define enB 6    // Motor B hız kontrol pini
#define in1 10   // Motor A yön kontrol pini 1
#define in2 9    // Motor A yön kontrol pini 2
#define in3 8    // Motor B yön kontrol pini 1
#define in4 7    // Motor B yön kontrol pini 2

#define trigPin 13  // Mesafe sensörünün trig pini
#define echoPin 12  // Mesafe sensörünün eko pini
#define buzzerPin 3 // Buzzer pin

long duration; // Süre
int distance;  // Mesafe
char command; // Bluetooth komutu
bool manualMode = true; // Manuel kontrol modu başlangıçta açık

void setup() {
  // Motor ve buzzer pinlerini çıkış olarak ayarlayın
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Mesafe sensörü pinlerini ayarlayın
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600); // Seri iletişim başlat (Bluetooth için)
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read(); // Bluetooth komutunu oku
    controlRobot(command); // Robotu komuta göre kontrol et
  }

  // Manuel mod değilse, engel kaçma modunu başlat
  if (!manualMode) {
    measureDistance();
    buzzerControl(); // Mesafeye göre buzzer kontrolü

    // Mesafe 15 cm'den küçükse dönüş yap
    if (distance < 15) {
      makeTurn();
    } else {
      moveForward();
    }

    // Auto moddayken de manuel komutları anında algıla
    if (Serial.available() > 0) {
      command = Serial.read(); // Komutu tekrar oku
      controlRobot(command); // Manuel moda geçmeye çalış
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
  delay(100); // Ölçüm sonrası bekleme süresi artırıldı
}

// İleri hareket eden fonksiyon (motor yönü ters)
void moveForward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); // Motor A'yı ileri hareket ettir
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); // Motor B'yi ileri hareket ettir
  analogWrite(enA, 150); // Hız ayarı
  analogWrite(enB, 150);
  delay(200); // Motorların çalışması için bekleme süresi artırıldı
}

// Dönüş yapan fonksiyon
void makeTurn() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); // Motor A'yı ileri hareket ettir
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); // Motor B'yi geri hareket ettir
  analogWrite(enA, 150); // Dönüş hızı ayarı
  analogWrite(enB, 150);
  delay(300); // Dönüş süresi artırıldı
}

// Motorları durduran fonksiyon
void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // Manuel modda buzzer bir kere çalsın
  tone(buzzerPin, 1000);
  delay(300); // Kısa bip
  noTone(buzzerPin);
}

// Mesafeye göre buzzer kontrolü
void buzzerControl() {
  if (distance <= 15) {
    tone(buzzerPin, 1000); // Sürekli ses
    delay(600); // Bekleme süresi artırıldı
  } else if (distance <= 25) {
    tone(buzzerPin, 1000, 200);
    delay(500);
  } else if (distance <= 40) {
    tone(buzzerPin, 1000, 100);
    delay(700);
  } else {
    noTone(buzzerPin);
    delay(100);
  }
}

// Bluetooth komutlarına göre robotu kontrol eden fonksiyon
void controlRobot(char command) {
  if (command == 'M') {
    manualMode = true;  // Manuel moda geç
    stopMotors();  // Auto moddayken motorları durdur
  } else if (command == 'A') {
    manualMode = false; // Auto moda geç
  }

  if (manualMode) {
    switch (command) {
      case 'F': // İleri
        moveForward();
        stopMotors(); // Buzzer bir kere çalsın
        break;
      case 'B': // Geri
        moveBackward();
        stopMotors(); // Buzzer bir kere çalsın
        break;
      case 'L': // Sola dönüş
        turnLeft();
        stopMotors(); // Buzzer bir kere çalsın
        break;
      case 'R': // Sağa dönüş
        turnRight();
        stopMotors(); // Buzzer bir kere çalsın
        break;
      case 'S': // Dur
        stopMotors();
        break;
      default:
        break;
    }
  }
}

// Geri hareket eden fonksiyon
void moveBackward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW); // Motor A'yı geri hareket ettir
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); // Motor B'yi geri hareket ettir
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  delay(200); // Bekleme süresi artırıldı
}

// Sola dönen fonksiyon
void turnLeft() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); // Motor A'yı ileri hareket ettir
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); // Motor B'yi durdur
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  delay(250); // Bekleme süresi artırıldı
}

// Sağa dönen fonksiyon
void turnRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW); // Motor A'yı durdur
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); // Motor B'yi ileri hareket ettir
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  delay(250); // Bekleme süresi artırıldı
}

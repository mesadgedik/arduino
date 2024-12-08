#include <SoftwareSerial.h>

SoftwareSerial Bluetooth(9, 10); // RX = 9, TX = 10

#define enA 2
#define enB 7
#define in1 3
#define in2 4
#define in3 5
#define in4 6
#define trigPin 11
#define echoPin 12
#define buzzerPin 8

long duration;
int distance;
int obstaclesCount=0;
int motorSpeed=130;
char command;
bool manualMode = true;
bool isMovingForward = false;
bool isMovingBackward = false;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Bluetooth.begin(9600); // Bluetooth haberleşmesi
  Serial.begin(9600);    // Seri haberleşme
  Serial.println("Robot hazır!");
}

void loop() {

  if (Bluetooth.available() > 0) {
    command = Bluetooth.read();
    Serial.print("Komut alındı: ");
    Serial.println(command);
    controlRobot(command);

    if (manualMode) {
      buzzerBeep(100); // Manuel modda komut alınca kısa bip sesi
    }
  }

  if (!manualMode) 
  {
      measureDistance();
      buzzerControl();

      if (distance<10)
      {
        Serial.println("Robot güvenli mesafede değil.");
        stopMotors();
        Serial.print("Mesafe: ");
        Serial.println(distance);
      }
      else
      {

          if (distance > 35 || obstaclesCount < 5)
          {
              if (distance < 35) {
                moveBackward();
                delay(50);
                makeTurn();
                delay(100);
                obstaclesCount = obstaclesCount+1;

                Serial.print("Engel: ");
                Serial.println(obstaclesCount);
                stopMotors();
                delay(100);
              } else {
                moveForward();
                obstaclesCount=0;
              }
          }
          else
          {
            Serial.println("Robot beklemeye alındı.Engel gidince tekrar hareket edecek.");

            Serial.print("Mesafe: ");
            Serial.println(distance);

            Serial.print("Engel: ");
            Serial.println(obstaclesCount);

            stopMotors();

            if (obstaclesCount >= 5 && distance > 35)
            {
              Serial.print("Engel sıfırlandı.");
              obstaclesCount=0;
            }
          }
      }

  }
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // cm cinsinden
}

void buzzerControl() {
  if (distance <= 35) {
    tone(buzzerPin, 1000); // Sürekli bip
  } else if (distance <= 45) {
    tone(buzzerPin, 1000, 200);
    delay(300);
  } else if (distance <= 60) {
    tone(buzzerPin, 1000, 100);
    delay(500);
  } else {
    noTone(buzzerPin);
  }
}

void controlRobot(char command) {
  if (command == 'M') {
    manualMode = true;
    stopMotors();
    Serial.println("Manuel moda geçildi.");
  } else if (command == 'A') {
    manualMode = false;
    Serial.println("Otomatik moda geçildi.");
  }

  if (manualMode) {
    switch (command) {
      case 'F': // İleri gitmeye başla
        if (!isMovingBackward) {
          isMovingForward = true;
          isMovingBackward = false;
          moveForward();
        }
        else
        {
          Serial.println("Geri giderken durduruldum.");
          stopMotors();
          isMovingBackward = false;
          isMovingForward = false;
        }
        break;
      case 'B': // Geri gitmeye başla
        if (!isMovingForward) {
          isMovingBackward = true;
          isMovingForward = false;
          moveBackward();
        }
        else
        {
          Serial.println("İleri giderken durduruldum.");
          stopMotors();
          isMovingBackward = false;
          isMovingForward = false;
        }
        break;
      case 'L': // Sola dön
        if (isMovingForward || isMovingBackward) {
          turnLeft();
          delay(100);
          continueMovement();
        }
        break;
      case 'R': // Sağa dön
        if (isMovingForward || isMovingBackward) {
          turnRight();
          delay(100);
          continueMovement();
        }
        break;
      default:
        Serial.println("Geçersiz komut.");
        break;
    }
  }
}

void moveForward() {
  analogWrite(enA, motorSpeed);  // Hız motorSpeed
  analogWrite(enB, motorSpeed);  // Hız motorSpeed
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  Serial.println("İleri gidiliyor.");
}

void moveBackward() {
  analogWrite(enA, motorSpeed);  // Hız motorSpeed
  analogWrite(enB, motorSpeed);  // Hız motorSpeed
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  Serial.println("Geri gidiliyor.");
}

void turnLeft() {
  analogWrite(enA, motorSpeed);  // Hız motorSpeed
  analogWrite(enB, motorSpeed);  // Hız motorSpeed
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(200);  // Dönüş için daha uzun süre
  Serial.println("Sola dönülüyor.");
}

void turnRight() {
  analogWrite(enA, motorSpeed);  // Hız motorSpeed
  analogWrite(enB, motorSpeed);  // Hız motorSpeed
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(200);  // Dönüş için daha uzun süre
  Serial.println("Sağa dönülüyor.");
}

void makeTurn() {
  if (isMovingForward) {
    turnRight();
    delay(50);
    moveForward(); // Dönüş yaptıktan sonra ileriye gitmeye devam et
  } else if (isMovingBackward) {
    turnLeft();
    delay(50);
    moveBackward(); // Dönüş yaptıktan sonra geri gitmeye devam et
  }
}

void continueMovement() {
  if (isMovingForward) {
    moveForward();
  } else if (isMovingBackward) {
    moveBackward();
  }
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
  if(manualMode)
  {
   noTone(buzzerPin);
  }

  Serial.println("Motorlar durduruldu.");
}

void buzzerBeep(int duration) {
  tone(buzzerPin, 1000, duration);
  delay(duration);
}

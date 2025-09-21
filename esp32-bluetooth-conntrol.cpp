#include <BluetoothSerial.h>
#include <ESP32Servo.h>

// Bluetooth Serial
BluetoothSerial SerialBT;

// Servo va motor pinlari
Servo myservo;
int servoPin = 18;     // Rul
int motorPin1 = 19;    // Motor oldinga
int motorPin2 = 21;    // Motor orqaga

// Servo chegaralari
int minAngle = 45;
int maxAngle = 115;

void setup() {
  Serial.begin(115200);
  // Bluetooth nomi: "ESP32Car" (telefoningizdan shu nom bilan ko'rinadi)
  SerialBT.begin("ESP32Car");
  Serial.println("Bluetooth ishga tushdi. Qurilmaga ulanishingiz mumkin (ESP32Car).");

  // Servo
  myservo.attach(servoPin);
  myservo.write(90); // markazga qo'yish

  // Motor pinlari
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  Serial.println("Tizim tayyor.");
}

void loop() {
  // Agar telefon (yoki boshqa BT qurilma)dan buyruq kelsa, uni o'qiymiz
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n'); // yangi qatordan oldin o'qiydi
    cmd.trim(); // bosh va ohirgi bo'shliqlarni olib tashlaymiz
    Serial.print("Keldi: ");
    Serial.println(cmd);

    // Buyruqlarni tekshirish (katta/ kichik harflarga qaramay)
    if (cmd.equalsIgnoreCase("forward")) {
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      SerialBT.println("Motor: Oldinga");
      Serial.println("Motor: Oldinga");
    } else if (cmd.equalsIgnoreCase("backward")) {
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      SerialBT.println("Motor: Orqaga");
      Serial.println("Motor: Orqaga");
    } else if (cmd.equalsIgnoreCase("stop")) {
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      SerialBT.println("Motor: To'xtadi");
      Serial.println("Motor: To'xtadi");
    } else if (cmd.equalsIgnoreCase("left")) {
      myservo.write(minAngle); // Chap
      SerialBT.println("Rul: Chap");
      Serial.println("Rul: Chap");
    } else if (cmd.equalsIgnoreCase("right")) {
      myservo.write(maxAngle); // O'ng
      SerialBT.println("Rul: O'ng");
      Serial.println("Rul: O'ng");
    } else if (cmd.equalsIgnoreCase("center")) {
      myservo.write((minAngle + maxAngle) / 2); // Markaz
      SerialBT.println("Rul: Markaz");
      Serial.println("Rul: Markaz");
    } else {
      SerialBT.println("Noma'lum buyruq. Foydalaning: forward, backward, stop, left, right, center");
      Serial.println("Noma'lum buyruq.");
    }
  }
}

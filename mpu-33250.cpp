#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define MPU_ADDR_0 0x68  // AD0=GND
#define MPU_ADDR_1 0x69  // AD0=VCC (agar shunday bo'lsa)
uint8_t mpuAddr = MPU_ADDR_0;

int16_t ax, ay, az, gx, gy, gz;
float aScale = 16384.0;     // ±2g uchun LSB/g
float gScale = 131.0;       // ±250 dps uchun LSB/(°/s)

bool writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(mpuAddr);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

bool readRegs(uint8_t startReg, uint8_t count, uint8_t* dest) {
  Wire.beginTransmission(mpuAddr);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom((int)mpuAddr, (int)count, (int)true);
  for (int i = 0; i < count; i++) {
    if (!Wire.available()) return false;
    dest[i] = Wire.read();
  }
  return true;
}

bool mpuInit() {
  // Tekshirish: qaysi manzil ishlayotganini topamiz
  for (uint8_t addr : {MPU_ADDR_0, MPU_ADDR_1}) {
    mpuAddr = addr;
    Wire.beginTransmission(mpuAddr);
    if (Wire.endTransmission() == 0) break;
  }

  // Uyg'otish (PWR_MGMT_1 = 0)
  if (!writeReg(0x6B, 0x00)) return false;
  delay(100);

  // Gyro: ±250 dps (GYRO_CONFIG = 0x00)
  if (!writeReg(0x1B, 0x00)) return false;

  // Accel: ±2g (ACCEL_CONFIG = 0x00)
  if (!writeReg(0x1C, 0x00)) return false;

  // DLPF (Low-pass filter) ni yumshoq qilish: CONFIG=0x03 (~44Hz accel, ~42Hz gyro)
  if (!writeReg(0x1A, 0x03)) return false;

  // WHO_AM_I o'qib ko'ramiz (MPU6050=0x68, MPU9250=0x71)
  uint8_t who=0;
  if (!readRegs(0x75, 1, &who)) return false;
  Serial.print("WHO_AM_I: 0x"); Serial.println(who, HEX);
  return true;
}

void readMPU() {
  uint8_t buf[14];
  if (readRegs(0x3B, 14, buf)) {
    ax = (int16_t)((buf[0] << 8) | buf[1]);
    ay = (int16_t)((buf[2] << 8) | buf[3]);
    az = (int16_t)((buf[4] << 8) | buf[5]);
    // Temp (ixtiyoriy)
    // int16_t tempRaw = (int16_t)((buf[6] << 8) | buf[7]);
    gx = (int16_t)((buf[8] << 8) | buf[9]);
    gy = (int16_t)((buf[10] << 8) | buf[11]);
    gz = (int16_t)((buf[12] << 8) | buf[13]);

    // Fizik birliklarga o'tkazish
    float ax_g = ax / aScale;
    float ay_g = ay / aScale;
    float az_g = az / aScale;

    float gx_dps = gx / gScale;
    float gy_dps = gy / gScale;
    float gz_dps = gz / gScale;

    Serial.print("A[g]: ");
    Serial.print(ax_g, 3); Serial.print(", ");
    Serial.print(ay_g, 3); Serial.print(", ");
    Serial.print(az_g, 3);

    Serial.print("   G[dps]: ");
    Serial.print(gx_dps, 1); Serial.print(", ");
    Serial.print(gy_dps, 1); Serial.print(", ");
    Serial.println(gz_dps, 1);
  } else {
    Serial.println("O'qishda xatolik!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Wire.begin(I2C_SDA, I2C_SCL, 400000); // 400kHz I2C
  if (!mpuInit()) {
    Serial.println("MPU topilmadi yoki init xato!");
    while (1) delay(1000);
  } else {
    Serial.println("MPU tayyor.");
  }
}

void loop() {
  readMPU();
  delay(50); // ~20 Hz chiqish
}

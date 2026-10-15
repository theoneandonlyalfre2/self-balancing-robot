#include <Wire.h>
#include <Adafruit_BNO08x.h>

#define SDA_PIN 23
#define SCL_PIN 22

#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Buscando BNO085...");

  if (!bno08x.begin_I2C(0x4A, &Wire))
  {
    Serial.println("ERROR: BNO085 no encontrado");
    while (1)
      delay(100);
  }

  Serial.println("BNO085 encontrado!");

  if (!bno08x.enableReport(SH2_ROTATION_VECTOR))
  {
    Serial.println("Error activando Rotation Vector");
  }
}

void loop()
{
  if (bno08x.wasReset())
  {
    bno08x.enableReport(SH2_ROTATION_VECTOR);
  }

  if (bno08x.getSensorEvent(&sensorValue))
  {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
    {
      float qi = sensorValue.un.rotationVector.i;
      float qj = sensorValue.un.rotationVector.j;
      float qk = sensorValue.un.rotationVector.k;
      float qr = sensorValue.un.rotationVector.real;

      Serial.print(qr, 6);
      Serial.print(",");
      Serial.print(qi, 6);
      Serial.print(",");
      Serial.print(qj, 6);
      Serial.print(",");
      Serial.println(qk, 6);
    }
  }
}

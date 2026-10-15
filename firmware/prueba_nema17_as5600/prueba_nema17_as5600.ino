/*
  Prueba de banco: ESP32-S3 DevKitC, 2 TMC2209 y 2 AS5600.
  Arduino IDE con paquete esp32 de Espressif. Wire viene incluido.
  TMC2209 en modo standalone STEP/DIR. No requiere UART ni multiplexor.

  Motor 1: STEP GPIO4, DIR GPIO5, ENABLE GPIO6.
  Motor 2: STEP GPIO15, DIR GPIO16, ENABLE GPIO17.
  AS5600 motor 1: SDA GPIO8, SCL GPIO9 (Wire).
  AS5600 motor 2: SDA GPIO10, SCL GPIO11 (Wire1).
  Logica y buses I2C a 3.3 V. Alimentacion de motores externa.
  Ambos TMC2209: MS1 y MS2 a 3.3 V, SPREAD a GND: 1/16 de paso.
  PDN_UART a GND en esta prueba standalone; DIAG e INDEX sin conectar.

  Monitor serie: 115200 baudios.
  '1' prueba motor 1; '2' prueba motor 2; 's' interrumpe el movimiento.
  Cada prueba ordena 90 grados y luego 90 grados en sentido contrario.
  Los angulos se imprimen continuamente cuando los motores estan libres.

  Ajustar el limite de corriente de cada driver antes de conectar motores.
  Usar motores sin carga y con espacio para girar. No conectar ni desconectar
  motores con alimentacion. Esta prueba mide; no corrige posicion por encoder.
*/

#include <Wire.h>

const uint8_t STEP_PIN[2] = {4, 15};
const uint8_t DIR_PIN[2]  = {5, 16};
const uint8_t EN_PIN[2]   = {6, 17};
const uint8_t AS_ADDR  = 0x36;
bool busListo[2] = {false, false};

// Revisar la ficha de cada motor: 200 para 1.8 grados; 400 para 0.9 grados.
// Son pasos completos por vuelta, SIN multiplicar por los micropasos.
const uint16_t PASOS_VUELTA[2] = {200, 200};
const uint16_t MICROPASOS = 16; // MS1=HIGH y MS2=HIGH en ambos TMC2209.

// Lee STATUS (0x0B) y RAW ANGLE (0x0C, 0x0D).
// Cada encoder tiene su propio controlador I2C, ambos con direccion 0x36.
bool leerEncoder(uint8_t motor, float &angulo) {
  Serial.print(F("Encoder "));
  Serial.print(motor + 1);
  Serial.print(F(": "));

  if (!busListo[motor]) {
    Serial.println(F("ERROR: bus I2C no inicializado"));
    return false;
  }

  TwoWire &bus = (motor == 0) ? Wire : Wire1;
  bus.beginTransmission(AS_ADDR);
  bus.write((uint8_t)0x0B);
  if (bus.endTransmission(false) != 0) {
    Serial.println(F("ERROR: AS5600 no responde"));
    return false;
  }
  if (bus.requestFrom(AS_ADDR, (size_t)3) != 3) {
    Serial.println(F("ERROR: lectura I2C incompleta"));
    return false;
  }

  uint8_t estado = bus.read();
  uint16_t raw = (uint16_t)(bus.read() & 0x0F) << 8;
  raw |= (uint8_t)bus.read();
  bool imanCorrecto = (estado & 0x38) == 0x20;

  if (!(estado & 0x20)) Serial.print(F("IMAN NO DETECTADO "));
  if (estado & 0x10) Serial.print(F("CAMPO DEBIL "));
  if (estado & 0x08) Serial.print(F("CAMPO FUERTE "));
  if (!imanCorrecto) {
    Serial.println();
    return false;
  }

  angulo = raw * (360.0f / 4096.0f);
  Serial.print(angulo, 2);
  Serial.println(F(" grados; iman OK"));
  return true;
}

// Diferencia con correccion del cruce 359 -> 0 grados.
// Adecuada para esta prueba de 90 grados; no cuenta vueltas completas.
float diferencia(float finalAngulo, float inicioAngulo) {
  float d = finalAngulo - inicioAngulo;
  if (d > 180.0f) d -= 360.0f;
  if (d < -180.0f) d += 360.0f;
  return d;
}

bool moverCuarto(uint8_t motor, bool sentido) {
  digitalWrite(DIR_PIN[motor], sentido ? HIGH : LOW);
  delayMicroseconds(10);
  const uint32_t pasos = (uint32_t)PASOS_VUELTA[motor] * MICROPASOS / 4;
  for (uint32_t i = 0; i < pasos; ++i) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == 's' || c == 'S') {
        Serial.println(F("Prueba interrumpida; no se ordena regreso."));
        return false;
      }
    }
    digitalWrite(STEP_PIN[motor], HIGH);
    delayMicroseconds(5);
    digitalWrite(STEP_PIN[motor], LOW);
    delay(1); // Aproximadamente 1000 micropasos/s o menos, sin carga.
  }
  delay(150); // Espera a que el eje y la lectura se estabilicen.
  return true;
}

void probarMotor(uint8_t motor) {
  float inicial, anterior, actual;
  Serial.print(F("\nPRUEBA MOTOR "));
  Serial.println(motor + 1);
  if (!leerEncoder(motor, inicial)) return;

  digitalWrite(EN_PIN[motor], LOW); // TMC2209 habilitado con nivel bajo.
  delay(500); // Alineacion inicial y establecimiento de corriente.
  if (!leerEncoder(motor, inicial)) {
    digitalWrite(EN_PIN[motor], HIGH);
    return;
  }
  anterior = inicial;

  for (uint8_t tramo = 0; tramo < 2; ++tramo) {
    Serial.println(tramo == 0 ? F("Ida: 90 grados") : F("Regreso: 90 grados"));
    if (!moverCuarto(motor, tramo == 0)) break;
    if (!leerEncoder(motor, actual)) {
      Serial.println(F("Prueba detenida por error del encoder."));
      break;
    }
    Serial.print(F("Giro medido en este tramo: "));
    Serial.print(diferencia(actual, anterior), 2);
    Serial.println(F(" grados"));
    anterior = actual;
    if (tramo == 1) {
      Serial.print(F("Desviacion respecto al inicio: "));
      Serial.print(diferencia(actual, inicial), 2);
      Serial.println(F(" grados (esperado: cerca de 0)"));
    }
    delay(400);
  }

  digitalWrite(EN_PIN[motor], HIGH);
  Serial.println(F("Motor deshabilitado. Comandos: 1 o 2."));
}

void setup() {
  for (uint8_t m = 0; m < 2; ++m) {
    digitalWrite(EN_PIN[m], HIGH);
    pinMode(EN_PIN[m], OUTPUT);
    digitalWrite(STEP_PIN[m], LOW);
    pinMode(STEP_PIN[m], OUTPUT);
    digitalWrite(DIR_PIN[m], LOW);
    pinMode(DIR_PIN[m], OUTPUT);
  }
  Serial.begin(115200);
  busListo[0] = Wire.begin(8, 9, 100000);
  busListo[1] = Wire1.begin(10, 11, 100000);
  Wire.setTimeOut(25);
  Wire1.setTimeOut(25);
  delay(1000);
  Serial.println(F("ESP32-S3 + TMC2209 + AS5600."));
  Serial.println(F("Envia 1 o 2 para probar; s para parar."));
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '1' || c == '2') probarMotor(c - '1');
  }

  static unsigned long ultimoReporte = 0;
  if (millis() - ultimoReporte >= 1000) {
    ultimoReporte = millis();
    float angulo;
    leerEncoder(0, angulo);
    leerEncoder(1, angulo);
  }
  delay(1);
}

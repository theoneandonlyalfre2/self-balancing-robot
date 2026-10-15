#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <AccelStepper.h>

// ============================================================
// PINES
// ============================================================

#define SDA_PIN 23
#define SCL_PIN 22

#define STEP_PIN 18
#define DIR_PIN  19
#define EN_PIN   20

// ============================================================
// BNO085
// ============================================================

#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// ============================================================
// NEMA17 + A4988
// ============================================================

AccelStepper stepper(
    AccelStepper::DRIVER,
    STEP_PIN,
    DIR_PIN
);

// ============================================================
// CONTROL
// ============================================================

// [steps/s por grado]
float Kp = 120.0;

// Velocidad máxima permitida
const float MAX_SPEED = 10000.0;   // steps/s

// Pequeña zona muerta
const float DEADBAND = 0.20;      // grados

// Si gira al revés, cambiar a -1
const float MOTOR_SIGN = 1.0;

// ============================================================
// CALIBRACIÓN
// ============================================================

const unsigned long CALIBRATION_TIME = 3000;

bool calibrating = true;

unsigned long calibrationStart = 0;

float rollSum = 0.0;
unsigned long rollSamples = 0;

float rollZero = 0.0;

// ============================================================
// ESTADO
// ============================================================

float rollActual = 0.0;
float errorRoll = 0.0;
float speedCommand = 0.0;

bool motorEnabled = false;

// ============================================================
// QUATERNION -> ROLL
// ============================================================

float quaternionToRoll(
    float w,
    float x,
    float y,
    float z
)
{
    float sinr_cosp =
        2.0 * (w * x + y * z);

    float cosr_cosp =
        1.0 - 2.0 * (x * x + y * y);

    float roll =
        atan2(sinr_cosp, cosr_cosp);

    return roll * 180.0 / PI;
}


// ============================================================
// DIFERENCIA ANGULAR [-180,180]
// ============================================================

float angleDifference(
    float angle,
    float reference
)
{
    float difference =
        angle - reference;

    while (difference > 180.0)
        difference -= 360.0;

    while (difference < -180.0)
        difference += 360.0;

    return difference;
}


// ============================================================
// INICIAR CALIBRACIÓN
// ============================================================

void startCalibration()
{
    calibrating = true;

    calibrationStart = millis();

    rollSum = 0.0;
    rollSamples = 0;

    speedCommand = 0.0;

    stepper.setSpeed(0);

    // Desactivar motor durante calibración
    digitalWrite(EN_PIN, HIGH);

    Serial.println("CALIBRATING");
}


// ============================================================
// COMANDOS DESDE PYTHON
// ============================================================

void processCommand(String command)
{
    command.trim();

    if (command.startsWith("KP,"))
    {
        float newKp =
            command.substring(3).toFloat();

        if (newKp >= 0)
        {
            Kp = newKp;

            Serial.print("KP_SET,");
            Serial.println(Kp);
        }
    }

    else if (command == "START")
    {
        motorEnabled = true;

        if (!calibrating)
            digitalWrite(EN_PIN, LOW);

        Serial.println("MOTOR_STARTED");
    }

    else if (command == "STOP")
    {
        motorEnabled = false;

        stepper.setSpeed(0);

        digitalWrite(EN_PIN, HIGH);

        Serial.println("MOTOR_STOPPED");
    }

    else if (command == "RECAL")
    {
        startCalibration();
    }
}


// ============================================================
// LEER SERIAL SIN BLOQUEAR
// ============================================================

void readSerialCommands()
{
    static String buffer = "";

    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            processCommand(buffer);

            buffer = "";
        }
        else
        {
            buffer += c;
        }
    }
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    // --------------------------------------------------------
    // I2C
    // --------------------------------------------------------

    Wire.begin(
        SDA_PIN,
        SCL_PIN
    );

    // --------------------------------------------------------
    // BNO085
    // --------------------------------------------------------

    if (!bno08x.begin_I2C(
        0x4A,
        &Wire
    ))
    {
        Serial.println(
            "ERROR_BNO085"
        );

        while (1)
            delay(100);
    }

    // 100 Hz
    bno08x.enableReport(
        SH2_ROTATION_VECTOR,
        10000
    );

    // --------------------------------------------------------
    // A4988
    // --------------------------------------------------------

    pinMode(
        EN_PIN,
        OUTPUT
    );

    // HIGH = desactivado
    digitalWrite(
        EN_PIN,
        HIGH
    );

    // --------------------------------------------------------
    // STEPPER
    // --------------------------------------------------------

    stepper.setMaxSpeed(
        MAX_SPEED
    );

    stepper.setMinPulseWidth(
        2
    );

    // --------------------------------------------------------
    // CALIBRACIÓN
    // --------------------------------------------------------

    startCalibration();

    Serial.println(
        "SYSTEM_READY"
    );
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    // SIEMPRE llamar a runSpeed()
    stepper.runSpeed();

    // Leer órdenes Python
    readSerialCommands();

    // --------------------------------------------------------
    // BNO085
    // --------------------------------------------------------

    if (bno08x.wasReset())
    {
        bno08x.enableReport(
            SH2_ROTATION_VECTOR,
            10000
        );

        startCalibration();
    }

    if (!bno08x.getSensorEvent(
        &sensorValue
    ))
    {
        return;
    }

    if (
        sensorValue.sensorId
        != SH2_ROTATION_VECTOR
    )
    {
        return;
    }

    // --------------------------------------------------------
    // QUATERNION
    // --------------------------------------------------------

    float qi =
        sensorValue.un.rotationVector.i;

    float qj =
        sensorValue.un.rotationVector.j;

    float qk =
        sensorValue.un.rotationVector.k;

    float qr =
        sensorValue.un.rotationVector.real;

    // --------------------------------------------------------
    // ROLL
    // --------------------------------------------------------

    rollActual =
        quaternionToRoll(
            qr,
            qi,
            qj,
            qk
        );

    // ========================================================
    // CALIBRACIÓN DE 3 SEGUNDOS
    // ========================================================

    if (calibrating)
    {
        rollSum += rollActual;

        rollSamples++;

        speedCommand = 0;

        stepper.setSpeed(0);

        if (
            millis() - calibrationStart
            >= CALIBRATION_TIME
        )
        {
            if (rollSamples > 0)
            {
                rollZero =
                    rollSum /
                    rollSamples;
            }

            calibrating = false;

            Serial.print(
                "ZERO_SET,"
            );

            Serial.println(
                rollZero,
                4
            );

            if (motorEnabled)
            {
                digitalWrite(
                    EN_PIN,
                    LOW
                );
            }
        }
    }

    // ========================================================
    // CONTROL PROPORCIONAL
    // ========================================================

    else
    {
        errorRoll =
            angleDifference(
                rollActual,
                rollZero
            );

        // Zona muerta
        if (
            fabs(errorRoll)
            < DEADBAND
        )
        {
            errorRoll = 0.0;
        }

        speedCommand =
            MOTOR_SIGN *
            Kp *
            errorRoll;

        // Saturación
        speedCommand =
            constrain(
                speedCommand,
                -MAX_SPEED,
                MAX_SPEED
            );

        if (motorEnabled)
        {
            digitalWrite(
                EN_PIN,
                LOW
            );

            stepper.setSpeed(
                speedCommand
            );
        }
        else
        {
            stepper.setSpeed(0);

            digitalWrite(
                EN_PIN,
                HIGH
            );
        }
    }

    // ========================================================
    // TELEMETRÍA PARA PYTHON
    // ========================================================

    Serial.print("DATA,");

    Serial.print(qr, 6);
    Serial.print(",");

    Serial.print(qi, 6);
    Serial.print(",");

    Serial.print(qj, 6);
    Serial.print(",");

    Serial.print(qk, 6);
    Serial.print(",");

    Serial.print(rollActual, 3);
    Serial.print(",");

    Serial.print(rollZero, 3);
    Serial.print(",");

    Serial.print(errorRoll, 3);
    Serial.print(",");

    Serial.print(speedCommand, 1);
    Serial.print(",");

    Serial.print(calibrating ? 1 : 0);
    Serial.print(",");

    Serial.println(Kp, 2);
}
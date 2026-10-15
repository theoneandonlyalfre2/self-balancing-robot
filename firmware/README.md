# Firmware de ensayos

Los tres sketches corresponden a pruebas independientes. Se mantienen los archivos originales y sus asignaciones de pines, sin adaptar automáticamente el hardware.

| Sketch | Función | Dependencias |
|---|---|---|
| `bno085/bno085.ino` | Lee cuaterniones del BNO085 y transmite `w,x,y,z` | Wire; Adafruit BNO08x y dependencias instaladas por Arduino Library Manager |
| `nemamasimu/nemamasimu.ino` | Control proporcional de roll para un motor; calibración y comandos serie | Wire; Adafruit BNO08x; AccelStepper |
| `prueba_nema17_as5600/prueba_nema17_as5600.ino` | Prueba por separado dos motores con TMC2209 y lectura de AS5600 | Núcleo Arduino-ESP32 y Wire |

Abre cada `.ino` desde su carpeta del mismo nombre en Arduino IDE. Selecciona la placa y el puerto que correspondan al montaje real. Para la prueba de dos motores, consulta `prueba_nema17_as5600/CONEXIONES.md`.

`bno085` y `nemamasimu` usan SDA=23 y SCL=22. `nemamasimu` añade STEP=18, DIR=19 y EN=20 y se describe como ensayo NEMA17+A4988. Estas asignaciones deben contrastarse con la placa utilizada; no se han declarado compatibles con el ESP32-S3 de la PCB.

La prueba de TMC2209 usa dos buses I2C para los AS5600. Mide los ángulos, pero no implementa control cerrado de posición ni equilibrio del robot.

No se han cargado estos sketches ni accionado motores durante la preparación del repositorio. Las instrucciones de banco y límites de corriente deben revisarse antes de ejecutar una prueba física.

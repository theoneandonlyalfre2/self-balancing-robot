# ESP32-S3 DevKitC + 2 TMC2209 + 2 AS5600

Prueba de banco con Arduino IDE. Cada motor gira 90° y regresa, y su encoder
indica el giro medido. Se prueba un motor por vez. Usar ejes sin carga y con
espacio para girar.

Los TMC2209 se configuran por sus pines y se controlan por STEP/DIR
(modo standalone). La corriente se ajusta con el potenciómetro VREF del
módulo. Esta prueba no utiliza UART ni modifica la memoria permanente.

## Conexiones de control

Los números indican **GPIO**, no posiciones físicas del conector.
La asignación corresponde a ESP32-S3 DevKitC-1.

| Señal | Motor / encoder 1 | Motor / encoder 2 |
|---|---|---|
| STEP del TMC2209 | GPIO4 | GPIO15 |
| DIR del TMC2209 | GPIO5 | GPIO16 |
| EN / ENN del TMC2209 | GPIO6 | GPIO17 |
| SDA del AS5600 | GPIO8 | GPIO10 |
| SCL del AS5600 | GPIO9 | GPIO11 |

El pin EN de cada driver va al GPIO indicado; no al pin EN/reset de la ESP32.
Agregar 10 kΩ entre cada EN del driver y 3,3 V para mantenerlo deshabilitado
durante el arranque o reinicio de la placa.

Referencias: [pines de la DevKitC-1, Espressif](https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)
y [API I2C de Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html).

## Cada TMC2209

Usar las etiquetas y el esquema del módulo: la posición física de los pines
varía entre fabricantes. Esta conexión supone módulos tipo StepStick con
VIO y ajuste de corriente mediante potenciómetro.

| Pin | Conexión |
|---|---|
| VIO / VCC_IO | 3V3 de la ESP32 |
| GND lógico y de potencia | Tierra común |
| VM / VMOT | Positivo de fuente externa de 12 V CC |
| A1 y A2, o 1A y 1B | Los dos cables de una bobina |
| B1 y B2, o 2A y 2B | Los dos cables de la otra bobina |
| MS1 y MS2 | Ambos a 3,3 V: 1/16 de paso |
| SPREAD, si está accesible | GND: stealthChop |
| PDN_UART, si está accesible | GND para esta prueba standalone |
| DIAG e INDEX | Sin conectar |

Si hay un pin STDBY accesible, mantenerlo a GND. Si el módulo usa jumpers
para sus modos, verificar su esquema para seleccionar standalone. No
suponer que un pin marcado MS3 es SPREAD: algunos módulos lo usan para UART.

Con motor de 1,8°: 200 pasos completos × 16 = 3200 pulsos por vuelta.
Para 90°, el programa envía 800 pulsos. La interpolación interna a 256
micropasos no cambia el número de pulsos externos necesario.

Referencia: [configuración y señales del TMC2209 SilentStepStick, Watterott](https://learn.watterott.com/silentstepstick/pinconfig/tmc2209/).

## Alimentación y corriente

- Alimentar la ESP32 por USB. La fuente externa alimenta únicamente VM/VMOT.
  Unir su negativo con GND de ESP32, drivers y encoders.
- Colocar junto a cada driver un condensador electrolítico de 100 µF, 25 V o
  superior, entre VM y GND. Positivo a VM; negativo a GND. Este valor de
  tensión del condensador corresponde a una fuente de 12 V.
- No conectar 12 V a VIO, 3V3 ni a los sensores. Todos los GPIO trabajan a 3,3 V.
- Identificar bobinas con multímetro, motor desconectado y sin alimentación:
  dos cables con continuidad/resistencia finita forman una bobina. No usar
  los colores como identificación universal. La tabla supone cuatro cables.
- Ajustar la corriente antes de conectar los motores. NEMA 17 indica el
  tamaño mecánico; la corriente admisible depende del modelo del motor.
  Respetar también la capacidad térmica del módulo y su refrigeración.
- Dimensionar la fuente para el consumo de ambos drivers. No conectar ni
  desconectar motores con los drivers alimentados.

Para módulos con resistencias externas de sensado y ajustes de fábrica,
la relación aproximada en standalone es:

`I_RMS = (VREF / 2,5) × 0,325 / ((R_sense + 0,020) × sqrt(2))`

I_RMS está en amperios, VREF en voltios y R_sense en ohmios. Por ejemplo,
**solo si el módulo tiene R_sense = 0,110 Ω (R110)**, resulta
`I_RMS ≈ 0,707 × VREF`. VREF ≈ 0,71 V corresponde a unos 0,50 A RMS.
Es una conversión, no una corriente recomendada para cualquier NEMA 17.
Comprobar resistencias y manual del módulo antes de usarla.

Referencia: [ficha TMC2209, Analog Devices/Trinamic, secciones 3 y 9](https://www.analog.com/media/en/technical-documentation/data-sheets/tmc2209_datasheet_rev1.08.pdf).

## Cada AS5600

| Pin | Conexión |
|---|---|
| VCC / entrada de alimentación de 3,3 V del módulo | 3V3 de la ESP32 |
| GND | Tierra común |
| SDA y SCL | GPIO de la tabla de control |
| DIR, si está accesible | GND |
| OUT y PGO | Sin conectar |

La tabla supone módulos preparados para 3,3 V. Si la placa expone VDD5V y
VDD3V3, seguir su esquema para el modo de 3,3 V: a nivel del chip ambos se
unen a 3,3 V y se conserva el desacoplo indicado en su ficha. Una placa con
regulador puede requerir una entrada distinta; verificar sus etiquetas.

Cada bus necesita pull-up a 3,3 V en SDA y SCL. Si el módulo no las incluye,
agregar una resistencia de 4,7 kΩ de SDA a 3V3 y otra de SCL a 3V3: cuatro
resistencias en total para los dos buses. No usar pull-up a 5 V.

Ambos sensores tienen dirección 0x36, pero al usar controladores I2C separados
no hay conflicto. No unir el bus del encoder 1 con el del encoder 2.

El imán debe ser diametral, estar centrado con el chip y girar con el eje.
La placa del sensor permanece fija. Mantener cables I2C cortos y alejados
de los cables de las bobinas.

Referencia: [ficha AS5600 de ams, alojada en Mouser](https://www.mouser.com/datasheet/2/588/AS5600_DS000365_5-00-1877365.pdf).

## Carga y uso del programa

1. Abrir `prueba_nema17_as5600/prueba_nema17_as5600.ino` en Arduino IDE.
2. Usar el paquete **esp32 de Espressif Systems** y seleccionar
   **ESP32S3 Dev Module** y el puerto de la placa.
3. Para el conector USB-UART, usar `USB CDC On Boot: Disabled`.
   Si se usa el USB nativo, seleccionar `USB CDC On Boot: Enabled` y su
   puerto correspondiente. Ajustar flash/PSRAM según la variante del módulo.
4. Revisar `PASOS_VUELTA`: 200 para 1,8° o 400 para 0,9°, por motor.
   `MICROPASOS = 16` debe coincidir con MS1 y MS2 conectados a 3,3 V.
5. Cargar el programa. Wire viene incluido; no se requieren otras bibliotecas.
6. Abrir el monitor serie a **115200 baudios**. Cada segundo aparecen ambos
   ángulos. Girar lentamente cada eje a mano con los drivers deshabilitados:
   debe variar su lectura y mantenerse aproximadamente estable la del otro.
7. Con corriente ajustada, motores sin carga y espacio libre, enviar `1` para
   probar motor 1 o `2` para motor 2. Enviar un comando por vez.

El motor elegido se habilita, se toma la referencia, gira 90° y vuelve 90°.
Después queda deshabilitado. Durante el movimiento, enviar `s` para
interrumpir; no se ordena el regreso automático tras una interrupción.

Ejemplo orientativo (los signos pueden invertirse según el montaje):

```text
Ida: 90 grados
Encoder 1: 132.01 grados; iman OK
Giro medido en este tramo: 89.91 grados
Regreso: 90 grados
Encoder 1: 42.10 grados; iman OK
Giro medido en este tramo: -89.91 grados
Desviacion respecto al inicio: 0.00 grados (esperado: cerca de 0)
```

La referencia se toma después de energizar las bobinas, porque el eje puede
moverse ligeramente al habilitar el driver. No se espera un cero absoluto
inicial. El salto de 359° a 0° es normal y se corrige al comparar los tramos.

## Diagnóstico básico

- `AS5600 no responde`: revisar alimentación, SDA/SCL, pull-up y bus correcto.
- `IMAN NO DETECTADO`, `CAMPO DEBIL` o `CAMPO FUERTE`: revisar tipo, centrado
  y separación del imán. La prueba de ese motor se cancela o se detiene.
- Vibra sin girar: revisar pares de bobinas con todo apagado.
- Giro proporcionalmente incorrecto: revisar ángulo de paso del motor,
  MS1/MS2 y `MICROPASOS`. El programa supone configuración de fábrica del
  driver; si se configuró por UART, reiniciar su alimentación para recuperar
  los valores de arranque, siempre que no se haya programado OTP.
- Gira pero su ángulo no cambia: revisar que el imán gire y que el encoder
  conectado corresponda a ese motor.
- No regresa cerca del inicio: revisar carga, corriente, montaje y posible
  pérdida de pasos.

Esta prueba mide el movimiento, pero no corrige posición en lazo cerrado.
No cuenta vueltas completas ni sustituye una calibración de precisión.

Validación: compilado correctamente para ESP32S3 Dev Module con
Arduino-ESP32 3.3.11, configuración USB-UART. No probado físicamente en esta sesión.

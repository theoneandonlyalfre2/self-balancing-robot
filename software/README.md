# Interfaces de escritorio

Dos aplicaciones Python acompañan ensayos diferentes del robot. Usan comunicación serie a **115200 baudios**.

## Instalación

Desde la raíz del repositorio, con Python 3 y `pip` disponibles:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r software/requirements.txt
```

En Windows, activa el entorno con `.venv\Scripts\activate`.

El visualizador utiliza PySide6, pyqtgraph y PyOpenGL. La lista original solo incluía PySide6 y pyserial; este repositorio incorpora las dependencias adicionales que requiere su código. No se fijan versiones porque todavía no se ha validado una instalación nueva completa.

## Puerto serie

Ambas aplicaciones conservan la constante `PORT = "/dev/cu.usbserial-110"` de los archivos originales. Cambia esa constante al puerto de tu equipo antes de ejecutarlas. Puedes consultar los puertos con:

```bash
python -m serial.tools.list_ports
```

## Visualizador de orientación

1. Abre `firmware/bno085/bno085.ino` en Arduino IDE.
2. Instala las dependencias indicadas en `firmware/README.md` y revisa sus pines para tu placa.
3. Con el firmware apropiado cargado, cierra el monitor serie de Arduino.
4. Ejecuta:

```bash
python software/imu_visualizer.py
```

Espera líneas `w,x,y,z`, en ese orden, con el cuaternión de orientación. Convierte las muestras a roll, pitch y yaw y actualiza una vista 3D.

## Interfaz de control proporcional

```bash
python software/roll_controller.py
```

Se comunica con `firmware/nemamasimu/nemamasimu.ino`. Muestra roll, referencia, error y velocidad ordenada, y permite enviar `START`, `STOP`, `RECAL` y `KP,<valor>`. Esta variante es un ensayo de un motor A4988; no es el firmware integrado de la PCB de dos TMC2209.

El puerto debe existir para abrir esta interfaz. La aplicación de control no tiene un modo de simulación sin hardware.

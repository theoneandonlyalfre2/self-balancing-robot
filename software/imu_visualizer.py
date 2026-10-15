import sys
import math
import serial

from PySide6.QtCore import QTimer, Qt
from PySide6.QtGui import QFont, QVector3D, QColor
from PySide6.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QVBoxLayout,
    QHBoxLayout,
    QFrame,
)

import pyqtgraph.opengl as gl


# ============================================================
# CONFIGURACIÓN
# ============================================================

PORT = "/dev/cu.usbserial-110"
BAUDRATE = 115200


# ============================================================
# QUATERNION -> EULER
# ============================================================

def quaternion_a_euler(w, x, y, z):

    # Roll
    sinr_cosp = 2.0 * (w * x + y * z)
    cosr_cosp = 1.0 - 2.0 * (x * x + y * y)

    roll = math.atan2(
        sinr_cosp,
        cosr_cosp
    )

    # Pitch
    sinp = 2.0 * (w * y - z * x)

    sinp = max(
        -1.0,
        min(1.0, sinp)
    )

    pitch = math.asin(sinp)

    # Yaw
    siny_cosp = 2.0 * (w * z + x * y)
    cosy_cosp = 1.0 - 2.0 * (y * y + z * z)

    yaw = math.atan2(
        siny_cosp,
        cosy_cosp
    )

    return (
        math.degrees(roll),
        math.degrees(pitch),
        math.degrees(yaw)
    )


# ============================================================
# VISUALIZADOR
# ============================================================

class IMUVisualizer(QWidget):

    def __init__(self):

        super().__init__()

        self.ser = None

        # ----------------------------------------------------
        # VENTANA
        # ----------------------------------------------------

        self.setWindowTitle(
            "BNO085 - 3D IMU Visualizer"
        )

        self.resize(
            1100,
            750
        )

        layout_principal = QVBoxLayout(self)

        # ----------------------------------------------------
        # TITULO
        # ----------------------------------------------------

        titulo = QLabel(
            "BNO085 • 3D Orientation Monitor"
        )

        titulo.setAlignment(
            Qt.AlignCenter
        )

        titulo.setFont(
            QFont(
                "Arial",
                24,
                QFont.Bold
            )
        )

        layout_principal.addWidget(
            titulo
        )

        # ----------------------------------------------------
        # ESTADO SERIAL
        # ----------------------------------------------------

        self.status_label = QLabel(
            "Conectando..."
        )

        self.status_label.setAlignment(
            Qt.AlignCenter
        )

        layout_principal.addWidget(
            self.status_label
        )

        # ----------------------------------------------------
        # ANGULOS
        # ----------------------------------------------------

        layout_angulos = QHBoxLayout()

        self.roll_label = self.crear_indicador(
            "ROLL"
        )

        self.pitch_label = self.crear_indicador(
            "PITCH",
            grande=True
        )

        self.yaw_label = self.crear_indicador(
            "YAW"
        )

        layout_angulos.addWidget(
            self.roll_label
        )

        layout_angulos.addWidget(
            self.pitch_label
        )

        layout_angulos.addWidget(
            self.yaw_label
        )

        layout_principal.addLayout(
            layout_angulos
        )

        # ====================================================
        # ESCENA 3D
        # ====================================================

        self.view = gl.GLViewWidget()

        self.view.setCameraPosition(
            distance=12,
            elevation=25,
            azimuth=45
        )

        layout_principal.addWidget(
            self.view,
            1
        )

        # ----------------------------------------------------
        # GRILLA
        # ----------------------------------------------------

        grid = gl.GLGridItem()

        grid.setSize(
            20,
            20
        )

        grid.setSpacing(
            1,
            1
        )

        self.view.addItem(
            grid
        )

        # ----------------------------------------------------
        # MODELO DEL BNO085 / ROBOT
        # ----------------------------------------------------

        self.modelo = gl.GLBoxItem(
            size=QVector3D(2.0, 4.0, 0.7),
            color=QColor(255, 255, 255)
        )

        self.modelo.translate(
            -1,
            -2,
            0
        )

        self.view.addItem(
            self.modelo
        )

        # ====================================================
        # SERIAL
        # ====================================================

        self.abrir_serial()

        # ====================================================
        # TIMER
        # ====================================================

        self.timer = QTimer(self)

        self.timer.timeout.connect(
            self.actualizar_serial
        )

        self.timer.start(
            20
        )


    # ========================================================
    # CREAR TARJETAS
    # ========================================================

    def crear_indicador(
        self,
        nombre,
        grande=False
    ):

        label = QLabel(
            f"{nombre}\n0.00°"
        )

        label.setAlignment(
            Qt.AlignCenter
        )

        label.setFrameShape(
            QFrame.StyledPanel
        )

        font = QFont(
            "Arial",
            18
        )

        if grande:

            font.setPointSize(
                24
            )

            font.setBold(
                True
            )

        label.setFont(
            font
        )

        label.setMinimumHeight(
            100
        )

        return label


    # ========================================================
    # SERIAL
    # ========================================================

    def abrir_serial(self):

        try:

            self.ser = serial.Serial(
                PORT,
                BAUDRATE,
                timeout=0
            )

            self.status_label.setText(
                f"Conectado • {PORT}"
            )

        except serial.SerialException as error:

            self.status_label.setText(
                f"No se pudo abrir {PORT}: {error}"
            )

            self.ser = None


    # ========================================================
    # ROTAR MODELO
    # ========================================================

    def actualizar_modelo(
        self,
        roll,
        pitch,
        yaw
    ):

        # Reiniciar transformación
        self.modelo.resetTransform()

        # Centrar el bloque alrededor del origen
        self.modelo.translate(
            -1,
            -2,
            -0.35
        )

        # Aplicar rotaciones
        self.modelo.rotate(
            yaw,
            0,
            0,
            1
        )

        self.modelo.rotate(
            pitch,
            0,
            1,
            0
        )

        self.modelo.rotate(
            roll,
            1,
            0,
            0
        )


    # ========================================================
    # LEER SERIAL
    # ========================================================

    def actualizar_serial(self):

        if (
            self.ser is None
            or not self.ser.is_open
        ):
            return

        ultimo_dato = None

        # Limitar la cantidad de muestras procesadas
        # para no bloquear la GUI.
        for _ in range(20):

            if self.ser.in_waiting <= 0:
                break

            raw = self.ser.readline()

            if not raw:
                break

            linea = raw.decode(
                "utf-8",
                errors="ignore"
            ).strip()

            partes = linea.split(",")

            if len(partes) != 4:
                continue

            try:

                ultimo_dato = tuple(
                    map(
                        float,
                        partes
                    )
                )

            except ValueError:
                continue

        if ultimo_dato is None:
            return

        w, x, y, z = ultimo_dato

        roll, pitch, yaw = quaternion_a_euler(
            w,
            x,
            y,
            z
        )

        # ----------------------------------------------------
        # MOSTRAR ANGULOS
        # ----------------------------------------------------

        self.roll_label.setText(
            f"ROLL\n{roll:+.2f}°"
        )

        self.pitch_label.setText(
            f"PITCH\n{pitch:+.2f}°"
        )

        self.yaw_label.setText(
            f"YAW\n{yaw:+.2f}°"
        )

        # ----------------------------------------------------
        # ACTUALIZAR 3D
        # ----------------------------------------------------

        self.actualizar_modelo(
            roll,
            pitch,
            yaw
        )


    # ========================================================
    # CERRAR
    # ========================================================

    def closeEvent(
        self,
        event
    ):

        if (
            self.ser is not None
            and self.ser.is_open
        ):

            self.ser.close()

        event.accept()


# ============================================================
# MAIN
# ============================================================

if __name__ == "__main__":

    app = QApplication(
        sys.argv
    )

    ventana = IMUVisualizer()

    ventana.show()

    sys.exit(
        app.exec()
    )
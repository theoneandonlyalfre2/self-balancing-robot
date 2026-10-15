import sys
import serial

from PySide6.QtCore import QTimer, Qt
from PySide6.QtGui import QFont

from PySide6.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QPushButton,
    QDoubleSpinBox,
    QVBoxLayout,
    QHBoxLayout,
    QGridLayout,
)


# ============================================================
# CONFIGURACIÓN
# ============================================================

PORT = "/dev/cu.usbserial-110"

BAUDRATE = 115200


# ============================================================
# INTERFAZ
# ============================================================

class Controller(QWidget):

    def __init__(self):

        super().__init__()

        self.setWindowTitle(
            "Self-Balancing Robot"
        )

        self.resize(
            700,
            450
        )

        # ----------------------------------------------------
        # SERIAL
        # ----------------------------------------------------

        self.ser = serial.Serial(
            PORT,
            BAUDRATE,
            timeout=0
        )

        # ----------------------------------------------------
        # LAYOUT
        # ----------------------------------------------------

        main = QVBoxLayout(self)

        # ----------------------------------------------------
        # TITULO
        # ----------------------------------------------------

        title = QLabel(
            "NEMA17 • Roll Controller"
        )

        title.setAlignment(
            Qt.AlignCenter
        )

        title.setFont(
            QFont(
                "Arial",
                24,
                QFont.Bold
            )
        )

        main.addWidget(title)

        # ----------------------------------------------------
        # ESTADO
        # ----------------------------------------------------

        self.status = QLabel(
            "Esperando calibración..."
        )

        self.status.setAlignment(
            Qt.AlignCenter
        )

        main.addWidget(
            self.status
        )

        # ----------------------------------------------------
        # VALORES
        # ----------------------------------------------------

        grid = QGridLayout()

        self.rollLabel = QLabel(
            "Roll\n0.00°"
        )

        self.zeroLabel = QLabel(
            "Equilibrio\n0.00°"
        )

        self.errorLabel = QLabel(
            "Error\n0.00°"
        )

        self.speedLabel = QLabel(
            "Velocidad\n0 steps/s"
        )

        labels = [
            self.rollLabel,
            self.zeroLabel,
            self.errorLabel,
            self.speedLabel
        ]

        for label in labels:

            label.setAlignment(
                Qt.AlignCenter
            )

            label.setFont(
                QFont(
                    "Arial",
                    18
                )
            )

        grid.addWidget(
            self.rollLabel,
            0,
            0
        )

        grid.addWidget(
            self.zeroLabel,
            0,
            1
        )

        grid.addWidget(
            self.errorLabel,
            1,
            0
        )

        grid.addWidget(
            self.speedLabel,
            1,
            1
        )

        main.addLayout(
            grid
        )

        # ====================================================
        # KP
        # ====================================================

        kpLayout = QHBoxLayout()

        kpText = QLabel(
            "Kp:"
        )

        self.kp = QDoubleSpinBox()

        self.kp.setRange(
            0,
            1000
        )

        self.kp.setValue(
            120
        )

        self.kp.setSingleStep(
            10
        )

        self.kp.setDecimals(
            1
        )

        self.kpButton = QPushButton(
            "Enviar Kp"
        )

        self.kpButton.clicked.connect(
            self.sendKp
        )

        kpLayout.addWidget(
            kpText
        )

        kpLayout.addWidget(
            self.kp
        )

        kpLayout.addWidget(
            self.kpButton
        )

        main.addLayout(
            kpLayout
        )

        # ====================================================
        # BOTONES
        # ====================================================

        buttons = QHBoxLayout()

        self.startButton = QPushButton(
            "ACTIVAR MOTOR"
        )

        self.stopButton = QPushButton(
            "STOP"
        )

        self.recalButton = QPushButton(
            "RECALIBRAR"
        )

        self.startButton.clicked.connect(
            lambda:
            self.sendCommand("START")
        )

        self.stopButton.clicked.connect(
            lambda:
            self.sendCommand("STOP")
        )

        self.recalButton.clicked.connect(
            lambda:
            self.sendCommand("RECAL")
        )

        buttons.addWidget(
            self.startButton
        )

        buttons.addWidget(
            self.stopButton
        )

        buttons.addWidget(
            self.recalButton
        )

        main.addLayout(
            buttons
        )

        # ====================================================
        # TIMER
        # ====================================================

        self.timer = QTimer()

        self.timer.timeout.connect(
            self.readSerial
        )

        self.timer.start(
            20
        )


    # ========================================================
    # ENVIAR COMANDO
    # ========================================================

    def sendCommand(
        self,
        command
    ):

        self.ser.write(
            (
                command
                + "\n"
            ).encode()
        )


    # ========================================================
    # ENVIAR KP
    # ========================================================

    def sendKp(self):

        value = self.kp.value()

        self.sendCommand(
            f"KP,{value}"
        )


    # ========================================================
    # LEER SERIAL
    # ========================================================

    def readSerial(self):

        latest = None

        for _ in range(30):

            if self.ser.in_waiting <= 0:
                break

            line = (
                self.ser
                .readline()
                .decode(
                    "utf-8",
                    errors="ignore"
                )
                .strip()
            )

            if line.startswith(
                "DATA,"
            ):
                latest = line

        if latest is None:
            return

        parts = latest.split(",")

        if len(parts) != 11:
            return

        try:

            roll = float(
                parts[5]
            )

            zero = float(
                parts[6]
            )

            error = float(
                parts[7]
            )

            speed = float(
                parts[8]
            )

            calibrating = int(
                parts[9]
            )

        except ValueError:
            return

        # ----------------------------------------------------
        # DISPLAY
        # ----------------------------------------------------

        self.rollLabel.setText(
            f"Roll\n{roll:+.2f}°"
        )

        self.zeroLabel.setText(
            f"Equilibrio\n{zero:+.2f}°"
        )

        self.errorLabel.setText(
            f"Error\n{error:+.2f}°"
        )

        self.speedLabel.setText(
            f"Velocidad\n{speed:+.0f} steps/s"
        )

        if calibrating:

            self.status.setText(
                "CALIBRANDO • mantener inmóvil"
            )

        else:

            self.status.setText(
                "Calibración terminada"
            )


    # ========================================================
    # CERRAR
    # ========================================================

    def closeEvent(
        self,
        event
    ):

        self.sendCommand(
            "STOP"
        )

        self.ser.close()

        event.accept()


# ============================================================
# MAIN
# ============================================================

app = QApplication(
    sys.argv
)

window = Controller()

window.show()

sys.exit(
    app.exec()
)
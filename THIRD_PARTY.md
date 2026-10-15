# Componentes y atribuciones

El repositorio contiene el proyecto del robot y referencias a herramientas, módulos y bibliotecas de terceros. El diseño de una PCB que integra módulos no implica autoría de los circuitos internos de esos módulos.

- **Espressif**: familia ESP32-S3 y biblioteca de símbolos `PCM_Espressif` referenciada por el esquema.
- **Adafruit**: módulo BNO085 y biblioteca Arduino `Adafruit_BNO08x`.
- **KiCad**: bibliotecas estándar de símbolos, huellas y modelos 3D referenciadas por el proyecto. Los modelos estándar se resuelven mediante `KICAD10_3DMODEL_DIR` y no se copian íntegramente aquí.
- **AccelStepper**: dependencia del ensayo proporcional.
- **PySide6, pyserial, pyqtgraph, PyOpenGL y NumPy**: dependencias de las herramientas de escritorio.
- Los módulos TMC2209, LM2596, AS5600, MAX98357 y MAX7219 aparecen en bibliotecas, esquemas o documentación del diseño; sus marcas y diseños originales pertenecen a sus respectivos titulares.

Las dependencias de software se instalan por separado. Las referencias técnicas y sus enlaces se conservan en las notas de diseño y conexión originales. No se ha añadido una licencia general al conjunto ni se han sustituido las condiciones de terceros.

# Procedencia de los archivos

Los materiales se tomaron de las carpetas del proyecto facilitadas o identificadas en el equipo del autor:

- Circuito Self-Balancing Robot: KiCad, huellas, símbolos, STEP, soporte original 3MF y primera exportación de fabricación.
- First PCB: segunda exportación de fabricación.
- SelfBalancingRobot.kicad_sym: biblioteca de símbolos del proyecto.
- Fusion 360 / Base motores NEMA17 y Segundo piso PCB: diseños editables, exportaciones, imágenes y notas de diseño.
- self-balancing robot: las dos aplicaciones Python.
- Arduino / bno085 y nemamasimu: ensayos de IMU y control proporcional.
- Trabajo previo de prueba de motores NEMA17 con AS5600: sketch de banco e instrucciones de conexión.

Las aplicaciones se renombraron a `imu_visualizer.py` y `roll_controller.py` para facilitar su uso. Su contenido se conserva. Las piezas CAD y los diseños KiCad no se alteraron.

Se excluyeron entornos virtuales, cachés, configuraciones personales de KiCad, copias temporales, versiones previas redundantes y archivos ZIP que repetían contenido descomprimido. Los scripts de generación CAD dependientes de una sesión concreta de Fusion se conservaron en sus carpetas originales; los modelos editables y las notas de diseño se incluyen aquí.

El manifiesto registra cada archivo copiado y su hash en el momento del empaquetado. Los archivos nuevos de documentación, configuración local y vistas SVG generadas no figuran como copias de origen.

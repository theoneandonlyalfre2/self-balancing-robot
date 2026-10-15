# Estado y verificaciones

Fecha del inventario: 15 de octubre de 2026.

## Alcance

El repositorio reúne los archivos existentes del proyecto. No se cambiaron circuitos, geometrías ni algoritmos de control. Se organizaron las carpetas, se añadieron documentación y bibliotecas de símbolos locales, y se completó la lista de dependencias de las aplicaciones Python.

## Comprobaciones del empaquetado

- Los archivos originales copiados tienen su hash SHA-256 en `source-manifest.csv`.
- La sintaxis de las aplicaciones Python se valida sin abrir puertos serie ni accionar motores.
- KiCad 10.0.6 puede abrir los archivos para exportar el esquema y una vista frontal de PCB en SVG.
- Se comprueba la existencia de los destinos de los enlaces locales de la documentación nueva.

## Pendiente de comprobar

- Compilación y ejecución de cada sketch con la placa y bibliotecas elegidas.
- Funcionamiento de las aplicaciones gráficas con el hardware conectado y una instalación nueva de dependencias.
- Correspondencia entre esquemático, PCB, firmware y conexiones físicas.
- Revisión ERC/DRC, fabricación y mediciones eléctricas de la PCB.
- Ajuste del control completo, estabilidad del robot, autonomía y pruebas mecánicas.

Las descripciones de verificaciones geométricas en los LEEME de Fusion pertenecen al trabajo previo sobre esos modelos; no se repitieron durante este empaquetado.

# Plataforma PCB — segundo piso del robot

Ensamblaje independiente: **Chasis robot - segundo piso PCB**, en el proyecto **Self-Balancing Robot**. Se creó a partir de una copia de la base mecánica actual. La PCB y la base originales se conservaron sin editar.

## Piezas

| Pieza | Cantidad | Dimensiones principales |
|---|---:|---|
| Plataforma PCB | 1 | 136 × 164 × 4 mm |
| Columna entre base y plataforma | 4 | Ø8 exterior × 55 mm; paso Ø3,4 |
| Separador aislante de PCB | 4 | Ø6 exterior × 6 mm; paso Ø3,4 |

La cara inferior de la plataforma queda a 60 mm de la cara inferior de la base; la cara inferior de la PCB, a 70 mm. Los separadores largos apoyan sobre la base de 5 mm. Hay 6,7 mm libres sobre las placas de fijación de motores y 4,06 mm libres entre la plataforma y los pines inferiores más largos que aparecen en el modelo de PCB.

La plataforma contiene 30 taladros Ø3,4: cuatro para acoplarse a la base, cuatro para la PCB y **22 para futuros accesorios**. Estos últimos se sitúan fuera del contorno de la placa, con filas laterales de paso 20 mm y fijaciones en ambos extremos. Hay dos pasos de cable de 28 × 8 mm y tres ventanas de ventilación; sus esquinas son redondeadas.

## Fijación de la PCB original

Se midió un contorno de 138 × 116 mm y un sustrato de 1,44 mm en el archivo importado. La PCB se gira 90° en el ensamblaje para que sus 116 mm queden entre las ruedas. Se usan exactamente sus cuatro agujeros existentes de Ø3,2 mm; no se perforó ni se modificó la PCB.

Centros en coordenadas del robot, en mm:

| Orificio | X | Y |
|---|---:|---:|
| H1 | 40,7 | −10,2 |
| H2 | 40,7 | 39,6 |
| H3 | −54,4 | −53,0 |
| H4 | −50,4 | 34,1 |

La copia de referencia conserva los 22 cuerpos presentes en el archivo original. No representa módulos enchufables u otros elementos que no estén modelados.

## Montaje previsto

1. Colocar las cuatro columnas de 55 mm sobre los anclajes existentes de la base, en (±32,1; ±27) mm.
2. Alinear y fijar la plataforma. Tornillería prevista: cuatro M3 × 70, con cabeza arriba de la plataforma y tuerca debajo de la base. Montarlos antes de instalar la PCB. Usar arandelas; comprobar la longitud efectiva con el material y herrajes reales. Esta orientación evita que los extremos roscados sobresalgan hacia la placa.
3. Colocar los cuatro separadores aislantes de 6 mm en H1…H4.
4. Sujetar la PCB por sus agujeros actuales. Tornillería prevista: cuatro M3 × 16, con tuercas bajo la plataforma; no deformar el sustrato al apretar.
5. Llevar los cables de motores y encoders al segundo piso por los pasos de cable y el espacio libre alrededor de la PCB. Los anclajes de accesorios permanecen disponibles.

Las columnas impresas tienen taladros pasantes; no se han modelado roscas impresas. Los tornillos y las tuercas no están incluidos geométricamente en el ensamblaje. Las fijaciones de PCB deben ser aislantes respecto de las pistas.

## Archivos y edición

- `Chasis_robot_segundo_piso_PCB.f3d`: ensamblaje paramétrico con base, motores de referencia, segundo piso, separadores y copia de PCB.
- `Plataforma_PCB.step` y `.stl`: solo la plataforma.
- `Columna_55mm_cantidad_4.step` y `.stl`: una columna, fabricar cuatro unidades.
- `Separador_PCB_6mm_cantidad_4.step` y `.stl`: un separador, fabricar cuatro unidades.
- `Piezas_segundo_piso.zip`: piezas, instrucciones y datos de verificación.
- `Montaje_segundo_piso.png`, `Montaje_sin_PCB.png` y `Plataforma_PCB.png`: vistas del resultado.

Los STL entregados están centrados en XY y apoyados en Z=0, en milímetros. Los STEP y el F3D conservan la posición de montaje.

Parámetros principales de Fusion: `piso_z`, `piso_espesor`, `piso_ancho`, `piso_fondo`, `columna_diam` y `pcb_separacion`. La longitud de columnas y la altura de la copia de PCB se actualizan con ellos. Los parámetros `pcb_H1_x`…`pcb_H4_y` proceden de la medición de la PCB y deben conservarse mientras no cambie su diseño.

## Verificación y alcance

Se comprobaron los centros de fijación, las envolventes de los 22 cuerpos copiados, los sólidos cerrados, las restricciones de los bocetos y las interferencias volumétricas del conjunto. También se ensayaron temporalmente cambios de altura del piso y separación de PCB; después se restauraron 60 y 6 mm.

El diseño es un prototipo geométrico. Falta validar con las piezas físicas las tolerancias de impresión y soldadura, los herrajes reales y los componentes no modelados. La resistencia bajo carga y la compatibilidad con las futuras ruedas se revisarán al definir esas partes.

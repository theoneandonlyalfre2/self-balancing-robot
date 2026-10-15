# Base de motores NEMA 17 + AS5600

Diseño guardado en Fusion como **Base motores NEMA17 - AS5600 v1**, dentro de **Self-Balancing Robot**. Documento: `urn:adsk.wipprod:dm.lineage:snlYqbT_RESwU_-TyPqxAQ`.

## Archivos

- `Base_NEMA17_AS5600.f3d`: diseño paramétrico completo, con historial, 35 parámetros y dos motores de referencia ocultos.
- `Base_NEMA17_AS5600.step`: solo la base, un sólido.
- `Base_NEMA17_AS5600.stl`: solo la base; dimensiones expresadas en milímetros.
- `Base_NEMA17_AS5600.png`: vista de la base.
- `Montaje_referencia.png`: vista con motores simplificados.
- `verificacion.json` y `verificacion_STL.json`: comprobaciones geométricas.

## Dimensiones

| Concepto | Medida |
|---|---:|
| Base exterior, transversal × longitudinal × alto | 138,4 × 64 × 53,3 mm |
| Motor + extensión AS5600, confirmado por usuario | 44,2 mm |
| Saliente de pines, confirmado por usuario | 6,5 mm |
| Reserva tras cada encoder para pines y cable | 18 mm |
| Separación entre caras traseras de encoders | 40 mm |
| Margen entre ambas reservas de cable | 4 mm |
| Separación entre extremos de pines | 27 mm |
| Espesor de plataforma y placas frontales | 5 mm |
| Altura de ejes desde cara inferior | 27,15 mm |
| Holgura inferior bajo el motor | 1 mm |
| Patrón de tornillos de cada motor | 31 × 31 mm |
| Agujeros de paso para M3 | Ø3,4 mm |
| Alojamiento del centrador frontal | Ø22,6 mm |

Los dos motores se montan enfrentados, con ejes coaxiales hacia el exterior y encoders orientados hacia el centro. La reserva de 18 mm se mide desde la cara trasera del encoder e **incluye** el saliente de pines. La abertura central permite acceder al cableado desde abajo. Hay cuatro cartelas laterales, dos ventanas de ventilación y cuatro anclajes adicionales para la futura estructura; estos últimos no corresponden al patrón de la PCB.

## Parámetros y medidas pendientes

En Modificar → Cambiar parámetros, los principales son `motor_largo`, `cable_reserva`, `pines_salida`, `paso_NEMA`, `diam_M3`, `diam_centrador`, `fondo_base` y los espesores. La anchura total se deriva de `2*motor_largo + 2*cable_reserva + separacion_extra + 2*espesor_soporte`.

La envolvente de 42,3 mm y la fijación frontal son nominales. Confirmar en los motores reales el ancho con la extensión, el patrón roscado, el diámetro del resalte frontal y la profundidad disponible de rosca antes de fabricar. El patrón nominal de 31 mm se contrastó con el [plano del soporte NEMA17 de Pololu](https://www.pololu.com/file/0J785/2266-dimension-diagram.pdf); la envolvente de 42,3 mm se usa como referencia de la [familia NEMA17 de Pololu](https://www.pololu.com/product/1200). Estas referencias no identifican el modelo exacto del motor fotografiado.

Los componentes `02 Referencia motor … - NO FABRICAR` están ocultos y pueden mostrarse desde el navegador de Fusion. Las tapas, la división de 4 mm de la extensión, la longitud de los ejes y el bloque amarillo que representa los pines son ilustrativos. No constituyen planos de los motores o de sus encoders. Solo la base se incluyó en STEP y STL.

Esta primera base es un prototipo geométrico. No se ha realizado cálculo de resistencia ni se han definido todavía masa del robot, par de motores, ruedas o proceso de impresión. La apariencia blanca es visual y no especifica el material de fabricación.

## Verificación realizada

- Un sólido cerrado y conectado en la base.
- Los 14 bocetos de fabricación están completamente restringidos.
- Ningún error o advertencia de las operaciones de modelado.
- Ninguna interferencia volumétrica con los motores de referencia.
- Los diez agujeros de montaje y centrado se comprobaron en coordenadas 3D.
- Cambiar temporalmente el largo de motor a 45,2 mm y la reserva de cable a 20 mm actualizó correctamente la base; se restauraron los valores acordados.
- STL cerrado, 3024 triángulos, escala milimétrica comprobada y volumen consistente con el sólido CAD.

La PCB original continúa abierta y sin modificaciones.

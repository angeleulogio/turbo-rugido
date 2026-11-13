# TURBO RUGIDO

Juego de carreras arcade para Nintendo GameCube (homebrew), para 1 o 2 jugadores con pantalla dividida.
Todas las pistas, autos, sonidos y textos están generados por código: es un diseño original y no usa material de otros juegos.

## Contenido

- **3 físicas** que se eligen en el menú:
  - **ARCADE DERRAPE**: derrapes largos y estables, sin daños.
  - **ARCADE CHOQUES**: turbo que se gana arriesgando, tráfico, takedowns y choques con cámara lenta.
  - **SIMULACION**: transferencia de peso, límite de agarre, tracción trasera, delantera o 4x4, ABS y controles de tracción y estabilidad.
- **14 pistas**: CIRCUITO OCHO (con puente), FORTALEZA AZUL (castillo y jardines), VALLE DEL TUNEL (colinas y túnel), tres trazados dibujados a mano por el jugador (BURGOS, MONTE FUJI y CASTELLOLI) y dos diseñados por su hermano: SUPEROVALO (óvalo con infield, gradas y torres de luz al atardecer) y COSTA ALTA (ciudad costera que sube del mar a la montaña), y PARQUE TUCAN, el recorrido real del parque de Monterrey agrandado a escala: parque hundido entre colonias, con el cerro de roca detrás. Y cinco trazados más dibujados por el jugador y ambientados en paisajes de México: DESIERTO SONORA (saguaros y mesetas rojas), SELVA MAYA (selva, pirámide y cenotes), LOS VOLCANES (bosque de pinos y dos volcanes nevados), CAMPOS DE AGAVE (hileras de agave azul, hacienda y volcán) y BAJA CALIFORNIA (carretera entre la sierra y el mar, con cardones y un arco de roca).
- **8 autos**: FURIA R, COMETA GT, TORMENTA 4X, BRISA V12, HAYASHI INCEPTION (sedán de 4 puertas diseñado por el hermano del jugador, con tracción 4x4), y tres dibujados por el jugador: YILDIZ GT (superdeportivo en cuña de los 80) ATLAS 84 (sedán cuadrado de los 80, tracción trasera y muy fácil de derrapar) y LUTECE V10 (sedán con motor V10 de carreras, el más rápido del juego).
- 3 rivales controlados por la IA, 3 vueltas, posiciones, mejor vuelta, minimapa, pausa y pantalla de resultados.

## Controles (mando de GameCube)

| Botón | Acción |
|---|---|
| Palanca / cruceta | Dirección y menús |
| A o gatillo R (analógico) | Acelerar |
| B o gatillo L (analógico) | Frenar / marcha atrás |
| X | Turbo (modo CHOQUES) |
| Z | Cambiar cámara (cerca / lejos) |
| START | Pausa |

**Cómo derrapar:**
- En ARCADE DERRAPE: a buena velocidad, gira y toca el freno, o suelta y vuelve a pisar el acelerador. Para salir del derrape, endereza o contravolantea.
- En ARCADE CHOQUES: gira y toca el freno.

En SIMULACION los gatillos analógicos permiten acelerar y frenar de forma gradual.

## Compilar (genera `turbo_rugido.dol`)

### Opción A: en tu PC con Windows

1. Instala **devkitPro** con su instalador oficial (github.com/devkitPro/installer, sección *Releases*). Durante la instalación, marca **GameCube development**.
2. Abre la terminal **MSYS2** que instala devkitPro (aparece en el menú Inicio como *devkitPro > MSYS2*).
3. Ve a la carpeta del proyecto, por ejemplo:
   `cd /c/Users/TU_USUARIO/Desktop/turbo_rugido`
4. Escribe `make` y pulsa Enter.
5. Al terminar tendrás el archivo `turbo_rugido.dol` en la carpeta.

En Linux o macOS es igual: instala devkitPro con `dkp-pacman -S gamecube-dev` y ejecuta `make`.

### Opción B: sin instalar nada (GitHub)

Sube la carpeta a un repositorio de GitHub. El archivo `.github/workflows/compilar.yml` compila el juego de forma automática. El `.dol` aparecerá en la pestaña **Actions**, dentro de *Artifacts*.

## Jugar

- **Dolphin** (emulador): *Archivo > Abrir* y elige `turbo_rugido.dol`. Para 2 jugadores, configura dos mandos en *Opciones > Configuración de mandos*.
- **GameCube real**: copia el `.dol` a la SD y ábrelo con **Swiss** (necesitas un método para cargar homebrew, por ejemplo PicoBoot o un SD2SP2). El botón RESET de la consola vuelve a Swiss.

### ¿Y la ISO?

Para jugar, el `.dol` es equivalente: Dolphin y Swiss lo cargan directamente.
Una ISO de GameCube arrancable necesita el *apploader*, un pequeño programa de Nintendo que va en todos los discos comerciales y que este proyecto no puede incluir.

## Estructura del código

```
source/
  core.c/.h     matemáticas, mallas y sombreado plano
  track.c/.h    generación de las 3 pistas y su escenografía
  cars.c/.h     especificaciones y modelado de los autos
  vehicle.c/.h  las 3 físicas
  race.c/.h     IA, tráfico, colisiones, choques, vueltas y cámara
  font.c/.h     fuente de píxeles 5x7
  game.c/.h     menús, HUD y dibujo
  plat.h        interfaz con la plataforma
  plat_gc.c     GameCube: GX (display lists), mandos y sonido sintetizado con ASND
pc/             banco de pruebas en PC (simulación y capturas, no forma parte del juego)
```

### Ajustar la física

Los números están en `source/vehicle.c` y en `CAR_SPECS` (`source/cars.c`). Ahí se cambian velocidad máxima, aceleración, giro, facilidad para derrapar, masa, potencia, agarre y tracción.

### Pruebas en PC (opcional, Linux o MSYS2)

- `make -f Makefile.pc sim`: corre carreras automáticas en las 3 pistas con los 3 modos y muestra tiempos de vuelta.
- `make -f Makefile.pc shots`: genera capturas en `shots/`.

## Créditos

El trazado de PARQUE TUCAN se tomó de un mapa de © colaboradores de OpenStreetMap (datos bajo licencia ODbL, openstreetmap.org/copyright).

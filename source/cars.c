/* cars.c - especificaciones y modelado de autos originales (estilo low-poly) */
#include "cars.h"

const CarSpec CAR_SPECS[CAR_COUNT] = {
    /* name, desc, spd, acc, hnd, vmax, accel, turn, drift, mass, power, grip, drive, wf, shape, paint, accent */
    {"FURIA R",   "SUPERDEPORTIVO EN CUNA",  0.85f, 0.70f, 0.65f, 71.0f, 9.0f, 1.00f, 1.00f, 1250, 265000, 1.08f, 0, 0.44f, 0, {214, 32, 36, 255},  {250, 250, 250, 255}},
    {"COMETA GT", "COUPE EQUILIBRADO",       0.72f, 0.82f, 0.85f, 67.0f, 10.0f, 1.10f, 1.05f, 1180, 225000, 1.12f, 0, 0.50f, 1, {28, 96, 214, 255},  {245, 245, 250, 255}},
    {"TORMENTA 4X","COMPACTO DE RALLY",      0.60f, 0.95f, 0.95f, 63.0f, 11.0f, 1.18f, 1.20f, 1100, 205000, 1.16f, 2, 0.58f, 2, {246, 196, 26, 255}, {30, 30, 36, 255}},
    {"BRISA V12", "GRAN TURISMO DE RECTA",   0.98f, 0.55f, 0.50f, 75.0f, 8.0f, 0.92f, 0.90f, 1420, 330000, 1.04f, 0, 0.52f, 3, {120, 48, 168, 255}, {200, 200, 210, 255}},
    /* disenado por el hermano del jugador */
    {"HAYASHI INCEPTION", "SEDAN DEPORTIVO 4 PUERTAS", 0.78f, 0.74f, 0.74f, 68.5f, 9.4f, 1.02f, 1.00f, 1530, 245000, 1.08f, 2, 0.57f, 4, {150, 26, 40, 255}, {200, 202, 210, 255}},
    /* dibujado por el jugador: superdeportivo turco */
    {"YILDIZ GT", "SUPERDEPORTIVO EN CUNA 80S", 0.93f, 0.76f, 0.60f, 74.0f, 9.6f, 0.98f, 1.06f, 1300, 300000, 1.07f, 0, 0.42f, 5, {206, 18, 46, 255}, {245, 245, 245, 255}},
    /* dibujado por el jugador: sedan cuadrado de los 80, traccion trasera y facil de derrapar */
    {"ATLAS 84", "SEDAN CLASICO DE LOS 80", 0.64f, 0.70f, 0.84f, 64.0f, 9.0f, 1.10f, 1.18f, 1250, 200000, 1.10f, 0, 0.52f, 6, {64, 94, 142, 255}, {196, 198, 204, 255}},
    /* dibujado por el jugador: sedan frances con motor V10 de competicion */
    {"LUTECE V10", "SEDAN CON MOTOR V10 DE CARRERAS", 1.00f, 0.92f, 0.55f, 78.0f, 11.0f, 0.95f, 1.02f, 1350, 380000, 1.08f, 0, 0.44f, 7, {28, 58, 150, 255}, {200, 206, 216, 255}},
    /* pagina de los hermanos: silueta rosa del hermano + dibujo a lapiz del jugador */
    {"TAURO V10", "SUPERDEPORTIVO ITALIANO", 1.00f, 1.00f, 0.62f, 79.0f, 11.5f, 0.97f, 1.00f, 1380, 400000, 1.10f, 2, 0.42f, 9, {250, 168, 20, 255}, {30, 30, 34, 255}},
    {"CICLON R", "ROADSTER DESCAPOTABLE", 0.74f, 0.80f, 0.92f, 67.5f, 10.2f, 1.14f, 1.10f, 1050, 215000, 1.12f, 0, 0.48f, 10, {30, 110, 70, 255}, {236, 230, 210, 255}},
    {"RAYO 4P", "SEDAN RAPIDO DE 4 PUERTAS", 0.80f, 0.86f, 0.76f, 69.5f, 10.4f, 1.04f, 1.04f, 1480, 270000, 1.10f, 2, 0.56f, 11, {170, 176, 186, 255}, {200, 30, 30, 255}},
    {"LINCE GT", "COUPE FASTBACK 2+2", 0.82f, 0.78f, 0.86f, 70.0f, 9.8f, 1.08f, 1.06f, 1300, 255000, 1.10f, 0, 0.47f, 12, {230, 110, 30, 255}, {28, 28, 32, 255}},
    {"VIENTO SP", "SPEEDSTER CON ARCOS", 0.88f, 0.84f, 0.80f, 72.5f, 10.2f, 1.05f, 1.08f, 1150, 290000, 1.09f, 0, 0.45f, 13, {236, 236, 240, 255}, {30, 80, 200, 255}},
    {"T155", "DEPORTIVO MODERNO", 0.79f, 0.80f, 0.80f, 69.0f, 9.7f, 1.05f, 1.03f, 1450, 250000, 1.09f, 0, 0.53f, 14, {20, 96, 104, 255}, {205, 208, 216, 255}},
    /* segunda pagina de los hermanos */
    {"SIRIO GT", "COUPE DE RESISTENCIA", 0.86f, 0.80f, 0.84f, 71.5f, 9.9f, 1.07f, 1.02f, 1100, 280000, 1.11f, 0, 0.45f, 15, {190, 194, 200, 255}, {200, 30, 36, 255}},
    {"SVSR5", "DEPORTIVO DE LOS 50", 0.70f, 0.92f, 0.76f, 66.5f, 10.8f, 1.04f, 1.10f, 1150, 230000, 1.07f, 0, 0.52f, 16, {80, 190, 180, 255}, {240, 235, 215, 255}},
    {"BELFI V8 69", "MUSCLE CAR DE 1969", 0.72f, 0.95f, 0.40f, 67.5f, 11.0f, 0.94f, 1.20f, 1500, 300000, 1.03f, 0, 0.56f, 17, {30, 40, 110, 255}, {240, 240, 244, 255}},
    /* el carrito de juguete del jugador */
    {"CARRITO", "EL COCHECITO DE JUGUETE", 0.74f, 0.76f, 0.80f, 67.0f, 9.6f, 1.06f, 1.08f, 1250, 210000, 1.08f, 1, 0.60f, 18, {206, 26, 30, 255}, {24, 24, 26, 255}},
    /* tercera hoja de los hermanos: los dos primeros rinden como el TAURO V10 */
    {"HALCON R", "FASTBACK CON COLA LEVANTADA", 0.98f, 0.97f, 0.78f, 78.5f, 11.3f, 1.02f, 1.08f, 1300, 390000, 1.10f, 0, 0.48f, 19, {30, 90, 220, 255}, {20, 20, 24, 255}},
    {"TORNADO V8", "COUPE MUSCLE MODERNO", 0.97f, 1.00f, 0.66f, 78.0f, 11.5f, 0.96f, 1.15f, 1480, 410000, 1.07f, 0, 0.54f, 20, {240, 190, 20, 255}, {20, 20, 24, 255}},
    {"QUASAR", "SUPERDEPORTIVO DE MOTOR CENTRAL", 0.94f, 0.88f, 0.86f, 76.0f, 10.6f, 1.06f, 1.02f, 1250, 350000, 1.12f, 0, 0.42f, 21, {34, 34, 38, 255}, {240, 120, 20, 255}},
    /* los autos del trafico, ahora jugables (los mas lentos) */
    {"SEDAN CIUDAD", "EL SEDAN DEL TRAFICO", 0.40f, 0.45f, 0.62f, 60.0f, 8.0f, 1.00f, 1.05f, 1300, 150000, 1.05f, 1, 0.60f, 100, {120, 140, 170, 255}, {34, 34, 38, 255}},
    {"FURGONETA", "LA FURGONETA DE REPARTO", 0.30f, 0.35f, 0.40f, 55.0f, 6.8f, 0.90f, 0.90f, 2000, 140000, 1.00f, 0, 0.55f, 101, {236, 236, 236, 255}, {220, 220, 225, 255}},
    /* la patrulla de las persecuciones: pesada y estable, con torreta */
    /* formula de los 90: el que mejor acelera y maneja, pero el de menor velocidad maxima de la clase A */
    {"FORMULA 90", "MONOPLAZA DE LOS 90", 0.90f, 1.00f, 1.00f, 73.5f, 13.0f, 1.22f, 0.82f, 620, 380000, 1.32f, 0, 0.45f, 23, {214, 24, 30, 255}, {245, 245, 245, 255}},
    /* dibujo del hermano: el mejor auto del juego en las tres estadisticas */
    {"ULTIMATE R", "SUPERDEPORTIVO DEFINITIVO", 1.00f, 1.00f, 1.00f, 81.0f, 13.4f, 1.24f, 0.95f, 980, 450000, 1.33f, 2, 0.46f, 24, {24, 64, 170, 255}, {214, 218, 226, 255}},
    /* sedan compacto de los 90, el clasico de la calle: lento pero noble */
    {"GRULLA 92", "SEDAN COMPACTO DE LOS 90", 0.42f, 0.50f, 0.70f, 61.5f, 8.6f, 1.06f, 1.08f, 1000, 112000, 1.07f, 1, 0.62f, 25, {196, 26, 32, 255}, {34, 34, 38, 255}},
    /* coupe de carreras japones de los 90 (diseno original): agil y facil de derrapar */
    {"HAYATE 94", "COUPE DE CARRERAS DE LOS 90", 0.62f, 0.78f, 0.86f, 66.0f, 10.0f, 1.12f, 1.15f, 1150, 200000, 1.10f, 0, 0.52f, 26, {244, 196, 24, 255}, {20, 60, 170, 255}},
    /* hatchback compacto de 3 puertas de los 90 (diseno original): ligero y de traccion delantera */
    {"COLIBRI 3P", "HATCHBACK DE 3 PUERTAS DE LOS 90", 0.55f, 0.74f, 0.88f, 64.5f, 9.9f, 1.15f, 1.04f, 950, 170000, 1.10f, 1, 0.62f, 27, {120, 196, 44, 255}, {245, 245, 245, 255}},
    /* la furgoneta del trafico, preparada: carroceria ancha, rebajada y con aleron */
    {"FURGONETA R", "FURGONETA PREPARADA DE CARRERAS", 0.82f, 0.70f, 0.58f, 70.5f, 10.0f, 0.98f, 1.12f, 1700, 300000, 1.08f, 0, 0.55f, 28, {236, 236, 236, 255}, {214, 34, 30, 255}},
    /* sexta tanda: estilos de carreras clasicos, todos disenos originales */
    {"RUGIDO 500", "STOCK CAR DE OVALO", 0.90f, 0.76f, 0.58f, 72.5f, 10.0f, 0.96f, 1.10f, 1550, 330000, 1.05f, 0, 0.53f, 29, {26, 70, 190, 255}, {250, 206, 30, 255}},
    {"CENTELLA C", "PROTOTIPO DE RESISTENCIA GRUPO C", 0.99f, 0.92f, 0.94f, 80.0f, 11.8f, 1.15f, 0.92f, 900, 500000, 1.25f, 0, 0.42f, 30, {242, 242, 246, 255}, {20, 150, 160, 255}},
    {"ESTRELLA GT1", "GT DE CARRERAS DE LOS 90", 0.98f, 0.90f, 0.84f, 79.5f, 11.6f, 1.06f, 1.00f, 1100, 450000, 1.16f, 0, 0.44f, 31, {240, 110, 20, 255}, {24, 24, 28, 255}},
    {"BISONTE GTR", "MUSCLE CAR PREPARADO", 0.86f, 0.54f, 0.48f, 74.0f, 7.9f, 0.90f, 1.18f, 1600, 380000, 1.04f, 0, 0.56f, 32, {70, 74, 82, 255}, {240, 120, 20, 255}},
    {"GUAYINA RS", "GUAYINA DEPORTIVA 4X4", 0.80f, 0.84f, 0.76f, 70.0f, 10.4f, 1.04f, 1.02f, 1500, 280000, 1.10f, 2, 0.56f, 33, {120, 24, 40, 255}, {200, 204, 212, 255}},
    {"LANZA FF", "COUPE DE COFRE LARGO", 0.84f, 0.80f, 0.70f, 71.0f, 10.2f, 1.06f, 0.95f, 1250, 270000, 1.10f, 1, 0.62f, 34, {236, 236, 240, 255}, {30, 80, 200, 255}},
    {"TIFON 68", "PROTOTIPO GRUPO 6 DE LOS 60", 0.86f, 0.86f, 0.78f, 72.0f, 10.6f, 1.08f, 1.06f, 800, 300000, 1.12f, 0, 0.40f, 35, {236, 232, 220, 255}, {200, 30, 34, 255}},
    /* septima tanda: ideas del hermano, todos disenos originales */
    {"FULMINE 8", "GT ITALIANO BAJISIMO DE MOTOR DELANTERO", 0.95f, 0.90f, 0.60f, 77.0f, 11.0f, 0.98f, 1.15f, 1450, 420000, 1.08f, 0, 0.52f, 36, {170, 20, 30, 255}, {240, 240, 240, 255}},
    {"RAIJIN V12", "HIPERAUTO JAPONES DE MOTOR DELANTERO", 0.97f, 0.93f, 0.88f, 79.0f, 11.8f, 1.10f, 1.00f, 1300, 480000, 1.20f, 2, 0.50f, 37, {30, 30, 34, 255}, {230, 30, 40, 255}},
    {"VORTICE X", "HIPERAUTO DE RECORD", 0.99f, 0.90f, 0.70f, 80.5f, 11.4f, 1.00f, 1.05f, 1250, 500000, 1.12f, 0, 0.42f, 38, {236, 236, 240, 255}, {30, 30, 34, 255}},
    {"ESTELA V8", "DEPORTIVO AMERICANO DE COFRE LARGO", 0.90f, 0.80f, 0.78f, 73.0f, 10.4f, 1.04f, 1.10f, 1450, 300000, 1.10f, 0, 0.51f, 39, {240, 200, 20, 255}, {30, 30, 34, 255}},
    {"TURISMO 96", "SEDAN DE TURISMO DE LOS 90", 0.78f, 0.82f, 0.90f, 69.5f, 10.4f, 1.12f, 1.05f, 1150, 240000, 1.14f, 1, 0.60f, 40, {245, 245, 245, 255}, {20, 120, 60, 255}},
    {"ZAFIRO LM", "SUPERDEPORTIVO DE CARRERAS", 0.99f, 0.94f, 0.84f, 80.0f, 12.0f, 1.08f, 1.00f, 1050, 480000, 1.18f, 0, 0.42f, 41, {20, 110, 80, 255}, {240, 240, 240, 255}},
    {"FAMILIAR 95", "MINIVAN DE LOS 90", 0.32f, 0.36f, 0.50f, 58.0f, 7.6f, 0.94f, 1.00f, 1700, 120000, 1.02f, 1, 0.58f, 42, {110, 130, 150, 255}, {200, 202, 210, 255}},
    {"PATRULLA", "INTERCEPTOR DE LA POLICIA", 0.86f, 0.80f, 0.72f, 72.0f, 10.0f, 1.04f, 0.98f, 1520, 290000, 1.10f, 0, 0.52f, 8, {24, 24, 30, 255}, {245, 245, 250, 255}},
};


const CarInfo CAR_INFO[CAR_COUNT] = {
    {"V8 CENTRAL", "CUNA BAJA CON MOTOR DETRAS DEL PILOTO.", "RAPIDO EN RECTA, NERVIOSO EN CURVA.", "DISENO ORIGINAL DEL JUEGO"},
    {"6 CILINDROS", "EL COUPE MAS EQUILIBRADO DEL GARAGE.", "BUENO PARA EMPEZAR EN CUALQUIER PISTA.", "DISENO ORIGINAL DEL JUEGO"},
    {"4 CIL. TURBO", "COMPACTO DE RALLY CON TRACCION TOTAL.", "DERRAPA FACIL Y SALE DISPARADO.", "DISENO ORIGINAL DEL JUEGO"},
    {"V12", "GRAN TURISMO LARGO Y PESADO.", "EL REY DE LAS RECTAS LARGAS.", "DISENO ORIGINAL DEL JUEGO"},
    {"V6 TURBO", "SEDAN DEPORTIVO CON FRENTE CROMADO.", "CUATRO PUERTAS Y TRACCION TOTAL.", "DIBUJO DEL HERMANO"},
    {"V8", "CUNA DE LOS 80 CON FAROS ESCONDIDOS.", "PERSIANAS EN LA LUNETA Y FRANJA BLANCA.", "DIBUJO DEL JUGADOR"},
    {"6 EN LINEA", "SEDAN CUADRADO DE LOS 80.", "TRACCION TRASERA: EL MAS FACIL DE CRUZAR.", "DIBUJO DEL JUGADOR"},
    {"V10 DE CARRERAS", "SEDAN CON MOTOR DE COMPETICION.", "RAPIDISIMO EN LAS RECTAS.", "DIBUJO DEL JUGADOR"},
    {"V10 CENTRAL", "SUPERDEPORTIVO ITALIANO CON ALERON Y ALETA.", "MUY RAPIDO Y ACELERA FUERTE.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"4 CILINDROS", "ROADSTER LIGERO CON COLA DE PATO.", "EL MEJOR MANEJO DE LA PAGINA.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V6 BITURBO", "SEDAN DE 4 PUERTAS CON TOMAS GRANDES.", "TRACCION TOTAL: ACELERA MUY FUERTE.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"6 CILINDROS", "COUPE CON TECHO QUE BAJA HASTA LA COLA.", "CUATRO ASIENTOS Y BUEN EQUILIBRIO.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V8", "SPEEDSTER SIN TECHO CON DOS ARCOS.", "LIGERO, POTENTE Y DE TRACCION TRASERA.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V6 TURBO", "DEPORTIVO MODERNO DE PARRILLA GRANDE.", "FAROS RECTANGULARES Y CUATRO ESCAPES.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V8 CENTRAL", "COUPE BAJO PARA CARRERAS DE 24 HORAS.", "FAROS REDONDOS Y TECHO QUE CAE A LA COLA.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V6", "DEPORTIVO DE LOS 50 CON TECHO REDONDO.", "PARACHOQUES CROMADOS Y PINTURA DE DOS TONOS.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V8", "MUSCLE CAR FASTBACK CON COLA LEVANTADA.", "ACELERA FUERTE, PERO CUESTA EN LAS CURVAS.", "SILUETA DEL HERMANO, DIBUJO DEL JUGADOR"},
    {"V6", "EL CARRITO DE JUGUETE DEL ESCRITORIO.", "ROJO, FALDONES NEGROS Y FILETES BLANCOS.", "COPIADO DEL CARRITO DEL JUGADOR"},
    {"V8 BITURBO", "FASTBACK DE LOS 90 CON COLA LEVANTADA.", "TAN RAPIDO COMO EL TAURO V10.", "DIBUJO DE LOS HERMANOS"},
    {"V8 COMPRESOR", "COUPE CUADRADO DE DOS PUERTAS.", "EL QUE MAS ACELERA, CON FRANJAS NEGRAS.", "DIBUJO DE LOS HERMANOS"},
    {"V10 CENTRAL", "CURVAS SUAVES Y TOMAS LATERALES.", "ESCAPES REDONDOS AL CENTRO.", "DIBUJO DE LOS HERMANOS"},
    {"4 CILINDROS", "EL SEDAN DE LA CIUDAD.", "NO GANA CARRERAS, PERO SE ESCONDE BIEN.", "AUTO DEL TRAFICO"},
    {"DIESEL", "FURGONETA ALTA DE REPARTO.", "LA MAS LENTA Y PESADA DEL GARAGE.", "AUTO DEL TRAFICO"},
    {"V10 3.5 L", "MONOPLAZA DE LOS 90 CON ALERONES.", "ACELERA Y DOBLA COMO POCOS, PERO NO ES EL MAS VELOZ.", "IDEA DE LOS HERMANOS"},
    {"V12 BITURBO", "NARIZ BAJA, RAYAS AL COSTADO Y DOS ESCAPES.", "EL MEJOR DEL JUEGO: VELOCIDAD, ACELERACION Y MANEJO.", "DIBUJO DEL HERMANO"},
    {"4 CILINDROS 1.6", "SEDAN COMPACTO CUADRADO DE LOS 90.", "LIGERO Y NOBLE: FACIL DE MANEJAR.", "IDEA DE LOS HERMANOS"},
    {"6 EN LINEA TURBO", "COUPE BAJO CON ALERON Y FRANJA DE CARRERAS.", "AGIL Y FACIL DE CRUZAR EN LAS CURVAS.", "DISENO ORIGINAL DEL JUEGO"},
    {"4 CIL. 16V 1.6", "HATCHBACK CORTO CON COLA CASI VERTICAL.", "LIGERO Y NOBLE: DOBLA COMO PEQUENO KART.", "DISENO ORIGINAL DEL JUEGO"},
    {"V8 SWAP", "LA FURGONETA DEL TRAFICO, ANCHA Y REBAJADA.", "PESADA, PERO CORRE COMO DEPORTIVO.", "LA FURGONETA DEL TRAFICO, PREPARADA"},
    {"V8 DE OVALO", "COUPE LARGO DE OVALO CON REDES EN LAS VENTANAS.", "VUELA EN RECTA, PERO PESA EN CURVA.", "DISENO ORIGINAL DEL JUEGO"},
    {"V8 BITURBO", "PROTOTIPO BAJO DE CABINA CERRADA Y COLA LARGA.", "PEGADO AL PISO Y MUY VELOZ.", "DISENO ORIGINAL DEL JUEGO"},
    {"V12 DE CARRERAS", "GT ANCHO CON ALERON GIGANTE Y TOMA EN EL TECHO.", "RAPIDO Y FUERTE, HECHO PARA 24 HORAS.", "DISENO ORIGINAL DEL JUEGO"},
    {"V8 COMPRESOR", "MUSCLE CAR ANCHO CON TOMA EN EL COFRE.", "EL MAS DIFICIL DE LA CLASE A.", "DISENO ORIGINAL DEL JUEGO"},
    {"5 CIL. TURBO", "GUAYINA BAJA CON ALERON EN EL TECHO.", "TRACCION TOTAL Y LUGAR PARA LA FAMILIA.", "DISENO ORIGINAL DEL JUEGO"},
    {"4 CIL. TURBO", "COUPE DE COFRE LARGO Y ALERON ALTO.", "TRACCION DELANTERA: SEGURO EN LAS CURVAS.", "DISENO ORIGINAL DEL JUEGO"},
    {"V8 DE CARRERAS", "PROTOTIPO DE LOS 60 CON COLA LARGA CORTADA.", "LIGERISIMO: ACELERA Y DOBLA CON GANAS.", "DISENO ORIGINAL DEL JUEGO"},
    {"8 EN LINEA", "COFRE LARGUISIMO Y CABINA PEGADA A LA COLA.", "BRUTAL EN RECTA, BRAVO EN LAS CURVAS.", "IDEA DEL HERMANO"},
    {"V12", "MOTOR DELANTERO Y ALERON GIGANTE DE MONTANA.", "TRACCION TOTAL: PEGADO AL PISO.", "IDEA DEL HERMANO"},
    {"V8 BITURBO", "MOTOR CENTRAL, ANCHO Y BAJISIMO.", "EL MAS VELOZ DESPUES DEL ULTIMATE R.", "IDEA DEL HERMANO"},
    {"V8", "COFRE LARGO, FAROS ESCONDIDOS Y COLA FASTBACK.", "RAPIDO Y FACIL DE MANEJAR.", "IDEA DEL HERMANO"},
    {"4 CIL. 2.0", "SEDAN DE TURISMO CON ALERON Y LABIO.", "TRACCION DELANTERA: CLAVADO EN LAS CURVAS.", "IDEA DEL HERMANO"},
    {"V12 CENTRAL", "TOMA EN EL TECHO Y ALERON DE CARRERAS.", "LIGERO, VELOZ Y MUY ESTABLE.", "IDEA DEL HERMANO"},
    {"4 CIL. 2.4", "MINIVAN FAMILIAR DE SIETE ASIENTOS.", "LENTA, PERO LLEVA A TODA LA FAMILIA.", "IDEA DEL HERMANO"},
    {"V8 POLICIA", "INTERCEPTOR CON DEFENSA DE EMPUJE.", "EL MISMO AUTO QUE TE PERSIGUE.", "DISENO ORIGINAL DEL JUEGO"},
};

/* clases: A desde ~74 m/s (266 km/h), B de 69 a 73, C el resto */
const char CAR_CLASS[CAR_COUNT] = {
    'B', 'C', 'C', 'A', 'C', 'A', 'C', 'A',      /* FURIA R .. LUTECE V10 */
    'A', 'C', 'B', 'B', 'B', 'B',                /* TAURO V10, CICLON R, RAYO 4P, LINCE GT, VIENTO SP, T155 */
    'B', 'C', 'C', 'C',                          /* SIRIO GT, SVSR5, BELFI V8 69, CARRITO */
    'A', 'A', 'A',                               /* HALCON R, TORNADO V8, QUASAR */
    'C', 'C',                                    /* SEDAN CIUDAD, FURGONETA */
    'A',                                         /* FORMULA 90 */
    'A',                                         /* ULTIMATE R */
    'C',                                         /* GRULLA 92 */
    'C',                                         /* HAYATE 94 */
    'C',                                         /* COLIBRI 3P */
    'B',                                         /* FURGONETA R */
    'B', 'A', 'A', 'A', 'B', 'B',                /* RUGIDO 500, CENTELLA C, ESTRELLA GT1, BISONTE GTR, GUAYINA RS, LANZA FF */
    'B',                                         /* TIFON 68 */
    'A', 'A', 'A', 'B', 'B', 'A', 'C',           /* FULMINE 8, RAIJIN V12, VORTICE X, ESTELA V8, TURISMO 96, ZAFIRO LM, FAMILIAR 95 */
    'B'                                          /* PATRULLA */
};

/* super patrulla: superdeportivo de motor central con la pintura de la policia */
const CarSpec SUPER_PATRULLA = {"SUPER PATRULLA", "INTERCEPTOR DE ALTA VELOCIDAD", 1.0f, 1.0f, 0.9f, 82.0f, 12.5f, 1.08f, 1.0f, 1300, 430000, 1.15f, 2, 0.45f, 22, {20, 20, 26, 255}, {245, 245, 250, 255}};

const CarSpec *car_spec(int i) { return i == CAR_SUPERPOLICE ? &SUPER_PATRULLA : &CAR_SPECS[i]; }

const Col CAR_ALT_PAINTS[6] = {
    {30, 160, 90, 255}, {240, 120, 30, 255}, {230, 230, 235, 255},
    {40, 40, 48, 255},  {60, 190, 220, 255}, {200, 40, 120, 255}
};

typedef struct { float z, ws, yb, ys, yt; } Sec;

static void section_pts(const Sec *s, V3 out[6]) {
    float wb = s->ws * 0.92f;
    out[0] = v3(-wb, s->yb, s->z);
    out[1] = v3(wb, s->yb, s->z);
    out[2] = v3(s->ws, s->ys, s->z);
    out[3] = v3(s->ws * 0.82f, s->yt, s->z);
    out[4] = v3(-s->ws * 0.82f, s->yt, s->z);
    out[5] = v3(-s->ws, s->ys, s->z);
}

static void body(Mesh *m, const Sec *sec, int ns, Col paint, Col dark) {
    int i, k;
    for (i = 0; i < ns - 1; i++) {
        V3 a[6], b[6];
        section_pts(&sec[i], a);
        section_pts(&sec[i + 1], b);
        for (k = 0; k < 6; k++) {
            int k2 = (k + 1) % 6;
            Col c = (k == 0) ? dark : paint;
            mesh_quad(m, a[k], b[k], b[k2], a[k2], c);
        }
    }
    /* tapas delantera y trasera */
    {
        V3 a[6], b[6];
        section_pts(&sec[0], a);
        section_pts(&sec[ns - 1], b);
        for (k = 1; k < 5; k++) {
            mesh_tri(m, a[0], a[k], a[k + 1], paint);
            mesh_tri(m, b[0], b[k + 1], b[k], paint);
        }
    }
}

static void cabin(Mesh *m, float z0, float z1, float ybase, float yroof, float wbase, float wtop,
                  float rake_f, float rake_r, Col paint, Col glass) {
    V3 b0 = v3(-wbase, ybase, z0), b1 = v3(wbase, ybase, z0), b2 = v3(wbase, ybase, z1), b3 = v3(-wbase, ybase, z1);
    V3 t0 = v3(-wtop, yroof, z0 + rake_f), t1 = v3(wtop, yroof, z0 + rake_f);
    V3 t2 = v3(wtop, yroof, z1 - rake_r), t3 = v3(-wtop, yroof, z1 - rake_r);
    mesh_quad(m, b0, b1, t1, t0, glass);          /* parabrisas */
    mesh_quad(m, b2, b3, t3, t2, glass);          /* luneta */
    mesh_quad(m, b1, b2, t2, t1, glass);          /* lateral derecho */
    mesh_quad(m, b3, b0, t0, t3, glass);          /* lateral izquierdo */
    mesh_quad(m, t0, t1, t2, t3, paint);          /* techo */
    /* pilares */
    {
        V3 mid0 = vlerp(b1, b2, 0.5f), mid1 = vlerp(t1, t2, 0.5f);
        V3 n0 = vlerp(b0, b3, 0.5f), n1 = vlerp(t0, t3, 0.5f);
        mesh_quad(m, vadd(mid0, v3(0.01f, 0, -0.07f)), vadd(mid0, v3(0.01f, 0, 0.07f)), vadd(mid1, v3(0.01f, 0, 0.07f)), vadd(mid1, v3(0.01f, 0, -0.07f)), paint);
        mesh_quad(m, vadd(n0, v3(-0.01f, 0, -0.07f)), vadd(n0, v3(-0.01f, 0, 0.07f)), vadd(n1, v3(-0.01f, 0, 0.07f)), vadd(n1, v3(-0.01f, 0, -0.07f)), paint);
    }
}

int g_wheel_first[4], g_wheel_count[4];
static int g_wheel_n;

static void wheel(Mesh *m, float x, float z, float r, float w) {
    int k, S = 8;
    int first = m->n;
    float side = x > 0 ? 1.0f : -1.0f;
    float xi = x - side * w * 0.5f, xo = x + side * w * 0.5f;
    Col tire = col(26, 26, 28), hub = col(180, 180, 188);
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 p0 = v3(xi, r + sinf(a0) * r, z + cosf(a0) * r), p1 = v3(xi, r + sinf(a1) * r, z + cosf(a1) * r);
        V3 q0 = v3(xo, p0.y, p0.z), q1 = v3(xo, p1.y, p1.z);
        mesh_quad(m, p0, p1, q1, q0, tire);
        mesh_tri(m, v3(xo, r, z), q0, q1, (k & 1) ? hub : col_shade(hub, 0.8f));
    }
    if (g_wheel_n < 4) { g_wheel_first[g_wheel_n] = first; g_wheel_count[g_wheel_n] = m->n - first; g_wheel_n++; }
}

void wheel_build_mesh(Mesh *m) {
    int i;
    mesh_init(m);
    g_wheel_n = 4;
    wheel(m, 0.14f, 0, 0.35f, 0.28f);
    for (i = 0; i < m->n; i++) { m->v[i].x -= 0.14f; m->v[i].y -= 0.35f; }
}

static void lights(Mesh *m, float zf, float zr, float y, float wx, float yr) {
    Col head = col(255, 246, 200), tail = col(230, 20, 20);
    mesh_quad_flat(m, v3(-wx, y, zf - 0.01f), v3(-wx + 0.38f, y, zf - 0.01f), v3(-wx + 0.38f, y + 0.12f, zf - 0.01f), v3(-wx, y + 0.12f, zf - 0.01f), head);
    mesh_quad_flat(m, v3(wx - 0.38f, y, zf - 0.01f), v3(wx, y, zf - 0.01f), v3(wx, y + 0.12f, zf - 0.01f), v3(wx - 0.38f, y + 0.12f, zf - 0.01f), head);
    mesh_quad_flat(m, v3(-wx, yr, zr + 0.01f), v3(-wx + 0.45f, yr, zr + 0.01f), v3(-wx + 0.45f, yr + 0.14f, zr + 0.01f), v3(-wx, yr + 0.14f, zr + 0.01f), tail);
    mesh_quad_flat(m, v3(wx - 0.45f, yr, zr + 0.01f), v3(wx, yr, zr + 0.01f), v3(wx, yr + 0.14f, zr + 0.01f), v3(wx - 0.45f, yr + 0.14f, zr + 0.01f), tail);
}

static void stripe(Mesh *m, const Sec *sec, int ns, float hw, Col c) {
    int i;
    for (i = 0; i < ns - 1; i++) {
        float y0 = sec[i].yt + 0.012f, y1 = sec[i + 1].yt + 0.012f;
        mesh_quad_flat(m, v3(-hw, y0, sec[i].z), v3(hw, y0, sec[i].z), v3(hw, y1, sec[i + 1].z), v3(-hw, y1, sec[i + 1].z), c);
    }
}

void car_build_mesh(Mesh *m, int shape, Col paint, Col accent) {
    Col dark = col(30, 30, 34), glass = col(36, 52, 80);
    if (shape >= 100) {   /* autos del trafico: misma malla que en la calle */
        traffic_build_mesh(m, shape - 100, paint);
        return;
    }
    if (shape == 22) {
        /* super patrulla: el superdeportivo QUASAR en negro, puertas blancas, torreta baja y defensa */
        Col white = accent, rojo = col(220, 30, 40), azul = col(30, 80, 230);
        car_build_mesh(m, 21, paint, white);
        mesh_box(m, v3(-1.015f, 0.34f, -1.45f), v3(-0.995f, 0.80f, -0.30f), white, white);
        mesh_box(m, v3(0.995f, 0.34f, -1.45f), v3(1.015f, 0.80f, -0.30f), white, white);
        mesh_box(m, v3(-0.55f, 1.10f, -0.10f), v3(0.55f, 1.14f, 0.24f), col(30, 30, 36), col(30, 30, 36));
        mesh_box(m, v3(-0.50f, 1.14f, -0.06f), v3(-0.03f, 1.24f, 0.20f), rojo, rojo);
        mesh_box(m, v3(0.03f, 1.14f, -0.06f), v3(0.50f, 1.24f, 0.20f), azul, azul);
        mesh_box(m, v3(-0.75f, 0.18f, -2.42f), v3(0.75f, 0.42f, -2.30f), col(30, 30, 36), col(30, 30, 36));   /* defensa de empuje */
        mesh_box(m, v3(-0.86f, 0.46f, -2.32f), v3(-0.62f, 0.52f, -2.30f), azul, azul);   /* luces en la parrilla */
        mesh_box(m, v3(0.62f, 0.46f, -2.32f), v3(0.86f, 0.52f, -2.30f), rojo, rojo);
        return;
    }
    if (shape == 8) {
        /* patrulla: carroceria del coupe en negro, puertas blancas y base de la torreta */
        Col white = accent;
        car_build_mesh(m, 1, paint, paint);
        mesh_box(m, v3(-0.995f, 0.42f, -0.55f), v3(-0.975f, 0.84f, 1.05f), white, white);
        mesh_box(m, v3(0.975f, 0.42f, -0.55f), v3(0.995f, 0.84f, 1.05f), white, white);
        mesh_box(m, v3(-0.66f, 1.34f, 0.06f), v3(0.66f, 1.38f, 0.44f), dark, dark);
        mesh_box(m, v3(-0.60f, 1.38f, 0.12f), v3(-0.04f, 1.53f, 0.38f), col(150, 30, 30), col(150, 30, 30));
        mesh_box(m, v3(0.04f, 1.38f, 0.12f), v3(0.60f, 1.53f, 0.38f), col(30, 60, 160), col(30, 60, 160));
        mesh_box(m, v3(-0.6f, 0.30f, -2.30f), v3(0.6f, 0.58f, -2.20f), dark, dark);   /* defensa de empuje */
        return;
    }
    mesh_init(m);
    g_wheel_n = 0;
    switch (shape) {
    case 0: { /* cuna superdeportiva */
        Sec s[] = {{-2.25f, 0.86f, 0.22f, 0.42f, 0.50f}, {-1.40f, 0.98f, 0.20f, 0.52f, 0.74f},
                   {0.20f, 1.00f, 0.20f, 0.62f, 0.90f}, {1.60f, 1.00f, 0.22f, 0.66f, 0.98f},
                   {2.22f, 0.94f, 0.30f, 0.66f, 0.94f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.16f, accent);
        cabin(m, -0.75f, 1.05f, 0.88f, 1.24f, 0.80f, 0.62f, 0.75f, 0.35f, paint, glass);
        /* aleron bajo */
        mesh_box(m, v3(-0.9f, 0.98f, 1.95f), v3(0.9f, 1.06f, 2.25f), dark, accent);
        wheel(m, -0.86f, -1.35f, 0.34f, 0.28f); wheel(m, 0.86f, -1.35f, 0.34f, 0.28f);
        wheel(m, -0.88f, 1.30f, 0.36f, 0.32f); wheel(m, 0.88f, 1.30f, 0.36f, 0.32f);
        lights(m, -2.25f, 2.22f, 0.36f, 0.72f, 0.62f);
        break;
    }
    case 1: { /* coupe GT */
        Sec s[] = {{-2.20f, 0.84f, 0.24f, 0.48f, 0.62f}, {-1.20f, 0.95f, 0.22f, 0.60f, 0.86f},
                   {0.40f, 0.96f, 0.22f, 0.66f, 0.92f}, {1.70f, 0.95f, 0.24f, 0.70f, 0.98f},
                   {2.18f, 0.90f, 0.32f, 0.70f, 0.96f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.12f, accent);
        cabin(m, -0.35f, 1.35f, 0.90f, 1.36f, 0.78f, 0.62f, 0.62f, 0.55f, paint, glass);
        mesh_box(m, v3(-0.86f, 1.16f, 1.86f), v3(0.86f, 1.22f, 2.18f), accent, accent);
        mesh_box(m, v3(-0.6f, 0.96f, 1.98f), v3(-0.5f, 1.16f, 2.08f), dark, dark);
        mesh_box(m, v3(0.5f, 0.96f, 1.98f), v3(0.6f, 1.16f, 2.08f), dark, dark);
        wheel(m, -0.84f, -1.30f, 0.34f, 0.28f); wheel(m, 0.84f, -1.30f, 0.34f, 0.28f);
        wheel(m, -0.84f, 1.30f, 0.34f, 0.30f); wheel(m, 0.84f, 1.30f, 0.34f, 0.30f);
        lights(m, -2.20f, 2.18f, 0.42f, 0.70f, 0.66f);
        break;
    }
    case 2: { /* compacto de rally */
        Sec s[] = {{-2.00f, 0.86f, 0.30f, 0.56f, 0.74f}, {-1.20f, 0.92f, 0.28f, 0.68f, 0.92f},
                   {0.20f, 0.92f, 0.28f, 0.72f, 0.96f}, {1.55f, 0.92f, 0.30f, 0.74f, 1.00f},
                   {1.98f, 0.88f, 0.36f, 0.74f, 0.98f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.22f, accent);
        cabin(m, -0.70f, 1.55f, 0.94f, 1.52f, 0.80f, 0.68f, 0.55f, 0.20f, paint, glass);
        /* toma de aire en el techo y gran aleron */
        mesh_box(m, v3(-0.22f, 1.52f, -0.05f), v3(0.22f, 1.64f, 0.45f), accent, accent);
        mesh_box(m, v3(-0.95f, 1.38f, 1.62f), v3(0.95f, 1.46f, 2.02f), accent, accent);
        mesh_box(m, v3(-0.75f, 1.00f, 1.75f), v3(-0.65f, 1.38f, 1.88f), dark, dark);
        mesh_box(m, v3(0.65f, 1.00f, 1.75f), v3(0.75f, 1.38f, 1.88f), dark, dark);
        /* faros auxiliares */
        mesh_box(m, v3(-0.55f, 0.60f, -2.08f), v3(-0.25f, 0.80f, -2.00f), col(255, 250, 210), dark);
        mesh_box(m, v3(0.25f, 0.60f, -2.08f), v3(0.55f, 0.80f, -2.00f), col(255, 250, 210), dark);
        wheel(m, -0.86f, -1.25f, 0.36f, 0.28f); wheel(m, 0.86f, -1.25f, 0.36f, 0.28f);
        wheel(m, -0.86f, 1.22f, 0.36f, 0.28f); wheel(m, 0.86f, 1.22f, 0.36f, 0.28f);
        lights(m, -2.00f, 1.98f, 0.44f, 0.72f, 0.70f);
        break;
    }
    case 9: { /* TAURO V10: cuna italiana muy baja (silueta del hermano + dibujo del jugador) */
        Sec s[] = {{-2.35f, 0.90f, 0.16f, 0.34f, 0.40f}, {-1.70f, 1.00f, 0.15f, 0.44f, 0.58f}, {-0.60f, 1.02f, 0.15f, 0.56f, 0.74f},
                   {0.80f, 1.04f, 0.16f, 0.62f, 0.86f}, {1.90f, 1.02f, 0.18f, 0.64f, 0.90f}, {2.32f, 0.96f, 0.24f, 0.62f, 0.86f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 6, paint, dark);
        cabin(m, -0.95f, 0.75f, 0.74f, 1.10f, 0.82f, 0.58f, 0.95f, 0.55f, paint, glass);
        /* tapa del motor con rejillas y aleta central */
        for (k = 0; k < 5; k++) {
            float z = 0.95f + k * 0.2f;
            mesh_quad_flat(m, v3(-0.55f, 0.885f, z), v3(0.55f, 0.885f, z), v3(0.55f, 0.885f, z + 0.08f), v3(-0.55f, 0.885f, z + 0.08f), black);
        }
        mesh_box(m, v3(-0.03f, 0.86f, 0.80f), v3(0.03f, 1.08f, 2.00f), accent, accent);
        /* aleron trasero sobre dos soportes */
        mesh_box(m, v3(-0.50f, 0.88f, 2.02f), v3(-0.42f, 1.18f, 2.12f), black, black);
        mesh_box(m, v3(0.42f, 0.88f, 2.02f), v3(0.50f, 1.18f, 2.12f), black, black);
        mesh_box(m, v3(-0.98f, 1.18f, 1.92f), v3(0.98f, 1.25f, 2.30f), accent, accent);
        /* labio delantero y tomas en V */
        mesh_box(m, v3(-0.95f, 0.10f, -2.48f), v3(0.95f, 0.16f, -2.20f), black, black);
        for (side = -1; side <= 1; side += 2) {
            mesh_tri_flat(m, v3(side * 0.40f, 0.18f, -2.36f), v3(side * 0.85f, 0.18f, -2.36f), v3(side * 0.82f, 0.36f, -2.36f), black);
            /* toma lateral grande delante de la rueda trasera */
            mesh_quad_flat(m, v3(side * 1.035f, 0.30f, 0.25f), v3(side * 1.035f, 0.30f, 1.00f), v3(side * 1.035f, 0.60f, 1.00f), v3(side * 1.035f, 0.56f, 0.40f), black);
        }
        wheel(m, -0.92f, -1.50f, 0.35f, 0.30f); wheel(m, 0.92f, -1.50f, 0.35f, 0.30f);
        wheel(m, -0.94f, 1.45f, 0.37f, 0.36f); wheel(m, 0.94f, 1.45f, 0.37f, 0.36f);
        lights(m, -2.35f, 2.32f, 0.26f, 0.78f, 0.52f);
        break;
    }
    case 10: { /* CICLON R: roadster con cola de pato y rejillas laterales */
        Sec s[] = {{-2.05f, 0.84f, 0.24f, 0.50f, 0.60f}, {-1.30f, 0.90f, 0.22f, 0.60f, 0.74f}, {-0.20f, 0.90f, 0.22f, 0.64f, 0.78f},
                   {1.10f, 0.92f, 0.22f, 0.66f, 0.82f}, {1.90f, 0.88f, 0.26f, 0.68f, 0.86f}, {2.10f, 0.84f, 0.34f, 0.70f, 0.94f}};
        Col black = col(22, 22, 26), seat = col(120, 70, 40);
        int side, k;
        body(m, s, 6, paint, dark);
        stripe(m, s, 6, 0.10f, accent);
        /* habitaculo abierto: piso oscuro, asientos y parabrisas */
        mesh_quad_flat(m, v3(-0.70f, 0.79f, -0.50f), v3(0.70f, 0.79f, -0.50f), v3(0.70f, 0.80f, 0.65f), v3(-0.70f, 0.80f, 0.65f), black);
        for (side = -1; side <= 1; side += 2)
            mesh_box(m, v3(side > 0 ? 0.12f : -0.58f, 0.78f, 0.25f), v3(side > 0 ? 0.58f : -0.12f, 1.12f, 0.58f), seat, seat);
        mesh_quad(m, v3(-0.76f, 0.78f, -0.58f), v3(0.76f, 0.78f, -0.58f), v3(0.68f, 1.10f, -0.30f), v3(-0.68f, 1.10f, -0.30f), glass);
        mesh_box(m, v3(-0.70f, 1.08f, -0.33f), v3(0.70f, 1.12f, -0.27f), black, black);
        /* tres rejillas verticales detras de la puerta */
        for (side = -1; side <= 1; side += 2)
            for (k = 0; k < 3; k++) {
                float z = 0.75f + k * 0.16f;
                mesh_quad_flat(m, v3(side * 0.925f, 0.38f, z), v3(side * 0.925f, 0.38f, z + 0.07f), v3(side * 0.925f, 0.62f, z + 0.07f), v3(side * 0.925f, 0.62f, z), black);
            }
        wheel(m, -0.84f, -1.25f, 0.33f, 0.26f); wheel(m, 0.84f, -1.25f, 0.33f, 0.26f);
        wheel(m, -0.86f, 1.30f, 0.34f, 0.30f); wheel(m, 0.86f, 1.30f, 0.34f, 0.30f);
        lights(m, -2.05f, 2.10f, 0.40f, 0.66f, 0.66f);
        break;
    }
    case 11: { /* RAYO 4P: sedan rapido de 4 puertas con tomas grandes */
        Sec s[] = {{-2.30f, 0.88f, 0.24f, 0.50f, 0.66f}, {-1.60f, 0.97f, 0.22f, 0.64f, 0.84f}, {0.10f, 0.97f, 0.22f, 0.70f, 0.90f},
                   {1.70f, 0.97f, 0.24f, 0.74f, 0.96f}, {2.25f, 0.90f, 0.30f, 0.74f, 0.96f}};
        Col black = col(22, 22, 26);
        int side;
        body(m, s, 5, paint, dark);
        cabin(m, -0.75f, 1.75f, 0.92f, 1.42f, 0.82f, 0.70f, 0.70f, 0.35f, paint, glass);
        /* pilar central: cuatro puertas */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.835f, 0.92f, 0.50f), v3(side * 0.835f, 0.92f, 0.62f), v3(side * 0.715f, 1.42f, 0.62f), v3(side * 0.715f, 1.42f, 0.50f), paint);
        /* aleron de techo, tomas delanteras en triangulo y rejilla central */
        mesh_box(m, v3(-0.68f, 1.40f, 1.55f), v3(0.68f, 1.46f, 1.80f), paint, paint);
        for (side = -1; side <= 1; side += 2)
            mesh_tri_flat(m, v3(side * 0.42f, 0.26f, -2.31f), v3(side * 0.86f, 0.26f, -2.31f), v3(side * 0.86f, 0.48f, -2.31f), black);
        mesh_quad_flat(m, v3(-0.30f, 0.30f, -2.31f), v3(0.30f, 0.30f, -2.31f), v3(0.30f, 0.44f, -2.31f), v3(-0.30f, 0.44f, -2.31f), black);
        /* faldones y pasos de rueda ensanchados */
        for (side = -1; side <= 1; side += 2) {
            mesh_box(m, v3(side > 0 ? 0.93f : -1.00f, 0.22f, -1.00f), v3(side > 0 ? 1.00f : -0.93f, 0.30f, 0.90f), accent, accent);
        }
        wheel(m, -0.90f, -1.45f, 0.35f, 0.30f); wheel(m, 0.90f, -1.45f, 0.35f, 0.30f);
        wheel(m, -0.90f, 1.40f, 0.35f, 0.32f); wheel(m, 0.90f, 1.40f, 0.35f, 0.32f);
        lights(m, -2.30f, 2.25f, 0.52f, 0.74f, 0.72f);
        break;
    }
    case 12: { /* LINCE GT: fastback 2+2 */
        Sec s[] = {{-2.15f, 0.86f, 0.24f, 0.48f, 0.60f}, {-1.30f, 0.94f, 0.22f, 0.60f, 0.80f}, {0.20f, 0.96f, 0.22f, 0.66f, 0.88f},
                   {1.55f, 0.95f, 0.24f, 0.70f, 0.92f}, {2.12f, 0.88f, 0.30f, 0.70f, 0.88f}};
        Col black = col(22, 22, 26);
        int side;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.18f, accent);
        cabin(m, -0.55f, 0.55f, 0.88f, 1.30f, 0.80f, 0.62f, 0.65f, 0.05f, paint, glass);
        /* techo que cae hasta la cola */
        mesh_quad(m, v3(-0.62f, 1.30f, 0.50f), v3(0.62f, 1.30f, 0.50f), v3(0.80f, 0.93f, 2.00f), v3(-0.80f, 0.93f, 2.00f), glass);
        for (side = -1; side <= 1; side += 2)
            mesh_tri(m, v3(side * 0.80f, 0.88f, 0.55f), v3(side * 0.80f, 0.92f, 2.00f), v3(side * 0.62f, 1.30f, 0.50f), paint);
        mesh_box(m, v3(-0.78f, 0.92f, 1.98f), v3(0.78f, 1.00f, 2.12f), accent, accent);
        /* paragolpes con ductos */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.20f, 0.28f, -2.16f), v3(side * 0.70f, 0.28f, -2.16f), v3(side * 0.70f, 0.38f, -2.16f), v3(side * 0.20f, 0.38f, -2.16f), black);
        wheel(m, -0.86f, -1.32f, 0.34f, 0.28f); wheel(m, 0.86f, -1.32f, 0.34f, 0.28f);
        wheel(m, -0.87f, 1.30f, 0.35f, 0.30f); wheel(m, 0.87f, 1.30f, 0.35f, 0.30f);
        lights(m, -2.15f, 2.12f, 0.44f, 0.70f, 0.66f);
        break;
    }
    case 13: { /* VIENTO SP: speedster sin techo con dos arcos detras de los asientos */
        Sec s[] = {{-2.15f, 0.86f, 0.22f, 0.46f, 0.56f}, {-1.30f, 0.94f, 0.20f, 0.58f, 0.72f}, {-0.10f, 0.95f, 0.20f, 0.62f, 0.78f},
                   {1.20f, 0.96f, 0.22f, 0.64f, 0.84f}, {2.15f, 0.90f, 0.28f, 0.64f, 0.80f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.10f, accent);
        mesh_quad_flat(m, v3(-0.72f, 0.79f, -0.50f), v3(0.72f, 0.79f, -0.50f), v3(0.72f, 0.81f, 0.45f), v3(-0.72f, 0.81f, 0.45f), black);
        /* parabrisas bajo envolvente */
        mesh_quad(m, v3(-0.80f, 0.77f, -0.62f), v3(0.80f, 0.77f, -0.62f), v3(0.74f, 0.98f, -0.45f), v3(-0.74f, 0.98f, -0.45f), glass);
        for (side = -1; side <= 1; side += 2) {
            float x0 = side > 0 ? 0.12f : -0.62f, x1 = side > 0 ? 0.62f : -0.12f;
            /* asiento y joroba (arco) que se alarga hasta la cola */
            mesh_box(m, v3(x0, 0.78f, 0.05f), v3(x1, 1.02f, 0.40f), black, black);
            mesh_box(m, v3(x0 + 0.06f, 0.80f, 0.42f), v3(x1 - 0.06f, 1.12f, 0.70f), paint, paint);
            mesh_quad(m, v3(x0 + 0.06f, 1.12f, 0.70f), v3(x1 - 0.06f, 1.12f, 0.70f), v3(x1 - 0.10f, 0.86f, 1.80f), v3(x0 + 0.10f, 0.86f, 1.80f), paint);
            /* rejillas en diagonal en el costado */
            for (k = 0; k < 3; k++) {
                float z = 0.70f + k * 0.18f;
                mesh_quad_flat(m, v3(side * 0.955f, 0.36f, z), v3(side * 0.955f, 0.36f, z + 0.06f), v3(side * 0.955f, 0.58f, z + 0.16f), v3(side * 0.955f, 0.58f, z + 0.10f), black);
            }
        }
        wheel(m, -0.86f, -1.32f, 0.34f, 0.28f); wheel(m, 0.86f, -1.32f, 0.34f, 0.28f);
        wheel(m, -0.88f, 1.32f, 0.35f, 0.32f); wheel(m, 0.88f, 1.32f, 0.35f, 0.32f);
        lights(m, -2.15f, 2.15f, 0.36f, 0.70f, 0.58f);
        break;
    }
    case 14: { /* T155: deportivo moderno con parrilla grande y faros rectangulares */
        Sec s[] = {{-2.40f, 0.88f, 0.26f, 0.56f, 0.72f}, {-1.70f, 0.95f, 0.24f, 0.68f, 0.88f}, {0.20f, 0.96f, 0.24f, 0.72f, 0.92f},
                   {1.80f, 0.95f, 0.26f, 0.74f, 0.96f}, {2.38f, 0.90f, 0.32f, 0.74f, 0.94f}};
        Col black = col(18, 18, 22), white = col(250, 248, 236), red = col(220, 20, 24);
        int side, k;
        body(m, s, 5, paint, dark);
        cabin(m, -0.70f, 1.45f, 0.94f, 1.42f, 0.82f, 0.70f, 0.65f, 0.55f, paint, glass);
        /* parrilla grande con barras cromadas */
        mesh_quad_flat(m, v3(-0.55f, 0.30f, -2.415f), v3(0.55f, 0.30f, -2.415f), v3(0.55f, 0.62f, -2.415f), v3(-0.55f, 0.62f, -2.415f), black);
        for (k = 0; k < 3; k++) {
            float y = 0.36f + k * 0.10f;
            mesh_quad_flat(m, v3(-0.53f, y, -2.42f), v3(0.53f, y, -2.42f), v3(0.53f, y + 0.025f, -2.42f), v3(-0.53f, y + 0.025f, -2.42f), accent);
        }
        for (side = -1; side <= 1; side += 2) {
            /* faros rectangulares en las esquinas */
            mesh_quad_flat(m, v3(side * 0.60f, 0.52f, -2.415f), v3(side * 0.86f, 0.52f, -2.415f), v3(side * 0.86f, 0.66f, -2.415f), v3(side * 0.60f, 0.66f, -2.415f), white);
            /* pilotos traseros alargados y dos escapes por lado */
            mesh_quad_flat(m, v3(side * 0.45f, 0.62f, 2.39f), v3(side * 0.86f, 0.62f, 2.39f), v3(side * 0.86f, 0.72f, 2.39f), v3(side * 0.45f, 0.72f, 2.39f), red);
            mesh_box(m, v3(side * 0.50f - 0.07f, 0.28f, 2.36f), v3(side * 0.50f + 0.07f, 0.36f, 2.46f), accent, black);
            mesh_box(m, v3(side * 0.68f - 0.07f, 0.28f, 2.36f), v3(side * 0.68f + 0.07f, 0.36f, 2.46f), accent, black);
        }
        mesh_box(m, v3(-0.86f, 0.94f, 2.22f), v3(0.86f, 1.00f, 2.38f), paint, paint);   /* labio en el maletero */
        wheel(m, -0.88f, -1.50f, 0.35f, 0.28f); wheel(m, 0.88f, -1.50f, 0.35f, 0.28f);
        wheel(m, -0.88f, 1.45f, 0.35f, 0.30f); wheel(m, 0.88f, 1.45f, 0.35f, 0.30f);
        break;
    }
    case 15: { /* SIRIO GT: coupe de resistencia, bajo y redondeado */
        Sec s[] = {{-2.30f, 0.86f, 0.20f, 0.38f, 0.46f}, {-1.50f, 0.98f, 0.18f, 0.50f, 0.66f}, {-0.30f, 0.97f, 0.18f, 0.58f, 0.78f},
                   {1.20f, 0.99f, 0.20f, 0.62f, 0.86f}, {2.20f, 0.94f, 0.26f, 0.62f, 0.86f}};
        Col black = col(22, 22, 26), lamp = col(255, 246, 210);
        int side;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.22f, accent);
        cabin(m, -0.70f, 0.55f, 0.82f, 1.10f, 0.78f, 0.56f, 0.70f, 0.08f, paint, glass);
        mesh_quad(m, v3(-0.56f, 1.10f, 0.47f), v3(0.56f, 1.10f, 0.47f), v3(0.82f, 0.88f, 1.95f), v3(-0.82f, 0.88f, 1.95f), paint);
        for (side = -1; side <= 1; side += 2) {
            mesh_tri(m, v3(side * 0.78f, 0.82f, 0.55f), v3(side * 0.82f, 0.87f, 1.95f), v3(side * 0.56f, 1.10f, 0.47f), paint);
            /* faros redondos bajo cubierta y toma de aire en la puerta */
            mesh_box(m, v3(side > 0 ? 0.38f : -0.72f, 0.40f, -2.05f), v3(side > 0 ? 0.72f : -0.38f, 0.56f, -1.80f), lamp, glass);
            mesh_quad_flat(m, v3(side * 0.985f, 0.36f, 0.20f), v3(side * 0.985f, 0.36f, 0.70f), v3(side * 0.985f, 0.56f, 0.70f), v3(side * 0.985f, 0.56f, 0.30f), black);
        }
        mesh_box(m, v3(-0.86f, 0.86f, 2.02f), v3(0.86f, 0.94f, 2.22f), black, black);   /* labio trasero */
        mesh_quad_flat(m, v3(-0.40f, 0.22f, -2.31f), v3(0.40f, 0.22f, -2.31f), v3(0.40f, 0.34f, -2.31f), v3(-0.40f, 0.34f, -2.31f), black);
        wheel(m, -0.88f, -1.38f, 0.34f, 0.28f); wheel(m, 0.88f, -1.38f, 0.34f, 0.28f);
        wheel(m, -0.90f, 1.36f, 0.35f, 0.32f); wheel(m, 0.90f, 1.36f, 0.35f, 0.32f);
        lights(m, -2.30f, 2.20f, 0.24f, 0.70f, 0.58f);
        break;
    }
    case 16: { /* SVSR5: deportivo de los 50, techo redondo, cromados y dos tonos */
        Sec s[] = {{-2.35f, 0.88f, 0.30f, 0.62f, 0.76f}, {-1.60f, 0.96f, 0.28f, 0.74f, 0.90f}, {0.00f, 0.96f, 0.28f, 0.76f, 0.92f},
                   {1.50f, 0.95f, 0.30f, 0.74f, 0.88f}, {2.25f, 0.86f, 0.36f, 0.66f, 0.74f}};
        Col chrome = col(214, 216, 222), lamp = col(255, 246, 210);
        int side;
        body(m, s, 5, paint, dark);
        cabin(m, -0.45f, 1.25f, 0.92f, 1.48f, 0.84f, 0.66f, 0.45f, 0.55f, accent, glass);
        for (side = -1; side <= 1; side += 2) {
            /* pilar central (cuatro ventanas), faros redondos y moldura cromada */
            mesh_quad_flat(m, v3(side * 0.845f, 0.92f, 0.38f), v3(side * 0.845f, 0.92f, 0.48f), v3(side * 0.665f, 1.48f, 0.42f), v3(side * 0.665f, 1.48f, 0.32f), accent);
            mesh_box(m, v3(side > 0 ? 0.52f : -0.82f, 0.60f, -2.40f), v3(side > 0 ? 0.82f : -0.52f, 0.78f, -2.30f), lamp, chrome);
            mesh_quad_flat(m, v3(side * 0.965f, 0.60f, -1.80f), v3(side * 0.965f, 0.60f, 1.90f), v3(side * 0.965f, 0.64f, 1.90f), v3(side * 0.965f, 0.64f, -1.80f), chrome);
        }
        /* parachoques cromados y parrilla */
        mesh_box(m, v3(-0.92f, 0.28f, -2.46f), v3(0.92f, 0.42f, -2.30f), chrome, chrome);
        mesh_box(m, v3(-0.88f, 0.32f, 2.22f), v3(0.88f, 0.44f, 2.36f), chrome, chrome);
        mesh_quad_flat(m, v3(-0.42f, 0.46f, -2.355f), v3(0.42f, 0.46f, -2.355f), v3(0.42f, 0.66f, -2.355f), v3(-0.42f, 0.66f, -2.355f), chrome);
        mesh_quad_flat(m, v3(-0.60f, 0.56f, 2.26f), v3(-0.30f, 0.56f, 2.26f), v3(-0.30f, 0.68f, 2.26f), v3(-0.60f, 0.68f, 2.26f), col(220, 20, 24));
        mesh_quad_flat(m, v3(0.30f, 0.56f, 2.26f), v3(0.60f, 0.56f, 2.26f), v3(0.60f, 0.68f, 2.26f), v3(0.30f, 0.68f, 2.26f), col(220, 20, 24));
        wheel(m, -0.86f, -1.40f, 0.36f, 0.26f); wheel(m, 0.86f, -1.40f, 0.36f, 0.26f);
        wheel(m, -0.86f, 1.35f, 0.36f, 0.26f); wheel(m, 0.86f, 1.35f, 0.36f, 0.26f);
        break;
    }
    case 17: { /* BELFI V8 69: muscle car fastback con cola levantada */
        Sec s[] = {{-2.45f, 0.92f, 0.28f, 0.56f, 0.70f}, {-1.70f, 0.98f, 0.26f, 0.66f, 0.84f}, {0.10f, 0.98f, 0.26f, 0.70f, 0.88f},
                   {1.80f, 0.98f, 0.28f, 0.72f, 0.92f}, {2.40f, 0.94f, 0.34f, 0.74f, 0.98f}};
        Col black = col(18, 18, 22), lamp = col(255, 246, 210);
        int side;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.14f, accent);
        cabin(m, -0.40f, 0.50f, 0.88f, 1.30f, 0.82f, 0.64f, 0.55f, 0.05f, paint, glass);
        /* techo fastback hasta la cola, con luneta pequena */
        mesh_quad(m, v3(-0.64f, 1.30f, 0.45f), v3(0.64f, 1.30f, 0.45f), v3(0.84f, 0.94f, 2.05f), v3(-0.84f, 0.94f, 2.05f), paint);
        mesh_quad_flat(m, v3(-0.55f, 1.27f, 0.62f), v3(0.55f, 1.27f, 0.62f), v3(0.62f, 1.09f, 1.30f), v3(-0.62f, 1.09f, 1.30f), glass);
        for (side = -1; side <= 1; side += 2) {
            mesh_tri(m, v3(side * 0.82f, 0.88f, 0.50f), v3(side * 0.84f, 0.92f, 2.05f), v3(side * 0.64f, 1.30f, 0.45f), paint);
            /* toma lateral delante de la rueda trasera y faro redondo en la parrilla */
            mesh_quad_flat(m, v3(side * 0.985f, 0.40f, 0.35f), v3(side * 0.985f, 0.40f, 0.85f), v3(side * 0.985f, 0.62f, 0.85f), v3(side * 0.985f, 0.50f, 0.35f), black);
            mesh_box(m, v3(side > 0 ? 0.50f : -0.80f, 0.44f, -2.48f), v3(side > 0 ? 0.80f : -0.50f, 0.62f, -2.44f), lamp, lamp);
        }
        /* parrilla negra a lo ancho, joroba en el cofre y cola levantada */
        mesh_quad_flat(m, v3(-0.88f, 0.34f, -2.455f), v3(0.88f, 0.34f, -2.455f), v3(0.88f, 0.68f, -2.455f), v3(-0.88f, 0.68f, -2.455f), black);
        mesh_box(m, v3(-0.32f, 0.70f, -1.90f), v3(0.32f, 0.86f, -0.60f), paint, paint);
        mesh_box(m, v3(-0.92f, 0.96f, 2.18f), v3(0.92f, 1.08f, 2.42f), paint, paint);
        mesh_quad_flat(m, v3(-0.88f, 0.60f, 2.405f), v3(0.88f, 0.60f, 2.405f), v3(0.88f, 0.76f, 2.405f), v3(-0.88f, 0.76f, 2.405f), col(200, 20, 22));
        mesh_box(m, v3(-0.94f, 0.26f, -2.52f), v3(0.94f, 0.34f, -2.40f), col(210, 212, 218), col(210, 212, 218));
        wheel(m, -0.90f, -1.55f, 0.36f, 0.30f); wheel(m, 0.90f, -1.55f, 0.36f, 0.30f);
        wheel(m, -0.92f, 1.45f, 0.37f, 0.36f); wheel(m, 0.92f, 1.45f, 0.37f, 0.36f);
        break;
    }
    case 18: { /* CARRITO: el coche de juguete del jugador, sedan redondeado de los 90 en dos tonos */
        Sec s[] = {{-2.25f, 0.86f, 0.22f, 0.46f, 0.58f}, {-1.55f, 0.95f, 0.20f, 0.60f, 0.78f}, {0.00f, 0.96f, 0.20f, 0.68f, 0.88f},
                   {1.55f, 0.95f, 0.22f, 0.70f, 0.90f}, {2.20f, 0.88f, 0.28f, 0.66f, 0.82f}};
        Col white = col(245, 245, 240), lamp = col(40, 40, 46);
        int side;
        body(m, s, 5, paint, dark);
        cabin(m, -0.75f, 1.30f, 0.86f, 1.32f, 0.80f, 0.62f, 0.80f, 0.85f, paint, glass);
        for (side = -1; side <= 1; side += 2) {
            /* pilar central de las 4 puertas */
            mesh_quad_flat(m, v3(side * 0.825f, 0.88f, 0.15f), v3(side * 0.825f, 0.88f, 0.25f), v3(side * 0.625f, 1.32f, 0.25f), v3(side * 0.625f, 1.32f, 0.15f), paint);
            /* faldon negro de punta a punta y filete blanco encima */
            mesh_box(m, v3(side > 0 ? 0.93f : -0.985f, 0.20f, -2.10f), v3(side > 0 ? 0.985f : -0.93f, 0.40f, 2.05f), accent, accent);
            mesh_quad_flat(m, v3(side * 0.99f, 0.41f, -2.05f), v3(side * 0.99f, 0.41f, 2.00f), v3(side * 0.99f, 0.44f, 2.00f), v3(side * 0.99f, 0.44f, -2.05f), white);
            /* faros rectangulares hundidos en el cofre, con borde blanco */
            mesh_quad_flat(m, v3(side * 0.30f, 0.715f, -1.88f), v3(side * 0.68f, 0.715f, -1.88f), v3(side * 0.68f, 0.765f, -1.48f), v3(side * 0.30f, 0.765f, -1.48f), white);
            mesh_quad_flat(m, v3(side * 0.34f, 0.72f, -1.84f), v3(side * 0.64f, 0.72f, -1.84f), v3(side * 0.64f, 0.77f, -1.52f), v3(side * 0.34f, 0.77f, -1.52f), lamp);
        }
        /* defensas negras y filete blanco al frente */
        mesh_box(m, v3(-0.90f, 0.20f, -2.34f), v3(0.90f, 0.38f, -2.10f), accent, accent);
        mesh_box(m, v3(-0.88f, 0.24f, 2.05f), v3(0.88f, 0.44f, 2.28f), accent, accent);
        mesh_quad_flat(m, v3(-0.80f, 0.40f, -2.27f), v3(0.80f, 0.40f, -2.27f), v3(0.80f, 0.43f, -2.27f), v3(-0.80f, 0.43f, -2.27f), white);
        mesh_quad_flat(m, v3(-0.80f, 0.56f, 2.205f), v3(-0.40f, 0.56f, 2.205f), v3(-0.40f, 0.66f, 2.205f), v3(-0.80f, 0.66f, 2.205f), col(220, 30, 30));
        mesh_quad_flat(m, v3(0.40f, 0.56f, 2.205f), v3(0.80f, 0.56f, 2.205f), v3(0.80f, 0.66f, 2.205f), v3(0.40f, 0.66f, 2.205f), col(220, 30, 30));
        wheel(m, -0.86f, -1.42f, 0.34f, 0.26f); wheel(m, 0.86f, -1.42f, 0.34f, 0.26f);
        wheel(m, -0.86f, 1.38f, 0.34f, 0.26f); wheel(m, 0.86f, 1.38f, 0.34f, 0.26f);
        break;
    }
    case 19: { /* HALCON R: fastback de los 90, cola levantada, faros delgados */
        Sec s[] = {{-2.35f, 0.90f, 0.24f, 0.42f, 0.56f}, {-1.50f, 0.98f, 0.22f, 0.56f, 0.74f}, {0.00f, 0.98f, 0.22f, 0.64f, 0.84f},
                   {1.50f, 0.98f, 0.24f, 0.68f, 0.88f}, {2.25f, 0.92f, 0.30f, 0.70f, 0.90f}};
        Col black = col(18, 18, 22), white = col(250, 248, 236), red = col(210, 20, 24);
        int side;
        body(m, s, 5, paint, dark);
        cabin(m, -0.70f, 0.55f, 0.84f, 1.24f, 0.80f, 0.60f, 0.75f, 0.08f, paint, glass);
        mesh_quad(m, v3(-0.60f, 1.24f, 0.50f), v3(0.60f, 1.24f, 0.50f), v3(0.82f, 0.90f, 2.05f), v3(-0.82f, 0.90f, 2.05f), glass);
        for (side = -1; side <= 1; side += 2) {
            mesh_tri(m, v3(side * 0.80f, 0.84f, 0.55f), v3(side * 0.82f, 0.89f, 2.05f), v3(side * 0.60f, 1.24f, 0.50f), paint);
            /* faros delgados y rectangulares en las esquinas */
            mesh_quad_flat(m, v3(side * 0.42f, 0.46f, -2.355f), v3(side * 0.86f, 0.46f, -2.355f), v3(side * 0.86f, 0.54f, -2.355f), v3(side * 0.42f, 0.54f, -2.355f), white);
            /* faros de niebla cuadrados en la defensa */
            mesh_quad_flat(m, v3(side * 0.45f, 0.28f, -2.365f), v3(side * 0.65f, 0.28f, -2.365f), v3(side * 0.65f, 0.38f, -2.365f), v3(side * 0.45f, 0.38f, -2.365f), white);
        }
        mesh_quad_flat(m, v3(-0.88f, 0.26f, -2.36f), v3(0.88f, 0.26f, -2.36f), v3(0.88f, 0.40f, -2.36f), v3(-0.88f, 0.40f, -2.36f), black);
        /* cola levantada (patito) */
        mesh_quad(m, v3(-0.84f, 0.90f, 2.02f), v3(0.84f, 0.90f, 2.02f), v3(0.86f, 1.04f, 2.32f), v3(-0.86f, 1.04f, 2.32f), paint);
        /* barra de pilotos de lado a lado con panel al centro */
        mesh_quad_flat(m, v3(-0.88f, 0.62f, 2.255f), v3(0.88f, 0.62f, 2.255f), v3(0.88f, 0.76f, 2.255f), v3(-0.88f, 0.76f, 2.255f), red);
        mesh_quad_flat(m, v3(-0.30f, 0.63f, 2.26f), v3(0.30f, 0.63f, 2.26f), v3(0.30f, 0.75f, 2.26f), v3(-0.30f, 0.75f, 2.26f), black);
        wheel(m, -0.86f, -1.42f, 0.34f, 0.28f); wheel(m, 0.86f, -1.42f, 0.34f, 0.28f);
        wheel(m, -0.88f, 1.38f, 0.35f, 0.32f); wheel(m, 0.88f, 1.38f, 0.35f, 0.32f);
        break;
    }
    case 20: { /* TORNADO V8: coupe cuadrado de dos puertas, musculoso, con franjas */
        Sec s[] = {{-2.45f, 0.94f, 0.28f, 0.60f, 0.80f}, {-1.70f, 1.00f, 0.26f, 0.72f, 0.92f}, {0.20f, 1.00f, 0.26f, 0.76f, 0.98f},
                   {1.90f, 1.00f, 0.28f, 0.78f, 1.00f}, {2.42f, 0.96f, 0.34f, 0.76f, 0.98f}};
        Col black = col(16, 16, 20), white = col(250, 248, 236), red = col(210, 20, 24), plata = col(200, 202, 208);
        int side;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.14f, accent);
        cabin(m, -0.55f, 1.20f, 0.96f, 1.36f, 0.84f, 0.70f, 0.70f, 0.45f, paint, glass);
        mesh_box(m, v3(-0.36f, 0.80f, -2.00f), v3(0.36f, 0.94f, -0.70f), paint, paint);   /* joroba del cofre */
        for (side = -1; side <= 1; side += 2) {
            /* faros delgados y alargados */
            mesh_quad_flat(m, v3(side * 0.30f, 0.66f, -2.455f), v3(side * 0.90f, 0.66f, -2.455f), v3(side * 0.90f, 0.74f, -2.455f), v3(side * 0.30f, 0.74f, -2.455f), white);
            /* pilotos anchos y escapes en las esquinas */
            mesh_quad_flat(m, v3(side * 0.20f, 0.70f, 2.425f), v3(side * 0.90f, 0.70f, 2.425f), v3(side * 0.90f, 0.82f, 2.425f), v3(side * 0.20f, 0.82f, 2.425f), red);
            mesh_box(m, v3(side * 0.70f - 0.09f, 0.28f, 2.38f), v3(side * 0.70f + 0.09f, 0.38f, 2.50f), plata, black);
        }
        /* parrilla negra ancha con barra plateada al centro */
        mesh_quad_flat(m, v3(-0.86f, 0.36f, -2.455f), v3(0.86f, 0.36f, -2.455f), v3(0.86f, 0.62f, -2.455f), v3(-0.86f, 0.62f, -2.455f), black);
        mesh_quad_flat(m, v3(-0.05f, 0.38f, -2.46f), v3(0.05f, 0.38f, -2.46f), v3(0.05f, 0.60f, -2.46f), v3(-0.05f, 0.60f, -2.46f), plata);
        mesh_box(m, v3(-0.88f, 0.98f, 2.25f), v3(0.88f, 1.04f, 2.42f), paint, paint);   /* labio del maletero */
        wheel(m, -0.90f, -1.55f, 0.36f, 0.30f); wheel(m, 0.90f, -1.55f, 0.36f, 0.30f);
        wheel(m, -0.92f, 1.45f, 0.37f, 0.36f); wheel(m, 0.92f, 1.45f, 0.37f, 0.36f);
        break;
    }
    case 21: { /* QUASAR: superdeportivo curvo de motor central, tomas laterales y escapes redondos */
        Sec s[] = {{-2.30f, 0.88f, 0.18f, 0.36f, 0.44f}, {-1.50f, 0.98f, 0.17f, 0.48f, 0.62f}, {-0.30f, 1.00f, 0.17f, 0.58f, 0.76f},
                   {1.10f, 1.02f, 0.18f, 0.64f, 0.84f}, {2.25f, 0.96f, 0.24f, 0.62f, 0.82f}};
        Col black = col(14, 14, 18), white = col(250, 248, 236), red = col(220, 20, 24);
        int side, k;
        body(m, s, 5, paint, dark);
        cabin(m, -0.85f, 0.85f, 0.74f, 1.12f, 0.80f, 0.56f, 0.90f, 0.50f, paint, glass);
        for (k = 0; k < 4; k++) {   /* rejillas sobre el motor */
            float z = 1.05f + k * 0.22f;
            mesh_quad_flat(m, v3(-0.50f, 0.835f, z), v3(0.50f, 0.835f, z), v3(0.50f, 0.835f, z + 0.10f), v3(-0.50f, 0.835f, z + 0.10f), black);
        }
        for (side = -1; side <= 1; side += 2) {
            /* toma lateral larga en diagonal, en el color de acento */
            mesh_quad_flat(m, v3(side * 1.005f, 0.30f, -0.30f), v3(side * 1.025f, 0.30f, 1.25f), v3(side * 1.025f, 0.56f, 1.05f), v3(side * 1.005f, 0.40f, -0.30f), accent);
            mesh_quad_flat(m, v3(side * 1.03f, 0.34f, 0.40f), v3(side * 1.03f, 0.34f, 1.15f), v3(side * 1.03f, 0.52f, 1.00f), v3(side * 1.03f, 0.42f, 0.40f), black);
            /* faros rasgados y faros de niebla redondos abajo */
            mesh_quad_flat(m, v3(side * 0.40f, 0.36f, -2.305f), v3(side * 0.84f, 0.32f, -2.305f), v3(side * 0.84f, 0.40f, -2.305f), v3(side * 0.40f, 0.44f, -2.305f), white);
            mesh_box(m, v3(side * 0.62f - 0.07f, 0.21f, -2.36f), v3(side * 0.62f + 0.07f, 0.29f, -2.29f), white, white);
            /* pilotos escalonados */
            mesh_quad_flat(m, v3(side * 0.30f, 0.62f, 2.255f), v3(side * 0.62f, 0.62f, 2.255f), v3(side * 0.62f, 0.70f, 2.255f), v3(side * 0.30f, 0.70f, 2.255f), red);
            mesh_quad_flat(m, v3(side * 0.62f, 0.56f, 2.255f), v3(side * 0.90f, 0.56f, 2.255f), v3(side * 0.90f, 0.66f, 2.255f), v3(side * 0.62f, 0.66f, 2.255f), red);
            /* dos escapes redondos al centro */
            mesh_box(m, v3(side * 0.16f - 0.09f, 0.22f, 2.22f), v3(side * 0.16f + 0.09f, 0.38f, 2.32f), col(180, 182, 188), black);
        }
        /* toma en V al centro del frente */
        mesh_tri_flat(m, v3(-0.30f, 0.43f, -2.31f), v3(0.30f, 0.43f, -2.31f), v3(0.0f, 0.26f, -2.31f), black);
        wheel(m, -0.88f, -1.40f, 0.34f, 0.28f); wheel(m, 0.88f, -1.40f, 0.34f, 0.28f);
        wheel(m, -0.92f, 1.40f, 0.36f, 0.36f); wheel(m, 0.92f, 1.40f, 0.36f, 0.36f);
        break;
    }
    case 23: { /* FORMULA 90: monoplaza de los 90, ruedas descubiertas, alerones, toma de aire y piloto */
        Col negro = col(20, 20, 24), casco = col(250, 220, 40), visor = col(30, 40, 60);
        int s;
        /* nariz elevada que se angosta hacia el frente */
        mesh_box(m, v3(-0.10f, 0.30f, -2.45f), v3(0.10f, 0.44f, -1.90f), paint, paint);
        mesh_box(m, v3(-0.15f, 0.30f, -1.90f), v3(0.15f, 0.52f, -1.30f), paint, accent);
        mesh_box(m, v3(-0.22f, 0.24f, -1.30f), v3(0.22f, 0.60f, -0.70f), paint, paint);
        /* aleron delantero con placas laterales y pilares */
        mesh_box(m, v3(-0.98f, 0.10f, -2.62f), v3(0.98f, 0.15f, -2.28f), accent, accent);
        mesh_box(m, v3(-0.98f, 0.18f, -2.50f), v3(0.98f, 0.21f, -2.34f), paint, paint);
        for (s = -1; s <= 1; s += 2) {
            mesh_box(m, v3(s * 0.98f - 0.02f, 0.08f, -2.64f), v3(s * 0.98f + 0.02f, 0.32f, -2.26f), paint, paint);
            mesh_box(m, v3(s * 0.08f - 0.02f, 0.14f, -2.45f), v3(s * 0.08f + 0.02f, 0.32f, -2.35f), negro, negro);
        }
        /* monocasco, cabina abierta y el piloto con casco */
        mesh_box(m, v3(-0.36f, 0.16f, -0.70f), v3(0.36f, 0.62f, 0.70f), paint, paint);
        mesh_box(m, v3(-0.24f, 0.62f, -0.55f), v3(0.24f, 0.63f, 0.05f), negro, negro);
        mesh_box(m, v3(-0.15f, 0.60f, -0.28f), v3(0.15f, 0.86f, 0.02f), casco, casco);
        mesh_box(m, v3(-0.13f, 0.70f, -0.29f), v3(0.13f, 0.78f, -0.27f), visor, visor);
        /* toma de aire sobre el piloto y cubierta del motor que baja hacia atras */
        mesh_box(m, v3(-0.17f, 0.62f, 0.05f), v3(0.17f, 0.98f, 0.45f), paint, accent);
        mesh_quad_flat(m, v3(-0.12f, 0.72f, 0.049f), v3(0.12f, 0.72f, 0.049f), v3(0.12f, 0.92f, 0.049f), v3(-0.12f, 0.92f, 0.049f), negro);
        mesh_quad(m, v3(-0.17f, 0.98f, 0.45f), v3(0.17f, 0.98f, 0.45f), v3(0.12f, 0.50f, 1.70f), v3(-0.12f, 0.50f, 1.70f), paint);
        for (s = -1; s <= 1; s += 2) {
            mesh_tri(m, v3(s * 0.17f, 0.62f, 0.45f), v3(s * 0.17f, 0.98f, 0.45f), v3(s * 0.12f, 0.50f, 1.70f), paint);
            /* pontones laterales con toma de aire negra */
            mesh_box(m, v3(s > 0 ? 0.36f : -0.74f, 0.14f, -0.30f), v3(s > 0 ? 0.74f : -0.36f, 0.48f, 0.95f), paint, accent);
            mesh_quad_flat(m, v3(s * 0.40f, 0.20f, -0.305f), v3(s * 0.70f, 0.20f, -0.305f), v3(s * 0.70f, 0.44f, -0.305f), v3(s * 0.40f, 0.44f, -0.305f), negro);
            /* brazos de suspension */
            mesh_box(m, v3(s > 0 ? 0.20f : -0.72f, 0.33f, -1.78f), v3(s > 0 ? 0.72f : -0.20f, 0.37f, -1.70f), negro, negro);
            mesh_box(m, v3(s > 0 ? 0.70f : -0.80f, 0.33f, 1.32f), v3(s > 0 ? 0.80f : -0.70f, 0.37f, 1.40f), negro, negro);
            /* placas del aleron trasero */
            mesh_box(m, v3(s * 0.62f - 0.02f, 0.40f, 1.70f), v3(s * 0.62f + 0.02f, 1.08f, 2.12f), paint, paint);
        }
        mesh_box(m, v3(-0.40f, 0.14f, 0.95f), v3(0.40f, 0.42f, 1.85f), paint, paint);   /* caja de cambios */
        /* aleron trasero de dos planos con su soporte */
        mesh_box(m, v3(-0.62f, 0.94f, 1.76f), v3(0.62f, 1.02f, 2.08f), accent, accent);
        mesh_box(m, v3(-0.62f, 0.72f, 1.86f), v3(0.62f, 0.77f, 2.06f), paint, paint);
        mesh_box(m, v3(-0.03f, 0.42f, 1.84f), v3(0.03f, 0.94f, 1.94f), negro, negro);
        mesh_box(m, v3(-0.08f, 0.22f, 1.86f), v3(0.08f, 0.32f, 1.92f), col(255, 40, 40), col(255, 40, 40));   /* luz de lluvia */
        /* ruedas descubiertas: las de atras mas anchas */
        wheel(m, -0.86f, -1.74f, 0.33f, 0.30f); wheel(m, 0.86f, -1.74f, 0.33f, 0.30f);
        wheel(m, -0.88f, 1.36f, 0.36f, 0.42f); wheel(m, 0.88f, 1.36f, 0.36f, 0.42f);
        break;
    }
    case 24: { /* ULTIMATE R: superdeportivo del dibujo del hermano (lateral, frente y trasera):
                  nariz baja, parabrisas largo, cabina atras, rayas que suben por el costado, toma detras de la puerta,
                  frente ancho con dos faros de niebla redondos y trasera con pilotos escalonados y dos escapes redondos */
        Sec s[] = {{-2.40f, 0.86f, 0.22f, 0.40f, 0.50f}, {-1.70f, 0.98f, 0.20f, 0.52f, 0.66f}, {-0.60f, 1.00f, 0.20f, 0.58f, 0.76f},
                   {0.80f, 1.03f, 0.20f, 0.62f, 0.86f}, {1.90f, 1.01f, 0.24f, 0.64f, 0.90f}, {2.36f, 0.99f, 0.28f, 0.64f, 0.88f}};
        Col negro = col(16, 16, 20), faro = col(255, 246, 210), rojo = col(226, 22, 26), cromo = col(200, 204, 212);
        int sd, k;
        body(m, s, 6, paint, dark);
        cabin(m, -0.75f, 1.35f, 0.80f, 1.15f, 0.80f, 0.58f, 1.00f, 0.55f, paint, glass);
        /* cofre con dos lineas de ventilacion */
        for (sd = -1; sd <= 1; sd += 2)
            mesh_quad_flat(m, v3(sd * 0.22f, 0.71f, -1.75f), v3(sd * 0.32f, 0.71f, -1.75f), v3(sd * 0.40f, 0.765f, -0.95f), v3(sd * 0.30f, 0.765f, -0.95f), negro);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.035f;
            /* dos rayas que suben hacia atras por el costado */
            mesh_quad_flat(m, v3(x, 0.44f, -0.90f), v3(x, 0.52f, 1.05f), v3(x, 0.57f, 1.05f), v3(x, 0.49f, -0.90f), accent);
            mesh_quad_flat(m, v3(x, 0.30f, -0.40f), v3(x, 0.30f, 0.80f), v3(x, 0.36f, 0.95f), v3(x, 0.35f, -0.40f), accent);
            /* toma de aire detras de la puerta */
            mesh_quad_flat(m, v3(x * 1.005f, 0.36f, 0.95f), v3(x * 1.005f, 0.42f, 1.40f), v3(x * 1.005f, 0.60f, 1.40f), v3(x * 1.005f, 0.56f, 1.10f), negro);
            /* faros rasgados en las esquinas del frente */
            mesh_quad_flat(m, v3(sd * 0.48f, 0.42f, -2.405f), v3(sd * 0.84f, 0.40f, -2.405f), v3(sd * 0.82f, 0.49f, -2.39f), v3(sd * 0.52f, 0.50f, -2.39f), faro);
            /* faros de niebla redondos en la toma baja */
            mesh_box(m, v3(sd * 0.56f - 0.07f, 0.25f, -2.43f), v3(sd * 0.56f + 0.07f, 0.33f, -2.40f), faro, faro);
            /* pilotos traseros escalonados de lado a lado */
            mesh_quad_flat(m, v3(sd * 0.12f, 0.70f, 2.365f), v3(sd * 0.50f, 0.70f, 2.365f), v3(sd * 0.50f, 0.78f, 2.365f), v3(sd * 0.12f, 0.78f, 2.365f), rojo);
            mesh_quad_flat(m, v3(sd * 0.50f, 0.64f, 2.365f), v3(sd * 0.90f, 0.64f, 2.365f), v3(sd * 0.90f, 0.76f, 2.365f), v3(sd * 0.50f, 0.76f, 2.365f), rojo);
            /* dos escapes redondos en las esquinas del difusor */
            mesh_box(m, v3(sd * 0.62f - 0.09f, 0.26f, 2.32f), v3(sd * 0.62f + 0.09f, 0.40f, 2.44f), cromo, negro);
        }
        /* toma baja ancha al frente y difusor negro atras */
        mesh_quad_flat(m, v3(-0.70f, 0.22f, -2.40f), v3(0.70f, 0.22f, -2.40f), v3(0.70f, 0.37f, -2.40f), v3(-0.70f, 0.37f, -2.40f), negro);
        mesh_quad_flat(m, v3(-0.80f, 0.26f, 2.36f), v3(0.80f, 0.26f, 2.36f), v3(0.80f, 0.46f, 2.36f), v3(-0.80f, 0.46f, 2.36f), negro);
        for (k = -1; k <= 1; k += 2) mesh_box(m, v3(k * 0.20f - 0.01f, 0.22f, 2.30f), v3(k * 0.20f + 0.01f, 0.40f, 2.44f), dark, dark);
        /* ruedas grandes, las de atras mas anchas */
        wheel(m, -0.90f, -1.50f, 0.37f, 0.30f); wheel(m, 0.90f, -1.50f, 0.37f, 0.30f);
        wheel(m, -0.93f, 1.48f, 0.39f, 0.38f); wheel(m, 0.93f, 1.48f, 0.39f, 0.38f);
        break;
    }
    case 25: { /* GRULLA 92: sedan compacto de tres volumenes de los 90, defensas negras y tapones */
        Sec s[] = {{-2.20f, 0.84f, 0.30f, 0.56f, 0.70f}, {-1.30f, 0.86f, 0.28f, 0.60f, 0.79f}, {0.30f, 0.87f, 0.28f, 0.62f, 0.82f},
                   {1.45f, 0.87f, 0.28f, 0.62f, 0.85f}, {2.15f, 0.85f, 0.32f, 0.62f, 0.84f}};
        Col negro = col(30, 30, 34), faro = col(236, 238, 230), rojo = col(206, 28, 30), ambar = col(240, 150, 30), tapon = col(200, 202, 206);
        int sd, k;
        body(m, s, 5, paint, dark);
        cabin(m, -0.95f, 1.20f, 0.82f, 1.38f, 0.82f, 0.68f, 0.58f, 0.42f, paint, glass);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.875f;
            /* pilar entre puertas y molduras negras a lo largo */
            mesh_quad_flat(m, v3(x * 0.97f, 0.84f, 0.06f), v3(x * 0.97f, 0.84f, 0.16f), v3(x * 0.80f, 1.37f, 0.16f), v3(x * 0.80f, 1.37f, 0.06f), negro);
            mesh_quad_flat(m, v3(x * 1.003f, 0.40f, -1.25f), v3(x * 1.003f, 0.40f, 1.45f), v3(x * 1.003f, 0.46f, 1.45f), v3(x * 1.003f, 0.46f, -1.25f), negro);
            for (k = 0; k < 2; k++) {   /* lineas de las 4 puertas */
                float z = k ? 1.12f : 0.11f;
                mesh_quad_flat(m, v3(x * 1.002f, 0.30f, z - 0.01f), v3(x * 1.002f, 0.30f, z + 0.01f), v3(x * 1.002f, 0.82f, z + 0.01f), v3(x * 1.002f, 0.82f, z - 0.01f), dark);
            }
            mesh_box(m, v3(x - (sd > 0 ? 0 : 0.14f), 0.86f, -0.92f), v3(x + (sd > 0 ? 0.14f : 0), 0.96f, -0.80f), negro, negro);   /* espejo */
            /* faros rectangulares y cuartos de esquina */
            mesh_quad_flat(m, v3(sd * 0.34f, 0.50f, -2.205f), v3(sd * 0.72f, 0.50f, -2.205f), v3(sd * 0.72f, 0.62f, -2.205f), v3(sd * 0.34f, 0.62f, -2.205f), faro);
            mesh_quad_flat(m, v3(sd * 0.72f, 0.50f, -2.205f), v3(sd * 0.82f, 0.50f, -2.205f), v3(sd * 0.82f, 0.60f, -2.195f), v3(sd * 0.72f, 0.60f, -2.205f), ambar);
            /* calaveras anchas */
            mesh_quad_flat(m, v3(sd * 0.40f, 0.58f, 2.155f), v3(sd * 0.82f, 0.58f, 2.155f), v3(sd * 0.82f, 0.72f, 2.155f), v3(sd * 0.40f, 0.72f, 2.155f), rojo);
            mesh_quad_flat(m, v3(sd * 0.28f, 0.58f, 2.155f), v3(sd * 0.40f, 0.58f, 2.155f), v3(sd * 0.40f, 0.72f, 2.155f), v3(sd * 0.28f, 0.72f, 2.155f), ambar);
            /* tapones plateados por fuera de las ruedas */
            mesh_box(m, v3(sd * 0.975f - 0.01f, 0.13f, -1.45f - 0.18f), v3(sd * 0.975f + 0.01f, 0.49f, -1.45f + 0.18f), tapon, tapon);
            mesh_box(m, v3(sd * 0.975f - 0.01f, 0.13f, 1.40f - 0.18f), v3(sd * 0.975f + 0.01f, 0.49f, 1.40f + 0.18f), tapon, tapon);
        }
        /* parrilla negra entre los faros */
        mesh_quad_flat(m, v3(-0.34f, 0.50f, -2.205f), v3(0.34f, 0.50f, -2.205f), v3(0.34f, 0.62f, -2.205f), v3(-0.34f, 0.62f, -2.205f), negro);
        for (k = 0; k < 3; k++) {
            float y = 0.525f + k * 0.035f;
            mesh_quad_flat(m, v3(-0.32f, y, -2.21f), v3(0.32f, y, -2.21f), v3(0.32f, y + 0.012f, -2.21f), v3(-0.32f, y + 0.012f, -2.21f), col(70, 70, 76));
        }
        /* defensas negras anchas */
        mesh_box(m, v3(-0.88f, 0.24f, -2.32f), v3(0.88f, 0.48f, -2.12f), negro, negro);
        mesh_box(m, v3(-0.88f, 0.26f, 2.10f), v3(0.88f, 0.50f, 2.28f), negro, negro);
        mesh_quad_flat(m, v3(-0.22f, 0.30f, 2.285f), v3(0.22f, 0.30f, 2.285f), v3(0.22f, 0.42f, 2.285f), v3(-0.22f, 0.42f, 2.285f), col(236, 236, 228));   /* placa */
        wheel(m, -0.86f, -1.45f, 0.31f, 0.22f); wheel(m, 0.86f, -1.45f, 0.31f, 0.22f);
        wheel(m, -0.86f, 1.40f, 0.31f, 0.22f); wheel(m, 0.86f, 1.40f, 0.31f, 0.22f);
        break;
    }
    case 26: { /* HAYATE 94: coupe de carreras de los 90, nariz redonda, faros delgados, aleron alto y franja central */
        Sec s[] = {{-2.30f, 0.84f, 0.24f, 0.40f, 0.52f}, {-1.55f, 0.94f, 0.22f, 0.52f, 0.68f}, {-0.30f, 0.96f, 0.22f, 0.58f, 0.76f},
                   {1.20f, 0.96f, 0.23f, 0.60f, 0.80f}, {2.20f, 0.92f, 0.28f, 0.60f, 0.78f}};
        Col negro = col(20, 20, 24), faro = col(240, 242, 236), rojo = col(214, 30, 30), ambar = col(244, 150, 30);
        int sd;
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.16f, accent);
        cabin(m, -0.75f, 0.95f, 0.78f, 1.18f, 0.84f, 0.62f, 0.75f, 0.70f, paint, glass);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.965f;
            /* franja lateral baja en el color de acento */
            mesh_quad_flat(m, v3(x * 1.004f, 0.30f, -1.90f), v3(x * 1.004f, 0.30f, 2.00f), v3(x * 1.004f, 0.38f, 2.00f), v3(x * 1.004f, 0.38f, -1.90f), accent);
            /* faros delgados que envuelven la esquina */
            mesh_quad_flat(m, v3(sd * 0.30f, 0.46f, -2.31f), v3(sd * 0.78f, 0.44f, -2.27f), v3(sd * 0.78f, 0.51f, -2.25f), v3(sd * 0.30f, 0.53f, -2.30f), faro);
            mesh_quad_flat(m, v3(sd * 0.78f, 0.44f, -2.27f), v3(sd * 0.84f, 0.44f, -2.10f), v3(sd * 0.84f, 0.50f, -2.10f), v3(sd * 0.78f, 0.51f, -2.25f), ambar);
            /* tomas bajas a los lados de la toma central */
            mesh_quad_flat(m, v3(sd * 0.40f, 0.24f, -2.31f), v3(sd * 0.72f, 0.24f, -2.29f), v3(sd * 0.72f, 0.34f, -2.29f), v3(sd * 0.40f, 0.34f, -2.31f), negro);
            /* pilotos traseros de dos tonos */
            mesh_quad_flat(m, v3(sd * 0.36f, 0.60f, 2.205f), v3(sd * 0.70f, 0.60f, 2.205f), v3(sd * 0.70f, 0.70f, 2.205f), v3(sd * 0.36f, 0.70f, 2.205f), rojo);
            mesh_quad_flat(m, v3(sd * 0.70f, 0.60f, 2.205f), v3(sd * 0.84f, 0.60f, 2.205f), v3(sd * 0.84f, 0.70f, 2.205f), v3(sd * 0.70f, 0.70f, 2.205f), ambar);
            /* soportes del aleron y escape */
            mesh_box(m, v3(sd * 0.62f - 0.04f, 0.78f, 1.80f), v3(sd * 0.62f + 0.04f, 1.08f, 1.98f), paint, paint);
            mesh_box(m, v3(sd * 0.55f - 0.08f, 0.24f, 2.16f), v3(sd * 0.55f + 0.08f, 0.34f, 2.28f), col(170, 172, 180), negro);
        }
        mesh_quad_flat(m, v3(-0.24f, 0.22f, -2.315f), v3(0.24f, 0.22f, -2.315f), v3(0.24f, 0.36f, -2.315f), v3(-0.24f, 0.36f, -2.315f), negro);   /* toma central */
        mesh_quad_flat(m, v3(-0.36f, 0.60f, 2.205f), v3(0.36f, 0.60f, 2.205f), v3(0.36f, 0.70f, 2.205f), v3(-0.36f, 0.70f, 2.205f), negro);   /* panel entre pilotos */
        mesh_box(m, v3(-0.92f, 1.08f, 1.74f), v3(0.92f, 1.14f, 2.10f), paint, accent);   /* aleron alto */
        mesh_box(m, v3(-0.94f, 0.96f, 1.76f), v3(-0.90f, 1.16f, 2.10f), accent, accent);
        mesh_box(m, v3(0.90f, 0.96f, 1.76f), v3(0.94f, 1.16f, 2.10f), accent, accent);
        wheel(m, -0.88f, -1.45f, 0.34f, 0.28f); wheel(m, 0.88f, -1.45f, 0.34f, 0.28f);
        wheel(m, -0.90f, 1.42f, 0.35f, 0.32f); wheel(m, 0.90f, 1.42f, 0.35f, 0.32f);
        break;
    }
    case 27: { /* COLIBRI 3P: hatchback corto de 3 puertas, nariz baja y redonda, techo largo y cola casi vertical */
        Sec s[] = {{-1.95f, 0.82f, 0.26f, 0.42f, 0.56f}, {-1.35f, 0.88f, 0.24f, 0.52f, 0.72f}, {-0.20f, 0.90f, 0.24f, 0.58f, 0.80f},
                   {1.30f, 0.90f, 0.26f, 0.60f, 0.84f}, {1.88f, 0.88f, 0.30f, 0.60f, 0.82f}};
        Col negro = col(24, 24, 28), faro = col(240, 242, 236), rojo = col(212, 28, 30), ambar = col(242, 150, 30);
        int sd;
        body(m, s, 5, paint, dark);
        /* cabina larga: parabrisas inclinado y techo que llega casi a la cola */
        cabin(m, -0.90f, 1.82f, 0.82f, 1.36f, 0.86f, 0.74f, 0.70f, 0.22f, paint, glass);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.905f;
            /* una sola puerta larga por lado y la ventana de cuarto */
            mesh_quad_flat(m, v3(x * 1.002f, 0.30f, 0.55f), v3(x * 1.002f, 0.30f, 0.57f), v3(x * 1.002f, 0.82f, 0.57f), v3(x * 1.002f, 0.82f, 0.55f), dark);
            mesh_quad_flat(m, v3(x * 0.97f, 0.84f, 0.55f), v3(x * 0.97f, 0.84f, 0.68f), v3(x * 0.82f, 1.35f, 0.68f), v3(x * 0.82f, 1.35f, 0.55f), paint);
            /* faldon y franja en el color de acento */
            mesh_quad_flat(m, v3(x * 1.004f, 0.27f, -1.25f), v3(x * 1.004f, 0.27f, 1.35f), v3(x * 1.004f, 0.34f, 1.35f), v3(x * 1.004f, 0.34f, -1.25f), accent);
            /* faros delgados inclinados */
            mesh_quad_flat(m, v3(sd * 0.28f, 0.50f, -1.965f), v3(sd * 0.74f, 0.48f, -1.94f), v3(sd * 0.72f, 0.56f, -1.92f), v3(sd * 0.30f, 0.58f, -1.95f), faro);
            mesh_quad_flat(m, v3(sd * 0.74f, 0.48f, -1.94f), v3(sd * 0.82f, 0.47f, -1.84f), v3(sd * 0.81f, 0.54f, -1.84f), v3(sd * 0.72f, 0.56f, -1.92f), ambar);
            /* pilotos verticales a los lados de la cola */
            mesh_quad_flat(m, v3(sd * 0.56f, 0.56f, 1.885f), v3(sd * 0.84f, 0.56f, 1.885f), v3(sd * 0.82f, 0.86f, 1.865f), v3(sd * 0.58f, 0.86f, 1.865f), rojo);
            mesh_box(m, v3(sd * 0.55f - 0.07f, 0.24f, 1.84f), v3(sd * 0.55f + 0.07f, 0.32f, 1.98f), col(170, 172, 180), negro);
        }
        /* toma ancha baja al frente, defensas en el color de acento y medallon trasero */
        mesh_quad_flat(m, v3(-0.62f, 0.28f, -1.965f), v3(0.62f, 0.28f, -1.965f), v3(0.62f, 0.40f, -1.965f), v3(-0.62f, 0.40f, -1.965f), negro);
        mesh_box(m, v3(-0.86f, 0.22f, -2.02f), v3(0.86f, 0.28f, -1.90f), accent, accent);
        mesh_quad(m, v3(-0.64f, 1.36f, 1.60f), v3(0.64f, 1.36f, 1.60f), v3(0.74f, 0.90f, 1.86f), v3(-0.74f, 0.90f, 1.86f), glass);
        mesh_box(m, v3(-0.70f, 1.32f, 1.52f), v3(0.70f, 1.38f, 1.72f), paint, accent);   /* alerito sobre el medallon */
        wheel(m, -0.84f, -1.25f, 0.31f, 0.24f); wheel(m, 0.84f, -1.25f, 0.31f, 0.24f);
        wheel(m, -0.84f, 1.22f, 0.31f, 0.24f); wheel(m, 0.84f, 1.22f, 0.31f, 0.24f);
        break;
    }
    case 28: { /* FURGONETA R: la furgoneta del trafico rebajada, con salpicaderas anchas, faldones, labio y aleron grande */
        Col gris = col(214, 214, 220), negro = col(22, 22, 26), cromo = col(190, 192, 200), faro = col(255, 246, 210), rojo = col(220, 24, 24);
        int sd, ax;
        mesh_box(m, v3(-1.0f, 0.20f, -2.6f), v3(1.0f, 2.15f, 2.6f), paint, gris);
        mesh_quad_flat(m, v3(-0.9f, 1.20f, -2.61f), v3(0.9f, 1.20f, -2.61f), v3(0.9f, 1.98f, -2.61f), v3(-0.9f, 1.98f, -2.61f), glass);
        for (sd = -1; sd <= 1; sd += 2) {
            float xo = sd * 1.0f;
            /* ventanillas de la cabina y franja de carreras a lo largo */
            mesh_quad_flat(m, v3(xo * 1.003f, 1.25f, -2.45f), v3(xo * 1.003f, 1.25f, -1.40f), v3(xo * 1.003f, 1.95f, -1.40f), v3(xo * 1.003f, 1.95f, -2.25f), glass);
            mesh_quad_flat(m, v3(xo * 1.004f, 0.95f, -2.55f), v3(xo * 1.004f, 0.95f, 2.55f), v3(xo * 1.004f, 1.10f, 2.55f), v3(xo * 1.004f, 1.10f, -2.55f), accent);
            /* salpicaderas anchas sobre cada rueda */
            for (ax = -1; ax <= 1; ax += 2) {
                float z = ax * 1.70f;
                mesh_box(m, v3(sd > 0 ? 0.98f : -1.24f, 0.30f, z - 0.62f), v3(sd > 0 ? 1.24f : -0.98f, 0.90f, z + 0.62f), paint, paint);
                mesh_quad_flat(m, v3(sd * 1.245f, 0.30f, z - 0.62f), v3(sd * 1.245f, 0.30f, z + 0.62f), v3(sd * 1.245f, 0.36f, z + 0.62f), v3(sd * 1.245f, 0.36f, z - 0.62f), negro);
            }
            /* faldon entre las salpicaderas */
            mesh_box(m, v3(sd > 0 ? 0.98f : -1.20f, 0.16f, -1.08f), v3(sd > 0 ? 1.20f : -0.98f, 0.36f, 1.08f), negro, negro);
            /* escapes laterales dobles */
            mesh_box(m, v3(sd > 0 ? 1.18f : -1.30f, 0.20f, 0.70f), v3(sd > 0 ? 1.30f : -1.18f, 0.30f, 0.98f), cromo, negro);
            /* soportes del aleron */
            mesh_box(m, v3(sd * 0.70f - 0.05f, 2.15f, 2.20f), v3(sd * 0.70f + 0.05f, 2.50f, 2.45f), negro, negro);
        }
        /* labio delantero, parrilla, faros y defensa trasera */
        mesh_box(m, v3(-1.18f, 0.12f, -2.80f), v3(1.18f, 0.20f, -2.48f), negro, negro);
        mesh_box(m, v3(-1.0f, 0.20f, -2.64f), v3(1.0f, 0.55f, -2.56f), negro, negro);
        mesh_quad_flat(m, v3(-0.55f, 0.62f, -2.61f), v3(0.55f, 0.62f, -2.61f), v3(0.55f, 1.00f, -2.61f), v3(-0.55f, 1.00f, -2.61f), negro);
        for (sd = -1; sd <= 1; sd += 2) {
            mesh_quad_flat(m, v3(sd * 0.60f, 0.70f, -2.612f), v3(sd * 0.92f, 0.70f, -2.612f), v3(sd * 0.92f, 0.92f, -2.612f), v3(sd * 0.60f, 0.92f, -2.612f), faro);
            mesh_quad_flat(m, v3(sd * 0.62f, 0.70f, 2.612f), v3(sd * 0.92f, 0.70f, 2.612f), v3(sd * 0.92f, 1.30f, 2.612f), v3(sd * 0.62f, 1.30f, 2.612f), rojo);
        }
        mesh_box(m, v3(-1.10f, 0.18f, 2.52f), v3(1.10f, 0.42f, 2.72f), negro, negro);
        /* aleron grande sobre el techo, en el color de acento */
        mesh_box(m, v3(-1.10f, 2.50f, 2.05f), v3(1.10f, 2.58f, 2.62f), accent, accent);
        mesh_box(m, v3(-1.12f, 2.30f, 2.05f), v3(-1.08f, 2.70f, 2.62f), negro, negro);
        mesh_box(m, v3(1.08f, 2.30f, 2.05f), v3(1.12f, 2.70f, 2.62f), negro, negro);
        /* ruedas anchas metidas en las salpicaderas */
        wheel(m, -1.08f, -1.70f, 0.38f, 0.36f); wheel(m, 1.08f, -1.70f, 0.38f, 0.36f);
        wheel(m, -1.08f, 1.70f, 0.39f, 0.40f); wheel(m, 1.08f, 1.70f, 0.39f, 0.40f);
        break;
    }
    case 29: { /* RUGIDO 500: stock car de ovalo, costados planos, redes en las ventanas, aleron recto y labio bajo */
        Sec s[] = {{-2.50f, 0.95f, 0.18f, 0.48f, 0.66f}, {-1.70f, 0.98f, 0.16f, 0.58f, 0.80f}, {0.30f, 0.99f, 0.16f, 0.62f, 0.84f},
                   {1.80f, 0.99f, 0.18f, 0.64f, 0.88f}, {2.45f, 0.97f, 0.22f, 0.64f, 0.88f}};
        Col negro = col(18, 18, 22), blanco = col(245, 245, 245), faro = col(250, 240, 200);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, -0.70f, 1.35f, 0.84f, 1.28f, 0.86f, 0.70f, 0.75f, 0.70f, paint, glass);
        stripe(m, s, 5, 0.12f, accent);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.995f;
            /* franja de acento, circulo blanco en la puerta y red negra en la ventana */
            mesh_quad_flat(m, v3(x * 1.003f, 0.30f, -2.30f), v3(x * 1.003f, 0.30f, 2.30f), v3(x * 1.003f, 0.42f, 2.30f), v3(x * 1.003f, 0.42f, -2.30f), accent);
            mesh_quad_flat(m, v3(x * 1.004f, 0.46f, -0.40f), v3(x * 1.004f, 0.46f, 0.40f), v3(x * 1.004f, 0.80f, 0.40f), v3(x * 1.004f, 0.80f, -0.40f), blanco);
            mesh_quad_flat(m, v3(sd * 0.83f, 0.90f, -0.05f), v3(sd * 0.83f, 0.90f, 0.55f), v3(sd * 0.73f, 1.24f, 0.55f), v3(sd * 0.73f, 1.24f, -0.05f), negro);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.52f, -2.505f), v3(sd * 0.82f, 0.52f, -2.505f), v3(sd * 0.82f, 0.62f, -2.505f), v3(sd * 0.40f, 0.62f, -2.505f), faro);
            mesh_quad_flat(m, v3(sd * 0.30f, 0.66f, 2.455f), v3(sd * 0.88f, 0.66f, 2.455f), v3(sd * 0.88f, 0.76f, 2.455f), v3(sd * 0.30f, 0.76f, 2.455f), col(200, 20, 20));
        }
        mesh_box(m, v3(-1.00f, 0.10f, -2.62f), v3(1.00f, 0.18f, -2.40f), negro, negro);   /* labio bajo */
        mesh_quad_flat(m, v3(-0.36f, 0.30f, -2.505f), v3(0.36f, 0.30f, -2.505f), v3(0.36f, 0.48f, -2.505f), v3(-0.36f, 0.48f, -2.505f), negro);
        mesh_box(m, v3(-0.92f, 0.88f, 2.30f), v3(0.92f, 1.12f, 2.40f), negro, negro);   /* aleron recto */
        mesh_box(m, v3(-0.30f, 1.29f, 0.50f), v3(0.30f, 1.30f, 0.80f), accent, accent);   /* aletas del techo */
        wheel(m, -0.90f, -1.55f, 0.36f, 0.32f); wheel(m, 0.90f, -1.55f, 0.36f, 0.32f);
        wheel(m, -0.90f, 1.55f, 0.36f, 0.32f); wheel(m, 0.90f, 1.55f, 0.36f, 0.32f);
        break;
    }
    case 30: { /* CENTELLA C: prototipo de resistencia, muy bajo, cabina cerrada al centro, ruedas traseras cubiertas y cola larga */
        Sec s[] = {{-2.55f, 1.00f, 0.12f, 0.24f, 0.30f}, {-1.80f, 1.04f, 0.12f, 0.42f, 0.52f}, {-0.70f, 1.05f, 0.12f, 0.50f, 0.62f},
                   {0.80f, 1.06f, 0.12f, 0.56f, 0.70f}, {2.10f, 1.04f, 0.14f, 0.56f, 0.66f}, {2.65f, 0.98f, 0.18f, 0.50f, 0.58f}};
        Col negro = col(18, 18, 22), faro = col(250, 246, 220);
        int sd;
        body(m, s, 6, paint, dark);
        cabin(m, -0.90f, 0.70f, 0.62f, 1.05f, 0.62f, 0.46f, 0.65f, 0.40f, paint, glass);
        mesh_quad(m, v3(-0.46f, 1.05f, 0.30f), v3(0.46f, 1.05f, 0.30f), v3(0.30f, 0.70f, 2.00f), v3(-0.30f, 0.70f, 2.00f), paint);   /* cubierta del motor */
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.065f;
            /* franja de acento baja a lo largo y faldon que cubre la rueda trasera */
            mesh_quad_flat(m, v3(x * 1.002f, 0.20f, -2.20f), v3(x * 1.002f, 0.20f, 2.50f), v3(x * 1.002f, 0.34f, 2.50f), v3(x * 1.002f, 0.34f, -2.20f), accent);
            mesh_box(m, v3(sd > 0 ? 1.05f : -1.11f, 0.14f, 1.10f), v3(sd > 0 ? 1.11f : -1.05f, 0.62f, 1.95f), paint, paint);
            /* faros carenados bajos */
            mesh_quad_flat(m, v3(sd * 0.55f, 0.24f, -2.40f), v3(sd * 0.92f, 0.24f, -2.20f), v3(sd * 0.92f, 0.36f, -2.18f), v3(sd * 0.55f, 0.36f, -2.38f), faro);
            /* placas del aleron trasero */
            mesh_box(m, v3(sd * 1.00f - 0.02f, 0.40f, 2.30f), v3(sd * 1.00f + 0.02f, 0.92f, 2.80f), paint, paint);
        }
        mesh_box(m, v3(-1.00f, 0.84f, 2.40f), v3(1.00f, 0.90f, 2.80f), accent, accent);   /* aleron de cola, de lado a lado */
        mesh_box(m, v3(-0.98f, 0.58f, 2.60f), v3(0.98f, 0.62f, 2.80f), paint, paint);
        mesh_quad_flat(m, v3(-0.80f, 0.20f, 2.655f), v3(0.80f, 0.20f, 2.655f), v3(0.80f, 0.42f, 2.655f), v3(-0.80f, 0.42f, 2.655f), negro);
        mesh_box(m, v3(-0.70f, 0.24f, 2.66f), v3(-0.30f, 0.32f, 2.67f), col(220, 20, 20), col(220, 20, 20));
        mesh_box(m, v3(0.30f, 0.24f, 2.66f), v3(0.70f, 0.32f, 2.67f), col(220, 20, 20), col(220, 20, 20));
        wheel(m, -0.92f, -1.65f, 0.34f, 0.30f); wheel(m, 0.92f, -1.65f, 0.34f, 0.30f);
        wheel(m, -0.92f, 1.52f, 0.35f, 0.36f); wheel(m, 0.92f, 1.52f, 0.35f, 0.36f);
        break;
    }
    case 31: { /* ESTRELLA GT1: GT de carreras de los 90, ancho y bajo, toma en el techo, aleron gigante y labio */
        Sec s[] = {{-2.45f, 0.96f, 0.16f, 0.32f, 0.42f}, {-1.70f, 1.06f, 0.14f, 0.48f, 0.64f}, {-0.40f, 1.08f, 0.14f, 0.56f, 0.74f},
                   {1.00f, 1.10f, 0.15f, 0.60f, 0.80f}, {2.00f, 1.08f, 0.18f, 0.60f, 0.80f}, {2.45f, 1.02f, 0.22f, 0.58f, 0.76f}};
        Col negro = col(16, 16, 20), faro = col(250, 244, 210);
        int sd;
        body(m, s, 6, paint, dark);
        cabin(m, -0.75f, 0.95f, 0.76f, 1.12f, 0.78f, 0.58f, 0.80f, 0.60f, paint, glass);
        mesh_box(m, v3(-0.16f, 1.12f, 0.20f), v3(0.16f, 1.26f, 0.95f), accent, accent);   /* toma de aire en el techo */
        mesh_quad_flat(m, v3(-0.12f, 1.14f, 0.195f), v3(0.12f, 1.14f, 0.195f), v3(0.12f, 1.24f, 0.195f), v3(-0.12f, 1.24f, 0.195f), negro);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.085f;
            mesh_quad_flat(m, v3(x * 1.003f, 0.20f, -2.10f), v3(x * 1.003f, 0.20f, 2.20f), v3(x * 1.003f, 0.32f, 2.20f), v3(x * 1.003f, 0.32f, -2.10f), accent);
            mesh_quad_flat(m, v3(x * 1.004f, 0.36f, 0.60f), v3(x * 1.004f, 0.36f, 1.20f), v3(x * 1.004f, 0.62f, 1.10f), v3(x * 1.004f, 0.56f, 0.60f), negro);   /* toma lateral */
            mesh_quad_flat(m, v3(sd * 0.42f, 0.36f, -2.455f), v3(sd * 0.90f, 0.32f, -2.30f), v3(sd * 0.90f, 0.40f, -2.28f), v3(sd * 0.42f, 0.44f, -2.44f), faro);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.62f, 2.455f), v3(sd * 0.90f, 0.62f, 2.455f), v3(sd * 0.90f, 0.72f, 2.455f), v3(sd * 0.40f, 0.72f, 2.455f), col(220, 24, 24));
            mesh_box(m, v3(sd * 0.70f - 0.04f, 0.76f, 1.95f), v3(sd * 0.70f + 0.04f, 1.22f, 2.20f), negro, negro);   /* soportes */
            mesh_box(m, v3(sd * 1.05f - 0.02f, 1.00f, 1.90f), v3(sd * 1.05f + 0.02f, 1.34f, 2.50f), accent, accent);   /* placas */
        }
        mesh_box(m, v3(-1.05f, 1.22f, 1.90f), v3(1.05f, 1.30f, 2.50f), accent, accent);   /* aleron gigante */
        mesh_box(m, v3(-1.10f, 0.08f, -2.62f), v3(1.10f, 0.15f, -2.30f), negro, negro);   /* labio */
        mesh_quad_flat(m, v3(-0.36f, 0.20f, -2.455f), v3(0.36f, 0.20f, -2.455f), v3(0.36f, 0.34f, -2.455f), v3(-0.36f, 0.34f, -2.455f), negro);
        mesh_quad_flat(m, v3(-0.90f, 0.18f, 2.455f), v3(0.90f, 0.18f, 2.455f), v3(0.90f, 0.40f, 2.455f), v3(-0.90f, 0.40f, 2.455f), negro);
        wheel(m, -0.94f, -1.55f, 0.36f, 0.34f); wheel(m, 0.94f, -1.55f, 0.36f, 0.34f);
        wheel(m, -0.96f, 1.50f, 0.37f, 0.40f); wheel(m, 0.96f, 1.50f, 0.37f, 0.40f);
        break;
    }
    case 32: { /* BISONTE GTR: muscle car ancho, cofre largo con toma, salpicaderas abiertas, labio y aleron de cola */
        Sec s[] = {{-2.55f, 0.98f, 0.20f, 0.56f, 0.72f}, {-1.80f, 1.04f, 0.18f, 0.64f, 0.84f}, {-0.30f, 1.06f, 0.18f, 0.66f, 0.88f},
                   {1.60f, 1.06f, 0.20f, 0.68f, 0.92f}, {2.45f, 1.02f, 0.26f, 0.68f, 0.92f}};
        Col negro = col(18, 18, 22), faro = col(250, 244, 214), rojo = col(214, 22, 26);
        int sd, k;
        body(m, s, 5, paint, dark);
        cabin(m, -0.30f, 1.40f, 0.88f, 1.30f, 0.84f, 0.64f, 0.70f, 0.85f, paint, glass);
        mesh_box(m, v3(-0.34f, 0.86f, -1.70f), v3(0.34f, 1.00f, -0.70f), paint, paint);   /* toma del cofre */
        mesh_quad_flat(m, v3(-0.28f, 0.88f, -1.705f), v3(0.28f, 0.88f, -1.705f), v3(0.28f, 0.98f, -1.705f), v3(-0.28f, 0.98f, -1.705f), negro);
        for (k = -1; k <= 1; k += 2) {   /* dos franjas de acento por todo lo largo */
            float x0 = k * 0.12f, x1 = k * 0.30f;
            mesh_quad_flat(m, v3(x0, 1.005f, -1.70f), v3(x1, 1.005f, -1.70f), v3(x1, 0.735f, -2.55f), v3(x0, 0.735f, -2.55f), accent);
            mesh_quad_flat(m, v3(x0, 0.935f, 1.62f), v3(x1, 0.935f, 1.62f), v3(x1, 0.935f, 2.45f), v3(x0, 0.935f, 2.45f), accent);
        }
        for (sd = -1; sd <= 1; sd += 2) {
            int ax;
            for (ax = -1; ax <= 1; ax += 2)   /* salpicaderas atornilladas */
                mesh_box(m, v3(sd > 0 ? 1.04f : -1.18f, 0.24f, ax * 1.55f - 0.55f), v3(sd > 0 ? 1.18f : -1.04f, 0.70f, ax * 1.55f + 0.55f), negro, negro);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.56f, -2.555f), v3(sd * 0.86f, 0.56f, -2.555f), v3(sd * 0.86f, 0.68f, -2.555f), v3(sd * 0.40f, 0.68f, -2.555f), faro);
            mesh_quad_flat(m, v3(sd * 0.30f, 0.66f, 2.455f), v3(sd * 0.88f, 0.66f, 2.455f), v3(sd * 0.88f, 0.80f, 2.455f), v3(sd * 0.30f, 0.80f, 2.455f), rojo);
            mesh_box(m, v3(sd * 0.55f - 0.09f, 0.22f, 2.40f), v3(sd * 0.55f + 0.09f, 0.34f, 2.56f), col(180, 182, 190), negro);
        }
        mesh_quad_flat(m, v3(-0.38f, 0.40f, -2.555f), v3(0.38f, 0.40f, -2.555f), v3(0.38f, 0.68f, -2.555f), v3(-0.38f, 0.68f, -2.555f), negro);
        mesh_box(m, v3(-1.10f, 0.12f, -2.70f), v3(1.10f, 0.20f, -2.45f), negro, negro);   /* labio */
        mesh_box(m, v3(-0.95f, 0.92f, 2.20f), v3(0.95f, 1.10f, 2.50f), negro, negro);   /* aleron de cola */
        wheel(m, -0.98f, -1.55f, 0.37f, 0.34f); wheel(m, 0.98f, -1.55f, 0.37f, 0.34f);
        wheel(m, -1.00f, 1.55f, 0.38f, 0.40f); wheel(m, 1.00f, 1.55f, 0.38f, 0.40f);
        break;
    }
    case 33: { /* GUAYINA RS: guayina deportiva baja, techo largo hasta la cola, aleron en el techo y rines grandes */
        Sec s[] = {{-2.40f, 0.86f, 0.24f, 0.50f, 0.66f}, {-1.60f, 0.92f, 0.22f, 0.58f, 0.80f}, {0.20f, 0.94f, 0.22f, 0.62f, 0.84f},
                   {1.80f, 0.94f, 0.24f, 0.64f, 0.86f}, {2.40f, 0.92f, 0.28f, 0.64f, 0.86f}};
        Col negro = col(20, 20, 24), faro = col(244, 246, 240), rojo = col(212, 24, 28);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, -0.85f, 2.30f, 0.84f, 1.36f, 0.88f, 0.76f, 0.75f, 0.12f, paint, glass);
        mesh_box(m, v3(-0.74f, 1.32f, 2.02f), v3(0.74f, 1.38f, 2.30f), paint, accent);   /* aleron del techo */
        mesh_box(m, v3(-0.70f, 1.37f, -0.20f), v3(-0.64f, 1.42f, 1.80f), accent, accent);   /* barras del techo */
        mesh_box(m, v3(0.64f, 1.37f, -0.20f), v3(0.70f, 1.42f, 1.80f), accent, accent);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.925f;
            mesh_quad_flat(m, v3(x * 0.97f, 0.86f, 0.40f), v3(x * 0.97f, 0.86f, 0.52f), v3(x * 0.84f, 1.35f, 0.52f), v3(x * 0.84f, 1.35f, 0.40f), paint);
            mesh_quad_flat(m, v3(x * 0.97f, 0.86f, 1.40f), v3(x * 0.97f, 0.86f, 1.52f), v3(x * 0.84f, 1.35f, 1.52f), v3(x * 0.84f, 1.35f, 1.40f), paint);
            mesh_quad_flat(m, v3(x * 1.004f, 0.26f, -1.30f), v3(x * 1.004f, 0.26f, 1.70f), v3(x * 1.004f, 0.33f, 1.70f), v3(x * 1.004f, 0.33f, -1.30f), accent);
            mesh_quad_flat(m, v3(sd * 0.36f, 0.52f, -2.405f), v3(sd * 0.82f, 0.50f, -2.38f), v3(sd * 0.82f, 0.60f, -2.36f), v3(sd * 0.36f, 0.62f, -2.40f), faro);
            mesh_quad_flat(m, v3(sd * 0.50f, 0.58f, 2.405f), v3(sd * 0.88f, 0.58f, 2.405f), v3(sd * 0.88f, 0.84f, 2.405f), v3(sd * 0.50f, 0.84f, 2.405f), rojo);
            mesh_box(m, v3(sd * 0.55f - 0.08f, 0.24f, 2.36f), v3(sd * 0.55f + 0.08f, 0.34f, 2.50f), col(176, 178, 186), negro);
        }
        mesh_quad_flat(m, v3(-0.34f, 0.30f, -2.405f), v3(0.34f, 0.30f, -2.405f), v3(0.34f, 0.50f, -2.405f), v3(-0.34f, 0.50f, -2.405f), negro);
        mesh_quad_flat(m, v3(-0.50f, 0.88f, 2.40f), v3(0.50f, 0.88f, 2.40f), v3(0.56f, 1.30f, 2.28f), v3(-0.56f, 1.30f, 2.28f), glass);   /* medallon */
        wheel(m, -0.88f, -1.52f, 0.35f, 0.30f); wheel(m, 0.88f, -1.52f, 0.35f, 0.30f);
        wheel(m, -0.88f, 1.55f, 0.35f, 0.30f); wheel(m, 0.88f, 1.55f, 0.35f, 0.30f);
        break;
    }
    case 34: { /* LANZA FF: coupe de cofre largo y cabina atras, toma en el cofre y aleron alto sobre la cajuela */
        Sec s[] = {{-2.45f, 0.88f, 0.24f, 0.46f, 0.62f}, {-1.70f, 0.94f, 0.22f, 0.56f, 0.76f}, {-0.20f, 0.96f, 0.22f, 0.60f, 0.82f},
                   {1.50f, 0.96f, 0.24f, 0.62f, 0.86f}, {2.25f, 0.94f, 0.28f, 0.62f, 0.86f}};
        Col negro = col(18, 18, 22), faro = col(244, 246, 240), rojo = col(214, 24, 28);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, 0.05f, 1.55f, 0.84f, 1.26f, 0.84f, 0.64f, 0.70f, 0.50f, paint, glass);
        mesh_box(m, v3(-0.26f, 0.80f, -1.10f), v3(0.26f, 0.88f, -0.55f), paint, paint);   /* toma del cofre */
        mesh_quad_flat(m, v3(-0.22f, 0.81f, -1.105f), v3(0.22f, 0.81f, -1.105f), v3(0.22f, 0.87f, -1.105f), v3(-0.22f, 0.87f, -1.105f), negro);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.965f;
            mesh_quad_flat(m, v3(x * 1.004f, 0.26f, -1.90f), v3(x * 1.004f, 0.26f, 2.00f), v3(x * 1.004f, 0.36f, 2.00f), v3(x * 1.004f, 0.36f, -1.90f), accent);
            mesh_quad_flat(m, v3(sd * 0.34f, 0.48f, -2.455f), v3(sd * 0.84f, 0.46f, -2.42f), v3(sd * 0.84f, 0.55f, -2.40f), v3(sd * 0.34f, 0.57f, -2.45f), faro);
            mesh_quad_flat(m, v3(sd * 0.36f, 0.62f, 2.255f), v3(sd * 0.88f, 0.62f, 2.255f), v3(sd * 0.88f, 0.74f, 2.255f), v3(sd * 0.36f, 0.74f, 2.255f), rojo);
            /* soportes altos del aleron, en la orilla de la cajuela */
            mesh_box(m, v3(sd * 0.74f - 0.04f, 0.86f, 1.82f), v3(sd * 0.74f + 0.04f, 1.20f, 2.10f), paint, paint);
            mesh_box(m, v3(sd * 0.98f - 0.02f, 1.10f, 1.78f), v3(sd * 0.98f + 0.02f, 1.30f, 2.20f), paint, paint);
        }
        mesh_box(m, v3(-0.98f, 1.20f, 1.80f), v3(0.98f, 1.27f, 2.18f), paint, accent);   /* aleron alto */
        mesh_quad_flat(m, v3(-0.38f, 0.26f, -2.455f), v3(0.38f, 0.26f, -2.455f), v3(0.38f, 0.44f, -2.455f), v3(-0.38f, 0.44f, -2.455f), negro);
        mesh_box(m, v3(-0.10f, 0.22f, 2.22f), v3(0.10f, 0.34f, 2.36f), col(176, 178, 186), negro);   /* escape central */
        wheel(m, -0.88f, -1.62f, 0.34f, 0.28f); wheel(m, 0.88f, -1.62f, 0.34f, 0.28f);
        wheel(m, -0.88f, 1.42f, 0.34f, 0.28f); wheel(m, 0.88f, 1.42f, 0.34f, 0.28f);
        break;
    }
    case 35: { /* TIFON 68: prototipo de los 60, nariz baja y redonda, salpicaderas altas, cabina de burbuja y cola larga cortada */
        Sec s[] = {{-2.35f, 0.70f, 0.20f, 0.30f, 0.38f}, {-1.75f, 0.92f, 0.18f, 0.46f, 0.58f}, {-0.80f, 0.96f, 0.18f, 0.50f, 0.56f},
                   {0.60f, 0.98f, 0.18f, 0.52f, 0.60f}, {1.60f, 0.98f, 0.20f, 0.56f, 0.66f}, {2.40f, 0.92f, 0.26f, 0.58f, 0.66f}};
        Col negro = col(20, 20, 24), faro = col(250, 240, 196), cromo = col(200, 202, 210);
        int sd;
        body(m, s, 6, paint, dark);
        stripe(m, s, 6, 0.20f, accent);
        cabin(m, -0.70f, 0.60f, 0.58f, 0.98f, 0.62f, 0.44f, 0.45f, 0.45f, paint, glass);
        mesh_quad(m, v3(-0.44f, 0.98f, 0.15f), v3(0.44f, 0.98f, 0.15f), v3(0.40f, 0.66f, 2.20f), v3(-0.40f, 0.66f, 2.20f), paint);   /* cola larga sobre el motor */
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.90f;
            /* salpicaderas altas y redondas sobre las ruedas */
            mesh_box(m, v3(x - 0.14f, 0.52f, -2.00f), v3(x + 0.14f, 0.66f, -1.10f), paint, paint);
            mesh_box(m, v3(x - 0.14f, 0.58f, 1.00f), v3(x + 0.14f, 0.72f, 1.95f), paint, paint);
            /* faros redondos bajo cubiertas de vidrio */
            mesh_box(m, v3(x - 0.10f, 0.50f, -2.08f), v3(x + 0.10f, 0.62f, -1.98f), faro, faro);
            /* circulo blanco para el numero en la puerta */
            mesh_quad_flat(m, v3(sd * 0.985f, 0.32f, -0.45f), v3(sd * 0.985f, 0.32f, 0.15f), v3(sd * 0.985f, 0.56f, 0.15f), v3(sd * 0.985f, 0.56f, -0.45f), col(250, 250, 250));
            /* pilotos redondos en la cola cortada y escapes */
            mesh_box(m, v3(sd * 0.62f - 0.08f, 0.48f, 2.40f), v3(sd * 0.62f + 0.08f, 0.58f, 2.42f), col(220, 30, 30), col(220, 30, 30));
            mesh_box(m, v3(sd * 0.30f - 0.06f, 0.22f, 2.36f), v3(sd * 0.30f + 0.06f, 0.30f, 2.52f), cromo, negro);
        }
        mesh_quad_flat(m, v3(-0.22f, 0.22f, -2.355f), v3(0.22f, 0.22f, -2.355f), v3(0.22f, 0.32f, -2.355f), v3(-0.22f, 0.32f, -2.355f), negro);   /* boca de radiador */
        mesh_quad_flat(m, v3(-0.70f, 0.30f, 2.405f), v3(0.70f, 0.30f, 2.405f), v3(0.70f, 0.46f, 2.405f), v3(-0.70f, 0.46f, 2.405f), negro);   /* cola cortada */
        mesh_box(m, v3(-0.88f, 0.64f, 2.30f), v3(0.88f, 0.70f, 2.44f), accent, accent);   /* labio de la cola */
        wheel(m, -0.84f, -1.55f, 0.32f, 0.26f); wheel(m, 0.84f, -1.55f, 0.32f, 0.26f);
        wheel(m, -0.86f, 1.48f, 0.33f, 0.32f); wheel(m, 0.86f, 1.48f, 0.33f, 0.32f);
        break;
    }
    case 36: { /* FULMINE 8: GT bajisimo de motor delantero, cofre larguisimo, cabina pegada a la cola, escapes laterales */
        Sec s[] = {{-2.55f, 0.90f, 0.20f, 0.40f, 0.52f}, {-1.80f, 1.00f, 0.18f, 0.54f, 0.68f}, {-0.40f, 1.04f, 0.18f, 0.60f, 0.76f},
                   {1.00f, 1.06f, 0.20f, 0.62f, 0.80f}, {2.20f, 1.02f, 0.24f, 0.60f, 0.76f}};
        Col negro = col(18, 18, 22), faro = col(250, 244, 214), cromo = col(200, 202, 210);
        int sd, k;
        body(m, s, 5, paint, dark);
        cabin(m, 0.25f, 1.65f, 0.80f, 1.12f, 0.78f, 0.58f, 0.65f, 0.55f, paint, glass);
        for (k = -1; k <= 1; k += 2) {   /* dos franjas de acento por todo el auto */
            float x0 = k * 0.10f, x1 = k * 0.26f;
            mesh_quad_flat(m, v3(x0, 0.775f, -0.40f), v3(x1, 0.775f, -0.40f), v3(x1, 0.535f, -2.55f), v3(x0, 0.535f, -2.55f), accent);
            mesh_quad_flat(m, v3(x0, 0.775f, -0.40f), v3(x1, 0.775f, -0.40f), v3(x1, 0.805f, 0.25f), v3(x0, 0.805f, 0.25f), accent);
            mesh_quad_flat(m, v3(x0, 0.785f, 1.65f), v3(x1, 0.785f, 1.65f), v3(x1, 0.775f, 2.20f), v3(x0, 0.775f, 2.20f), accent);
        }
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.045f;
            mesh_quad_flat(m, v3(x * 1.003f, 0.38f, -1.20f), v3(x * 1.003f, 0.38f, -0.70f), v3(x * 1.003f, 0.58f, -0.80f), v3(x * 1.003f, 0.56f, -1.20f), negro);   /* branquias */
            mesh_box(m, v3(sd > 0 ? 1.02f : -1.12f, 0.20f, -0.60f), v3(sd > 0 ? 1.12f : -1.02f, 0.30f, 0.90f), cromo, negro);   /* escape lateral */
            mesh_quad_flat(m, v3(sd * 0.48f, 0.34f, -2.55f), v3(sd * 0.86f, 0.32f, -2.42f), v3(sd * 0.86f, 0.42f, -2.40f), v3(sd * 0.48f, 0.44f, -2.53f), faro);
            mesh_box(m, v3(sd * 0.62f - 0.10f, 0.54f, 2.18f), v3(sd * 0.62f + 0.10f, 0.66f, 2.22f), col(220, 24, 24), col(220, 24, 24));
        }
        mesh_quad_flat(m, v3(-0.40f, 0.22f, -2.555f), v3(0.40f, 0.22f, -2.555f), v3(0.36f, 0.38f, -2.555f), v3(-0.36f, 0.38f, -2.555f), negro);
        mesh_box(m, v3(-0.80f, 0.74f, 2.05f), v3(0.80f, 0.82f, 2.22f), paint, paint);   /* cola de pato */
        wheel(m, -0.94f, -1.70f, 0.36f, 0.32f); wheel(m, 0.94f, -1.70f, 0.36f, 0.32f);
        wheel(m, -0.96f, 1.45f, 0.38f, 0.40f); wheel(m, 0.96f, 1.45f, 0.38f, 0.40f);
        break;
    }
    case 37: { /* RAIJIN V12: hiperauto de motor delantero, labio enorme, salpicaderas anchas y alerón gigante de montana */
        Sec s[] = {{-2.45f, 0.96f, 0.16f, 0.36f, 0.48f}, {-1.70f, 1.06f, 0.14f, 0.52f, 0.70f}, {-0.30f, 1.08f, 0.14f, 0.58f, 0.78f},
                   {1.10f, 1.08f, 0.16f, 0.60f, 0.82f}, {2.30f, 1.04f, 0.22f, 0.58f, 0.78f}};
        Col negro = col(14, 14, 18), faro = col(240, 248, 255);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, -0.10f, 1.40f, 0.80f, 1.16f, 0.80f, 0.58f, 0.70f, 0.60f, paint, glass);
        mesh_quad_flat(m, v3(-0.30f, 0.79f, -1.50f), v3(0.30f, 0.79f, -1.50f), v3(0.30f, 0.80f, -0.70f), v3(-0.30f, 0.80f, -0.70f), negro);   /* rejilla del cofre */
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.085f;
            mesh_quad_flat(m, v3(x * 1.003f, 0.18f, -2.20f), v3(x * 1.003f, 0.18f, 2.20f), v3(x * 1.003f, 0.28f, 2.20f), v3(x * 1.003f, 0.28f, -2.20f), accent);
            mesh_box(m, v3(sd > 0 ? 1.06f : -1.20f, 0.10f, -1.00f), v3(sd > 0 ? 1.20f : -1.06f, 0.22f, 1.00f), negro, negro);   /* faldon */
            mesh_quad_flat(m, v3(sd * 0.40f, 0.36f, -2.455f), v3(sd * 0.92f, 0.32f, -2.28f), v3(sd * 0.92f, 0.38f, -2.26f), v3(sd * 0.40f, 0.42f, -2.44f), faro);
            mesh_quad_flat(m, v3(sd * 0.30f, 0.62f, 2.305f), v3(sd * 0.92f, 0.62f, 2.305f), v3(sd * 0.92f, 0.68f, 2.305f), v3(sd * 0.30f, 0.68f, 2.305f), col(230, 20, 24));
            /* columnas altas del aleron, desde la cola */
            mesh_box(m, v3(sd * 0.80f - 0.05f, 0.78f, 2.05f), v3(sd * 0.80f + 0.05f, 1.62f, 2.40f), negro, negro);
            mesh_box(m, v3(sd * 1.25f - 0.03f, 1.40f, 1.90f), v3(sd * 1.25f + 0.03f, 1.82f, 2.75f), accent, accent);   /* placas */
            mesh_box(m, v3(sd > 0 ? 1.08f : -1.40f, 0.10f, -2.80f), v3(sd > 0 ? 1.40f : -1.08f, 0.14f, -2.30f), negro, negro);   /* canards del labio */
        }
        mesh_box(m, v3(-1.25f, 1.62f, 1.90f), v3(1.25f, 1.72f, 2.75f), accent, accent);   /* alerón gigante */
        mesh_box(m, v3(-1.25f, 1.40f, 2.40f), v3(1.25f, 1.46f, 2.75f), paint, paint);
        mesh_box(m, v3(-1.15f, 0.06f, -2.85f), v3(1.15f, 0.12f, -2.35f), negro, negro);   /* labio enorme */
        mesh_quad_flat(m, v3(-0.50f, 0.18f, -2.455f), v3(0.50f, 0.18f, -2.455f), v3(0.50f, 0.34f, -2.455f), v3(-0.50f, 0.34f, -2.455f), negro);
        mesh_quad_flat(m, v3(-0.90f, 0.16f, 2.305f), v3(0.90f, 0.16f, 2.305f), v3(0.90f, 0.40f, 2.305f), v3(-0.90f, 0.40f, 2.305f), negro);
        wheel(m, -0.96f, -1.65f, 0.36f, 0.36f); wheel(m, 0.96f, -1.65f, 0.36f, 0.36f);
        wheel(m, -0.98f, 1.50f, 0.37f, 0.40f); wheel(m, 0.98f, 1.50f, 0.37f, 0.40f);
        break;
    }
    case 38: { /* VORTICE X: hiperauto de motor central, ancho, bajisimo, cabina burbuja adelantada y cola con ventila */
        Sec s[] = {{-2.40f, 0.92f, 0.16f, 0.30f, 0.38f}, {-1.60f, 1.06f, 0.14f, 0.46f, 0.58f}, {-0.40f, 1.10f, 0.14f, 0.54f, 0.70f},
                   {1.00f, 1.12f, 0.15f, 0.60f, 0.78f}, {2.10f, 1.10f, 0.18f, 0.60f, 0.76f}, {2.45f, 1.04f, 0.24f, 0.56f, 0.70f}};
        Col negro = col(16, 16, 20), faro = col(240, 246, 255);
        int sd, k;
        body(m, s, 6, paint, dark);
        cabin(m, -0.95f, 0.65f, 0.68f, 1.06f, 0.72f, 0.50f, 0.70f, 0.40f, paint, glass);
        mesh_quad_flat(m, v3(-0.08f, 0.39f, -2.40f), v3(0.08f, 0.39f, -2.40f), v3(0.08f, 0.705f, -0.40f), v3(-0.08f, 0.705f, -0.40f), accent);   /* nervio central */
        for (k = 0; k < 4; k++) {   /* ventila sobre el motor */
            float z = 0.95f + k * 0.26f;
            mesh_quad_flat(m, v3(-0.55f, 0.79f, z), v3(0.55f, 0.79f, z), v3(0.55f, 0.79f, z + 0.12f), v3(-0.55f, 0.79f, z + 0.12f), negro);
        }
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.105f;
            mesh_quad_flat(m, v3(x * 1.003f, 0.30f, 0.20f), v3(x * 1.003f, 0.36f, 1.30f), v3(x * 1.003f, 0.62f, 1.10f), v3(x * 1.003f, 0.44f, 0.20f), negro);   /* toma lateral */
            mesh_quad_flat(m, v3(x * 1.003f, 0.18f, -2.00f), v3(x * 1.003f, 0.18f, 2.30f), v3(x * 1.003f, 0.26f, 2.30f), v3(x * 1.003f, 0.26f, -2.00f), accent);
            mesh_quad_flat(m, v3(sd * 0.36f, 0.30f, -2.405f), v3(sd * 0.90f, 0.28f, -2.20f), v3(sd * 0.90f, 0.34f, -2.18f), v3(sd * 0.36f, 0.36f, -2.39f), faro);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.58f, 2.455f), v3(sd * 0.92f, 0.58f, 2.455f), v3(sd * 0.92f, 0.64f, 2.455f), v3(sd * 0.40f, 0.64f, 2.455f), col(230, 20, 24));
        }
        mesh_box(m, v3(-0.92f, 0.70f, 2.20f), v3(0.92f, 0.78f, 2.45f), accent, accent);   /* aleron integrado */
        mesh_quad_flat(m, v3(-0.92f, 0.16f, 2.455f), v3(0.92f, 0.16f, 2.455f), v3(0.92f, 0.44f, 2.455f), v3(-0.92f, 0.44f, 2.455f), negro);
        mesh_box(m, v3(-0.16f, 0.24f, 2.42f), v3(0.16f, 0.36f, 2.52f), col(190, 192, 200), negro);   /* escape central */
        wheel(m, -0.96f, -1.50f, 0.35f, 0.32f); wheel(m, 0.96f, -1.50f, 0.35f, 0.32f);
        wheel(m, -0.98f, 1.50f, 0.37f, 0.42f); wheel(m, 0.98f, 1.50f, 0.37f, 0.42f);
        break;
    }
    case 39: { /* ESTELA V8: deportivo americano, cofre largo con nervio, faros escondidos, cola fastback cortada con 4 pilotos */
        Sec s[] = {{-2.45f, 0.92f, 0.22f, 0.44f, 0.54f}, {-1.70f, 1.00f, 0.20f, 0.56f, 0.70f}, {-0.30f, 1.02f, 0.20f, 0.60f, 0.78f},
                   {1.20f, 1.04f, 0.22f, 0.62f, 0.84f}, {2.25f, 1.00f, 0.28f, 0.62f, 0.84f}};
        Col negro = col(18, 18, 22), rojo = col(220, 24, 26);
        int sd, k;
        body(m, s, 5, paint, dark);
        cabin(m, -0.30f, 1.40f, 0.82f, 1.20f, 0.82f, 0.60f, 0.75f, 0.70f, paint, glass);
        mesh_quad(m, v3(-0.60f, 1.20f, 0.70f), v3(0.60f, 1.20f, 0.70f), v3(0.78f, 0.84f, 2.10f), v3(-0.78f, 0.84f, 2.10f), glass);   /* medallon fastback */
        mesh_quad_flat(m, v3(-0.10f, 0.555f, -2.45f), v3(0.10f, 0.555f, -2.45f), v3(0.10f, 0.79f, -0.30f), v3(-0.10f, 0.79f, -0.30f), col_shade(paint, 1.12f));   /* nervio del cofre */
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.025f;
            mesh_quad_flat(m, v3(sd * 0.50f, 0.50f, -2.455f), v3(sd * 0.86f, 0.48f, -2.38f), v3(sd * 0.86f, 0.52f, -2.36f), v3(sd * 0.50f, 0.54f, -2.44f), negro);   /* faros escondidos (cerrados) */
            mesh_quad_flat(m, v3(x * 1.003f, 0.40f, -1.25f), v3(x * 1.003f, 0.40f, -0.85f), v3(x * 1.003f, 0.62f, -0.95f), v3(x * 1.003f, 0.60f, -1.25f), negro);   /* branquia */
            for (k = 0; k < 2; k++) {   /* dos pilotos redondos por lado */
                float xc = sd * (0.40f + k * 0.34f);
                mesh_box(m, v3(xc - 0.11f, 0.62f, 2.24f), v3(xc + 0.11f, 0.76f, 2.26f), rojo, rojo);
            }
            mesh_box(m, v3(sd * 0.14f - 0.06f, 0.24f, 2.20f), v3(sd * 0.14f + 0.06f, 0.34f, 2.36f), col(190, 192, 200), negro);   /* escapes al centro */
        }
        mesh_quad_flat(m, v3(-0.50f, 0.24f, -2.455f), v3(0.50f, 0.24f, -2.455f), v3(0.46f, 0.40f, -2.455f), v3(-0.46f, 0.40f, -2.455f), negro);
        mesh_box(m, v3(-0.70f, 0.84f, 2.10f), v3(0.70f, 0.90f, 2.26f), paint, paint);
        wheel(m, -0.92f, -1.60f, 0.36f, 0.30f); wheel(m, 0.92f, -1.60f, 0.36f, 0.30f);
        wheel(m, -0.94f, 1.45f, 0.37f, 0.36f); wheel(m, 0.94f, 1.45f, 0.37f, 0.36f);
        break;
    }
    case 40: { /* TURISMO 96: sedan de turismo de los 90, rebajado, labio, alerón sobre la cajuela y pintura de dos tonos */
        Sec s[] = {{-2.30f, 0.88f, 0.20f, 0.52f, 0.66f}, {-1.45f, 0.92f, 0.18f, 0.60f, 0.78f}, {0.30f, 0.93f, 0.18f, 0.62f, 0.82f},
                   {1.55f, 0.93f, 0.20f, 0.62f, 0.86f}, {2.25f, 0.90f, 0.24f, 0.62f, 0.84f}};
        Col negro = col(18, 18, 22), faro = col(244, 246, 240), rojo = col(214, 24, 28);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, -0.85f, 1.35f, 0.82f, 1.30f, 0.84f, 0.70f, 0.60f, 0.50f, paint, glass);
        stripe(m, s, 5, 0.30f, accent);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.935f;
            mesh_quad_flat(m, v3(x * 1.004f, 0.22f, -2.20f), v3(x * 1.004f, 0.22f, 2.20f), v3(x * 1.004f, 0.48f, 2.20f), v3(x * 1.004f, 0.48f, -2.20f), accent);   /* mitad baja de acento */
            mesh_quad_flat(m, v3(x * 1.005f, 0.52f, -0.40f), v3(x * 1.005f, 0.52f, 0.30f), v3(x * 1.005f, 0.78f, 0.30f), v3(x * 1.005f, 0.78f, -0.40f), col(250, 250, 250));   /* panel blanco de numero */
            mesh_quad_flat(m, v3(sd * 0.34f, 0.50f, -2.305f), v3(sd * 0.80f, 0.50f, -2.305f), v3(sd * 0.80f, 0.60f, -2.305f), v3(sd * 0.34f, 0.60f, -2.305f), faro);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.60f, 2.255f), v3(sd * 0.86f, 0.60f, 2.255f), v3(sd * 0.86f, 0.72f, 2.255f), v3(sd * 0.40f, 0.72f, 2.255f), rojo);
            mesh_box(m, v3(sd * 0.62f - 0.03f, 0.86f, 1.92f), v3(sd * 0.62f + 0.03f, 1.02f, 2.12f), negro, negro);
        }
        mesh_box(m, v3(-0.86f, 1.02f, 1.88f), v3(0.86f, 1.07f, 2.18f), accent, accent);   /* aleron */
        mesh_box(m, v3(-1.00f, 0.10f, -2.45f), v3(1.00f, 0.18f, -2.20f), negro, negro);   /* labio */
        mesh_quad_flat(m, v3(-0.34f, 0.28f, -2.305f), v3(0.34f, 0.28f, -2.305f), v3(0.34f, 0.46f, -2.305f), v3(-0.34f, 0.46f, -2.305f), negro);
        wheel(m, -0.86f, -1.45f, 0.34f, 0.28f); wheel(m, 0.86f, -1.45f, 0.34f, 0.28f);
        wheel(m, -0.86f, 1.45f, 0.34f, 0.28f); wheel(m, 0.86f, 1.45f, 0.34f, 0.28f);
        break;
    }
    case 41: { /* ZAFIRO LM: superdeportivo de carreras de motor central, redondeado, toma en el techo y alerón alto */
        Sec s[] = {{-2.25f, 0.92f, 0.16f, 0.32f, 0.42f}, {-1.50f, 1.02f, 0.14f, 0.48f, 0.62f}, {-0.20f, 1.04f, 0.14f, 0.56f, 0.72f},
                   {1.10f, 1.06f, 0.15f, 0.60f, 0.78f}, {2.20f, 1.02f, 0.20f, 0.58f, 0.74f}};
        Col negro = col(16, 16, 20), faro = col(250, 246, 220);
        int sd;
        body(m, s, 5, paint, dark);
        cabin(m, -0.80f, 0.90f, 0.72f, 1.14f, 0.76f, 0.56f, 0.75f, 0.55f, paint, glass);
        mesh_box(m, v3(-0.14f, 1.14f, -0.05f), v3(0.14f, 1.28f, 0.70f), paint, paint);   /* toma del techo */
        mesh_quad_flat(m, v3(-0.10f, 1.16f, -0.055f), v3(0.10f, 1.16f, -0.055f), v3(0.10f, 1.26f, -0.055f), v3(-0.10f, 1.26f, -0.055f), negro);
        mesh_quad_flat(m, v3(-0.12f, 1.285f, -0.05f), v3(0.12f, 1.285f, -0.05f), v3(0.12f, 1.285f, 0.70f), v3(-0.12f, 1.285f, 0.70f), accent);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 1.045f;
            mesh_quad_flat(m, v3(x * 1.003f, 0.34f, 0.30f), v3(x * 1.003f, 0.40f, 1.20f), v3(x * 1.003f, 0.64f, 1.05f), v3(x * 1.003f, 0.52f, 0.30f), negro);   /* toma lateral grande */
            mesh_quad_flat(m, v3(x * 1.003f, 0.18f, -1.90f), v3(x * 1.003f, 0.18f, 2.10f), v3(x * 1.003f, 0.26f, 2.10f), v3(x * 1.003f, 0.26f, -1.90f), accent);
            mesh_box(m, v3(sd * 0.60f - 0.10f, 0.32f, -2.20f), v3(sd * 0.60f + 0.10f, 0.42f, -2.10f), faro, faro);   /* faros en cubierta */
            mesh_box(m, v3(sd * 0.62f - 0.12f, 0.56f, 2.20f), v3(sd * 0.62f + 0.12f, 0.66f, 2.22f), col(220, 24, 24), col(220, 24, 24));
            mesh_box(m, v3(sd * 0.70f - 0.04f, 0.74f, 1.85f), v3(sd * 0.70f + 0.04f, 1.10f, 2.10f), negro, negro);
            mesh_box(m, v3(sd * 1.00f - 0.02f, 0.96f, 1.80f), v3(sd * 1.00f + 0.02f, 1.20f, 2.30f), paint, paint);
        }
        mesh_box(m, v3(-1.00f, 1.10f, 1.80f), v3(1.00f, 1.17f, 2.30f), accent, accent);   /* aleron alto */
        mesh_box(m, v3(-1.02f, 0.08f, -2.40f), v3(1.02f, 0.14f, -2.10f), negro, negro);
        mesh_quad_flat(m, v3(-0.80f, 0.16f, 2.205f), v3(0.80f, 0.16f, 2.205f), v3(0.80f, 0.40f, 2.205f), v3(-0.80f, 0.40f, 2.205f), negro);
        wheel(m, -0.92f, -1.40f, 0.34f, 0.30f); wheel(m, 0.92f, -1.40f, 0.34f, 0.30f);
        wheel(m, -0.94f, 1.40f, 0.36f, 0.38f); wheel(m, 0.94f, 1.40f, 0.36f, 0.38f);
        break;
    }
    case 42: { /* FAMILIAR 95: minivan de los 90, frente corto inclinado, cuerpo alto redondeado, puerta corrediza y moldura */
        Sec s[] = {{-2.35f, 0.88f, 0.28f, 0.58f, 0.76f}, {-1.70f, 0.94f, 0.26f, 0.66f, 0.92f}, {2.20f, 0.95f, 0.28f, 0.68f, 0.98f}, {2.40f, 0.93f, 0.32f, 0.68f, 0.96f}};
        Col negro = col(36, 36, 40), faro = col(240, 242, 236), rojo = col(206, 30, 30);
        int sd;
        body(m, s, 4, paint, dark);
        cabin(m, -1.70f, 2.35f, 0.92f, 1.78f, 0.92f, 0.84f, 0.95f, 0.10f, paint, glass);
        for (sd = -1; sd <= 1; sd += 2) {
            float x = sd * 0.955f;
            mesh_quad_flat(m, v3(x * 1.004f, 0.44f, -2.10f), v3(x * 1.004f, 0.44f, 2.30f), v3(x * 1.004f, 0.54f, 2.30f), v3(x * 1.004f, 0.54f, -2.10f), accent);   /* moldura */
            mesh_quad_flat(m, v3(x * 1.003f, 0.32f, -0.15f), v3(x * 1.003f, 0.32f, -0.13f), v3(x * 1.003f, 0.92f, -0.13f), v3(x * 1.003f, 0.92f, -0.15f), dark);   /* puerta corrediza */
            mesh_quad_flat(m, v3(x * 0.95f, 0.94f, -0.20f), v3(x * 0.95f, 0.94f, -0.08f), v3(x * 0.88f, 1.76f, -0.08f), v3(x * 0.88f, 1.76f, -0.20f), paint);
            mesh_quad_flat(m, v3(x * 0.95f, 0.94f, 1.10f), v3(x * 0.95f, 0.94f, 1.22f), v3(x * 0.88f, 1.76f, 1.22f), v3(x * 0.88f, 1.76f, 1.10f), paint);
            mesh_quad_flat(m, v3(sd * 0.40f, 0.60f, -2.355f), v3(sd * 0.84f, 0.60f, -2.355f), v3(sd * 0.84f, 0.72f, -2.355f), v3(sd * 0.40f, 0.72f, -2.355f), faro);
            mesh_quad_flat(m, v3(sd * 0.70f, 0.60f, 2.405f), v3(sd * 0.90f, 0.60f, 2.405f), v3(sd * 0.90f, 1.20f, 2.405f), v3(sd * 0.70f, 1.20f, 2.405f), rojo);   /* calaveras verticales */
        }
        mesh_quad_flat(m, v3(-0.40f, 0.60f, -2.355f), v3(0.40f, 0.60f, -2.355f), v3(0.40f, 0.72f, -2.355f), v3(-0.40f, 0.72f, -2.355f), negro);
        mesh_box(m, v3(-0.94f, 0.24f, -2.45f), v3(0.94f, 0.46f, -2.28f), accent, accent);   /* defensas */
        mesh_box(m, v3(-0.94f, 0.26f, 2.32f), v3(0.94f, 0.48f, 2.50f), accent, accent);
        mesh_box(m, v3(-0.62f, 1.78f, -0.60f), v3(-0.56f, 1.84f, 1.80f), negro, negro);   /* rieles del techo */
        mesh_box(m, v3(0.56f, 1.78f, -0.60f), v3(0.62f, 1.84f, 1.80f), negro, negro);
        wheel(m, -0.86f, -1.55f, 0.33f, 0.24f); wheel(m, 0.86f, -1.55f, 0.33f, 0.24f);
        wheel(m, -0.86f, 1.60f, 0.33f, 0.24f); wheel(m, 0.86f, 1.60f, 0.33f, 0.24f);
        break;
    }
    case 4: { /* sedan deportivo de 4 puertas (dibujo del hermano) */
        Sec s[] = {{-2.48f, 0.88f, 0.26f, 0.52f, 0.66f}, {-1.70f, 0.95f, 0.24f, 0.64f, 0.82f},
                   {0.30f, 0.96f, 0.24f, 0.70f, 0.88f}, {1.85f, 0.95f, 0.26f, 0.72f, 0.94f},
                   {2.45f, 0.90f, 0.34f, 0.72f, 0.92f}};
        Col chrome = col(205, 208, 216), grille = col(28, 28, 32);
        int side;
        body(m, s, 5, paint, dark);
        /* habitaculo largo de 4 puertas con luneta inclinada */
        cabin(m, -0.70f, 1.75f, 0.88f, 1.44f, 0.84f, 0.68f, 0.78f, 0.62f, paint, glass);
        /* parrilla ancha y faros finos */
        mesh_quad_flat(m, v3(-0.62f, 0.34f, -2.49f), v3(0.62f, 0.34f, -2.49f), v3(0.56f, 0.60f, -2.49f), v3(-0.56f, 0.60f, -2.49f), grille);
        mesh_quad_flat(m, v3(-0.60f, 0.46f, -2.50f), v3(0.60f, 0.46f, -2.50f), v3(0.60f, 0.50f, -2.50f), v3(-0.60f, 0.50f, -2.50f), chrome);
        mesh_quad_flat(m, v3(-0.80f, 0.56f, -2.47f), v3(-0.46f, 0.56f, -2.47f), v3(-0.50f, 0.64f, -2.42f), v3(-0.82f, 0.62f, -2.40f), col(255, 246, 210));
        mesh_quad_flat(m, v3(0.46f, 0.56f, -2.47f), v3(0.80f, 0.56f, -2.47f), v3(0.82f, 0.62f, -2.40f), v3(0.50f, 0.64f, -2.42f), col(255, 246, 210));
        /* tomas de aire bajas en la defensa */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.78f, 0.28f, -2.49f), v3(side * 0.48f, 0.28f, -2.49f), v3(side * 0.50f, 0.38f, -2.49f), v3(side * 0.80f, 0.38f, -2.49f), grille);
        /* lineas de puertas, manijas y molduras cromadas */
        for (side = -1; side <= 1; side += 2) {
            float x = side * 0.965f;
            float zs[3] = {-0.62f, 0.52f, 1.62f};
            int k;
            for (k = 0; k < 3; k++)
                mesh_quad_flat(m, v3(x, 0.32f, zs[k] - 0.015f), v3(x, 0.32f, zs[k] + 0.015f), v3(x, 0.86f, zs[k] + 0.015f), v3(x, 0.86f, zs[k] - 0.015f), dark);
            mesh_quad_flat(m, v3(x * 1.002f, 0.70f, -0.25f), v3(x * 1.002f, 0.70f, -0.08f), v3(x * 1.002f, 0.74f, -0.08f), v3(x * 1.002f, 0.74f, -0.25f), chrome);
            mesh_quad_flat(m, v3(x * 1.002f, 0.70f, 0.90f), v3(x * 1.002f, 0.70f, 1.07f), v3(x * 1.002f, 0.74f, 1.07f), v3(x * 1.002f, 0.74f, 0.90f), chrome);
            mesh_quad_flat(m, v3(x * 1.003f, 0.30f, -1.55f), v3(x * 1.003f, 0.30f, 1.75f), v3(x * 1.003f, 0.34f, 1.75f), v3(x * 1.003f, 0.34f, -1.55f), chrome);
        }
        /* pequeno aleron en la tapa del maletero */
        mesh_box(m, v3(-0.78f, 0.92f, 2.22f), v3(0.78f, 0.98f, 2.40f), paint, col_shade(paint, 1.15f));
        wheel(m, -0.88f, -1.52f, 0.35f, 0.28f); wheel(m, 0.88f, -1.52f, 0.35f, 0.28f);
        wheel(m, -0.88f, 1.55f, 0.35f, 0.28f); wheel(m, 0.88f, 1.55f, 0.35f, 0.28f);
        lights(m, -2.60f, 2.45f, 0.30f, 0.74f, 0.66f);
        break;
    }
    case 5: { /* cuna de los 80 (dibujo del jugador) */
        Sec s[] = {{-2.30f, 0.86f, 0.24f, 0.40f, 0.46f}, {-1.45f, 0.96f, 0.22f, 0.52f, 0.66f},
                   {0.10f, 0.99f, 0.22f, 0.62f, 0.82f}, {1.55f, 0.99f, 0.24f, 0.66f, 0.90f},
                   {2.25f, 0.96f, 0.30f, 0.66f, 0.88f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 5, paint, dark);
        /* cabina baja y adelantada, con luneta casi plana hacia la cola */
        cabin(m, -0.60f, 1.05f, 0.80f, 1.16f, 0.82f, 0.60f, 0.80f, 0.15f, paint, glass);
        mesh_quad(m, v3(-0.60f, 1.16f, 0.90f), v3(0.60f, 1.16f, 0.90f), v3(0.80f, 0.90f, 2.05f), v3(-0.80f, 0.90f, 2.05f), glass);
        /* persianas sobre la luneta trasera */
        for (k = 0; k < 6; k++) {
            float t0 = 0.08f + k * 0.15f, t1 = t0 + 0.06f;
            V3 a0 = vlerp(v3(-0.66f, 1.165f, 0.90f), v3(-0.82f, 0.905f, 2.05f), t0), b0 = vlerp(v3(0.66f, 1.165f, 0.90f), v3(0.82f, 0.905f, 2.05f), t0);
            V3 a1 = vlerp(v3(-0.66f, 1.165f, 0.90f), v3(-0.82f, 0.905f, 2.05f), t1), b1 = vlerp(v3(0.66f, 1.165f, 0.90f), v3(0.82f, 0.905f, 2.05f), t1);
            a0.y += 0.02f; b0.y += 0.02f; a1.y += 0.02f; b1.y += 0.02f;
            mesh_quad_flat(m, a0, b0, b1, a1, black);
        }
        /* faros escamoteables (cerrados) sobre el frente */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.30f, 0.555f, -1.95f), v3(side * 0.72f, 0.555f, -1.95f), v3(side * 0.72f, 0.60f, -1.65f), v3(side * 0.30f, 0.60f, -1.65f), col_shade(paint, 0.75f));
        /* franja blanca lateral y tomas de aire (branquias) detras de las puertas */
        for (side = -1; side <= 1; side += 2) {
            float x = side * 1.0f;
            mesh_quad_flat(m, v3(x * 1.004f, 0.40f, -2.1f), v3(x * 1.004f, 0.40f, 2.1f), v3(x * 1.004f, 0.46f, 2.1f), v3(x * 1.004f, 0.46f, -2.1f), accent);
            for (k = 0; k < 4; k++) {
                float y = 0.50f + k * 0.07f;
                mesh_quad_flat(m, v3(x * 1.005f, y, 0.85f), v3(x * 1.005f, y, 1.45f), v3(x * 1.005f, y + 0.035f, 1.45f), v3(x * 1.005f, y + 0.035f, 0.85f), black);
            }
            mesh_quad_flat(m, v3(x * 1.004f, 0.28f, -0.60f), v3(x * 1.004f, 0.28f, 0.80f), v3(x * 1.004f, 0.30f, 0.80f), v3(x * 1.004f, 0.30f, -0.60f), black);
        }
        /* trasera: banda de pilotos a lo ancho y panel negro */
        mesh_quad_flat(m, v3(-0.92f, 0.46f, 2.26f), v3(0.92f, 0.46f, 2.26f), v3(0.92f, 0.62f, 2.26f), v3(-0.92f, 0.62f, 2.26f), col(200, 20, 20));
        mesh_quad_flat(m, v3(-0.70f, 0.32f, 2.27f), v3(0.70f, 0.32f, 2.27f), v3(0.70f, 0.44f, 2.27f), v3(-0.70f, 0.44f, 2.27f), black);
        /* faros auxiliares en el paragolpes delantero */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.42f, 0.30f, -2.31f), v3(side * 0.78f, 0.30f, -2.31f), v3(side * 0.78f, 0.38f, -2.31f), v3(side * 0.42f, 0.38f, -2.31f), col(255, 246, 210));
        wheel(m, -0.88f, -1.40f, 0.34f, 0.28f); wheel(m, 0.88f, -1.40f, 0.34f, 0.28f);
        wheel(m, -0.90f, 1.40f, 0.36f, 0.32f); wheel(m, 0.90f, 1.40f, 0.36f, 0.32f);
        break;
    }
    case 6: { /* sedan cuadrado de los 80 (dibujo del jugador) */
        Sec s[] = {{-2.32f, 0.88f, 0.28f, 0.66f, 0.76f}, {-1.75f, 0.92f, 0.26f, 0.72f, 0.84f},
                   {0.30f, 0.93f, 0.26f, 0.74f, 0.86f}, {1.70f, 0.93f, 0.27f, 0.75f, 0.90f},
                   {2.30f, 0.90f, 0.30f, 0.75f, 0.90f}};
        Col black = col(30, 30, 34), chrome = col(196, 198, 206);
        int side, k;
        body(m, s, 5, paint, dark);
        /* habitaculo alto y recto, pilares finos */
        cabin(m, -0.85f, 1.45f, 0.86f, 1.48f, 0.86f, 0.80f, 0.55f, 0.40f, paint, glass);
        for (side = -1; side <= 1; side += 2) {
            float x = side * 0.865f;
            /* pilar entre la puerta trasera y la ventanilla de cuarto */
            mesh_quad_flat(m, v3(x, 0.88f, 1.05f), v3(x, 0.88f, 1.15f), v3(x * 0.95f, 1.46f, 1.02f), v3(x * 0.95f, 1.46f, 0.92f), paint);
            /* moldura lateral y lineas de puertas */
            mesh_quad_flat(m, v3(side * 0.935f, 0.52f, -2.2f), v3(side * 0.935f, 0.52f, 2.2f), v3(side * 0.935f, 0.58f, 2.2f), v3(side * 0.935f, 0.58f, -2.2f), black);
            for (k = 0; k < 3; k++) {
                float z = -0.85f + k * 1.15f;
                mesh_quad_flat(m, v3(side * 0.936f, 0.30f, z - 0.012f), v3(side * 0.936f, 0.30f, z + 0.012f), v3(side * 0.936f, 0.86f, z + 0.012f), v3(side * 0.936f, 0.86f, z - 0.012f), black);
            }
            mesh_quad_flat(m, v3(side * 0.937f, 0.70f, -0.62f), v3(side * 0.937f, 0.70f, -0.45f), v3(side * 0.937f, 0.74f, -0.45f), v3(side * 0.937f, 0.74f, -0.62f), chrome);
            mesh_quad_flat(m, v3(side * 0.937f, 0.70f, 0.52f), v3(side * 0.937f, 0.70f, 0.69f), v3(side * 0.937f, 0.74f, 0.69f), v3(side * 0.937f, 0.74f, 0.52f), chrome);
        }
        /* frente: faros rectangulares y parrilla de barras horizontales */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.42f, 0.48f, -2.33f), v3(side * 0.80f, 0.48f, -2.33f), v3(side * 0.80f, 0.66f, -2.33f), v3(side * 0.42f, 0.66f, -2.33f), col(255, 246, 214));
        mesh_quad_flat(m, v3(-0.40f, 0.48f, -2.33f), v3(0.40f, 0.48f, -2.33f), v3(0.40f, 0.66f, -2.33f), v3(-0.40f, 0.66f, -2.33f), black);
        for (k = 0; k < 3; k++) {
            float y = 0.51f + k * 0.05f;
            mesh_quad_flat(m, v3(-0.38f, y, -2.34f), v3(0.38f, y, -2.34f), v3(0.38f, y + 0.02f, -2.34f), v3(-0.38f, y + 0.02f, -2.34f), chrome);
        }
        /* defensas anchas oscuras */
        mesh_box(m, v3(-0.92f, 0.26f, -2.45f), v3(0.92f, 0.42f, -2.28f), black, black);
        mesh_box(m, v3(-0.92f, 0.28f, 2.26f), v3(0.92f, 0.44f, 2.42f), black, black);
        /* pilotos traseros grandes */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.30f, 0.50f, 2.31f), v3(side * 0.88f, 0.50f, 2.31f), v3(side * 0.88f, 0.72f, 2.31f), v3(side * 0.30f, 0.72f, 2.31f), col(200, 24, 24));
        mesh_quad_flat(m, v3(-0.28f, 0.52f, 2.31f), v3(0.28f, 0.52f, 2.31f), v3(0.28f, 0.70f, 2.31f), v3(-0.28f, 0.70f, 2.31f), black);
        wheel(m, -0.86f, -1.42f, 0.32f, 0.24f); wheel(m, 0.86f, -1.42f, 0.32f, 0.24f);
        wheel(m, -0.86f, 1.42f, 0.32f, 0.24f); wheel(m, 0.86f, 1.42f, 0.32f, 0.24f);
        break;
    }
    case 7: { /* sedan V10 (dibujo del jugador) */
        Sec s[] = {{-2.36f, 0.90f, 0.20f, 0.52f, 0.64f}, {-1.60f, 0.97f, 0.20f, 0.64f, 0.82f},
                   {0.30f, 0.98f, 0.20f, 0.70f, 0.88f}, {1.75f, 0.97f, 0.22f, 0.74f, 0.98f},
                   {2.30f, 0.92f, 0.30f, 0.74f, 0.98f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 5, paint, dark);
        /* techo que cae suave hasta el maletero alto */
        cabin(m, -0.75f, 1.80f, 0.86f, 1.40f, 0.84f, 0.66f, 0.85f, 0.85f, paint, glass);
        /* retrovisores */
        for (side = -1; side <= 1; side += 2)
            mesh_box(m, v3(side * 0.86f - (side > 0 ? 0 : 0.22f), 0.92f, -0.62f), v3(side * 0.86f + (side > 0 ? 0.22f : 0), 1.04f, -0.48f), paint, paint);
        /* frente de competicion: parrilla baja ancha, grandes tomas y labio */
        mesh_quad_flat(m, v3(-0.70f, 0.24f, -2.37f), v3(0.70f, 0.24f, -2.37f), v3(0.62f, 0.44f, -2.37f), v3(-0.62f, 0.44f, -2.37f), black);
        for (side = -1; side <= 1; side += 2) {
            mesh_quad_flat(m, v3(side * 0.40f, 0.50f, -2.36f), v3(side * 0.82f, 0.50f, -2.36f), v3(side * 0.84f, 0.58f, -2.30f), v3(side * 0.42f, 0.60f, -2.30f), col(255, 246, 214));
            mesh_quad_flat(m, v3(side * 0.74f, 0.22f, -2.37f), v3(side * 0.90f, 0.22f, -2.37f), v3(side * 0.90f, 0.40f, -2.37f), v3(side * 0.76f, 0.40f, -2.37f), black);
        }
        mesh_box(m, v3(-0.96f, 0.10f, -2.52f), v3(0.96f, 0.16f, -2.26f), black, black);
        /* faldones laterales y franja */
        for (side = -1; side <= 1; side += 2) {
            mesh_box(m, v3(side > 0 ? 0.90f : -1.02f, 0.12f, -1.1f), v3(side > 0 ? 1.02f : -0.90f, 0.24f, 1.1f), black, black);
            mesh_quad_flat(m, v3(side * 1.003f, 0.56f, -2.1f), v3(side * 1.003f, 0.56f, 2.1f), v3(side * 1.003f, 0.60f, 2.1f), v3(side * 1.003f, 0.60f, -2.1f), accent);
            /* lineas de las 4 puertas y manijas */
            for (k = 0; k < 3; k++) {
                float z = -0.72f + k * 1.18f;
                mesh_quad_flat(m, v3(side * 1.004f, 0.26f, z - 0.012f), v3(side * 1.004f, 0.26f, z + 0.012f), v3(side * 1.004f, 0.86f, z + 0.012f), v3(side * 1.004f, 0.86f, z - 0.012f), black);
            }
            mesh_quad_flat(m, v3(side * 1.005f, 0.72f, -0.50f), v3(side * 1.005f, 0.72f, -0.33f), v3(side * 1.005f, 0.76f, -0.33f), v3(side * 1.005f, 0.76f, -0.50f), accent);
            mesh_quad_flat(m, v3(side * 1.005f, 0.72f, 0.68f), v3(side * 1.005f, 0.72f, 0.85f), v3(side * 1.005f, 0.76f, 0.85f), v3(side * 1.005f, 0.76f, 0.68f), accent);
            /* toma de aire del V10 detras de las puertas traseras */
            mesh_quad_flat(m, v3(side * 1.004f, 0.32f, 1.45f), v3(side * 1.004f, 0.32f, 1.85f), v3(side * 1.004f, 0.50f, 1.85f), v3(side * 1.004f, 0.50f, 1.45f), black);
        }
        /* trasera: pilotos, difusor y cuatro escapes */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.40f, 0.66f, 2.31f), v3(side * 0.88f, 0.66f, 2.31f), v3(side * 0.88f, 0.80f, 2.31f), v3(side * 0.40f, 0.80f, 2.31f), col(210, 24, 24));
        mesh_box(m, v3(-0.86f, 0.12f, 2.20f), v3(0.86f, 0.30f, 2.42f), black, black);
        for (k = 0; k < 4; k++) {
            float x = -0.54f + k * 0.36f;
            mesh_prism(m, v3(x, 0.18f, 2.43f), 0.06f, 0.01f, 6, col(160, 160, 166), col(40, 40, 44));
            mesh_quad_flat(m, v3(x - 0.06f, 0.16f, 2.43f), v3(x + 0.06f, 0.16f, 2.43f), v3(x + 0.06f, 0.26f, 2.43f), v3(x - 0.06f, 0.26f, 2.43f), col(60, 60, 64));
        }
        /* pequeno aleron en el borde del maletero */
        mesh_box(m, v3(-0.82f, 0.98f, 2.12f), v3(0.82f, 1.06f, 2.32f), paint, accent);
        wheel(m, -0.90f, -1.48f, 0.36f, 0.30f); wheel(m, 0.90f, -1.48f, 0.36f, 0.30f);
        wheel(m, -0.92f, 1.50f, 0.37f, 0.34f); wheel(m, 0.92f, 1.50f, 0.37f, 0.34f);
        break;
    }
    default: { /* gran turismo largo */
        Sec s[] = {{-2.45f, 0.86f, 0.22f, 0.44f, 0.56f}, {-1.30f, 0.97f, 0.20f, 0.58f, 0.80f},
                   {0.60f, 0.99f, 0.20f, 0.62f, 0.86f}, {1.85f, 0.98f, 0.22f, 0.64f, 0.90f},
                   {2.42f, 0.90f, 0.30f, 0.62f, 0.86f}};
        body(m, s, 5, paint, dark);
        cabin(m, 0.05f, 1.70f, 0.84f, 1.24f, 0.80f, 0.60f, 0.70f, 0.70f, paint, glass);
        /* franjas laterales */
        {
            int side;
            for (side = -1; side <= 1; side += 2) {
                float x = side * 1.003f;
                mesh_quad_flat(m, v3(x, 0.50f, -1.3f), v3(x, 0.50f, 1.85f), v3(x, 0.58f, 1.85f), v3(x, 0.58f, -1.3f), accent);
            }
        }
        wheel(m, -0.86f, -1.45f, 0.35f, 0.28f); wheel(m, 0.86f, -1.45f, 0.35f, 0.28f);
        wheel(m, -0.88f, 1.45f, 0.36f, 0.32f); wheel(m, 0.88f, 1.45f, 0.36f, 0.32f);
        lights(m, -2.45f, 2.42f, 0.36f, 0.72f, 0.58f);
        break;
    }
    }
    /* sombra bajo el auto */
    mesh_quad_flat(m, v3(-1.05f, 0.03f, -2.3f), v3(1.05f, 0.03f, -2.3f), v3(1.05f, 0.03f, 2.3f), v3(-1.05f, 0.03f, 2.3f), col(30, 30, 30));
}

void traffic_build_mesh(Mesh *m, int kind, Col paint) {
    Col dark = col(34, 34, 38), glass = col(40, 56, 80);
    mesh_init(m);
    g_wheel_n = 0;
    if (kind == 0) { /* sedan */
        mesh_box(m, v3(-0.9f, 0.25f, -2.2f), v3(0.9f, 0.85f, 2.2f), paint, paint);
        cabin(m, -0.9f, 1.2f, 0.85f, 1.42f, 0.84f, 0.74f, 0.45f, 0.35f, paint, glass);
        wheel(m, -0.86f, -1.35f, 0.32f, 0.24f); wheel(m, 0.86f, -1.35f, 0.32f, 0.24f);
        wheel(m, -0.86f, 1.35f, 0.32f, 0.24f); wheel(m, 0.86f, 1.35f, 0.32f, 0.24f);
        lights(m, -2.2f, 2.2f, 0.55f, 0.78f, 0.6f);
    } else { /* furgoneta */
        mesh_box(m, v3(-1.0f, 0.3f, -2.6f), v3(1.0f, 2.3f, 2.6f), paint, col(220, 220, 225));
        mesh_quad_flat(m, v3(-0.9f, 1.3f, -2.61f), v3(0.9f, 1.3f, -2.61f), v3(0.9f, 2.1f, -2.61f), v3(-0.9f, 2.1f, -2.61f), glass);
        mesh_box(m, v3(-1.0f, 0.3f, -2.62f), v3(1.0f, 0.55f, -2.55f), dark, dark);
        wheel(m, -0.95f, -1.7f, 0.36f, 0.26f); wheel(m, 0.95f, -1.7f, 0.36f, 0.26f);
        wheel(m, -0.95f, 1.7f, 0.36f, 0.26f); wheel(m, 0.95f, 1.7f, 0.36f, 0.26f);
        lights(m, -2.6f, 2.6f, 0.65f, 0.85f, 0.7f);
    }
    mesh_quad_flat(m, v3(-1.05f, 0.03f, -2.6f), v3(1.05f, 0.03f, -2.6f), v3(1.05f, 0.03f, 2.6f), v3(-1.05f, 0.03f, 2.6f), col(30, 30, 30));
}

/* camioneta de policia: carroceria alta y cuadrada en negro, puertas blancas, defensa de empuje grande y torreta */
void suv_build_mesh(Mesh *m) {
    Col paint = col(22, 22, 28), white = col(242, 242, 246), dark = col(30, 30, 34), glass = col(36, 52, 80), black = col(12, 12, 14);
    Sec s[] = {{-2.45f, 0.98f, 0.36f, 0.84f, 1.04f}, {-1.70f, 1.03f, 0.34f, 0.92f, 1.10f}, {1.00f, 1.04f, 0.34f, 0.94f, 1.12f},
               {2.30f, 1.02f, 0.38f, 0.94f, 1.12f}, {2.45f, 0.98f, 0.42f, 0.92f, 1.08f}};
    int side;
    mesh_init(m);
    g_wheel_n = 0;
    body(m, s, 5, paint, dark);
    cabin(m, -1.05f, 2.25f, 1.10f, 1.88f, 0.94f, 0.86f, 0.50f, 0.08f, paint, glass);
    for (side = -1; side <= 1; side += 2) {
        /* puertas blancas y estribo */
        mesh_box(m, v3(side > 0 ? 1.035f : -1.055f, 0.56f, -0.95f), v3(side > 0 ? 1.055f : -1.035f, 1.08f, 1.20f), white, white);
        mesh_box(m, v3(side > 0 ? 1.00f : -1.12f, 0.36f, -1.10f), v3(side > 0 ? 1.12f : -1.00f, 0.42f, 1.30f), black, black);
        /* pilares de las 4 puertas y ventana trasera lateral */
        mesh_quad_flat(m, v3(side * 0.945f, 1.10f, 0.10f), v3(side * 0.945f, 1.10f, 0.22f), v3(side * 0.865f, 1.88f, 0.22f), v3(side * 0.865f, 1.88f, 0.10f), paint);
        mesh_quad_flat(m, v3(side * 0.945f, 1.10f, 1.30f), v3(side * 0.945f, 1.10f, 1.42f), v3(side * 0.865f, 1.88f, 1.42f), v3(side * 0.865f, 1.88f, 1.30f), paint);
    }
    /* defensa de empuje: marco tubular grande al frente */
    mesh_box(m, v3(-0.95f, 0.30f, -2.70f), v3(0.95f, 0.42f, -2.55f), black, black);
    mesh_box(m, v3(-0.95f, 0.92f, -2.70f), v3(0.95f, 1.02f, -2.55f), black, black);
    mesh_box(m, v3(-0.60f, 0.30f, -2.70f), v3(-0.48f, 1.02f, -2.55f), black, black);
    mesh_box(m, v3(0.48f, 0.30f, -2.70f), v3(0.60f, 1.02f, -2.55f), black, black);
    mesh_box(m, v3(-0.95f, 0.30f, -2.62f), v3(-0.85f, 1.02f, -2.45f), black, black);
    mesh_box(m, v3(0.85f, 0.30f, -2.62f), v3(0.95f, 1.02f, -2.45f), black, black);
    /* base de la torreta y luces fijas (la torreta que destella se dibuja encima) */
    mesh_box(m, v3(-0.70f, 1.86f, -0.30f), v3(0.70f, 1.90f, 0.10f), dark, dark);
    mesh_box(m, v3(-0.64f, 1.90f, -0.24f), v3(-0.04f, 2.03f, 0.04f), col(150, 30, 30), col(150, 30, 30));
    mesh_box(m, v3(0.04f, 1.90f, -0.24f), v3(0.64f, 2.03f, 0.04f), col(30, 60, 160), col(30, 60, 160));
    /* llanta de refaccion atras */
    mesh_box(m, v3(-0.42f, 0.70f, 2.45f), v3(0.42f, 1.42f, 2.62f), black, dark);
    wheel(m, -0.98f, -1.55f, 0.42f, 0.34f); wheel(m, 0.98f, -1.55f, 0.42f, 0.34f);
    wheel(m, -0.98f, 1.50f, 0.42f, 0.34f); wheel(m, 0.98f, 1.50f, 0.42f, 0.34f);
    lights(m, -2.45f, 2.45f, 0.70f, 0.80f, 0.86f);
}

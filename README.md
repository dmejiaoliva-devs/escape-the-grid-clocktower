# Escape the Grid - Clocktower Edition

## Descripción General del Juego

**Escape the Grid** es un puzzle game innovador que combina navegación estratégica en una cuadrícula hexagonal con mecánicas únicas de juego. Los jugadores deben navegar desde un punto de inicio hasta la meta, recolectando gemas y superando obstáculos dinámicos como compuertas temporales y paredes que cambian con el tiempo.

### Características Principales:
- **Grid Hexagonal**: Sistema de navegación único basado en hexágonos con 6 direcciones
- **Mecánicas Temporales**: Compuertas y paredes que cambian según los turnos
- **Sistema de Puntuación**: Recompensas por eficiencia y penalizaciones por backtracking
- **Resolución Automática**: Múltiples algoritmos (A*, BFS, Dijkstra) para resolver puzzles automáticamente
- **Tutorial Interactivo**: Guía visual de todos los elementos del juego
- **Múltiples Niveles**: Desde básico hasta nightmare con complejidad creciente
- **Protecciones Inteligentes**: Algoritmos con timeouts y límites para evitar cuelgues

## Tecnologías Utilizadas

- **Lenguaje**: C++17
- **Biblioteca Gráfica**: Raylib 4.0+
- **Compilador**: GCC/Clang/MSVC compatible
- **Algoritmos**: A* con heurística hexagonal, BFS, Dijkstra optimizado
- **Arquitectura**: Patrón MVC modificado con gestión de estados

## Instrucciones de Compilación

### Requisitos Previos:
1. **Raylib 4.0+**: Instalado en el sistema
2. **Compilador C++**: Compatible con C++17 (GCC, Clang, MSVC)
3. **Sistema Operativo**: Windows, Linux, o macOS

### Instalación de Raylib:

#### En Linux (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install libraylib-dev
```

#### En macOS (con Homebrew):
```bash
brew install raylib
```

#### En Windows:
- Descargar Raylib desde [raylib.com](https://www.raylib.com/)
- Configurar variables de entorno según tu compilador

### Compilación:

#### Linux/macOS:
```bash
# Clonar el repositorio
git clone [URL_DEL_REPOSITORIO]
cd escape-the-grid

# Compilar todos los archivos fuente
g++ -std=c++17 -O2 \
    Game.cpp Grid.cpp HexCell.cpp Player.cpp \
    PathFinder.cpp FileLoader.cpp main.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -o escape_the_grid

# Ejecutar
./escape_the_grid
```

#### Windows (MinGW):
```bash
g++ -std=c++17 -O2 ^
    Game.cpp Grid.cpp HexCell.cpp Player.cpp ^
    PathFinder.cpp FileLoader.cpp main.cpp ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -o escape_the_grid.exe

escape_the_grid.exe
```

#### Makefile (Recomendado):
Crear un `Makefile`:
```makefile
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SOURCES = Game.cpp Grid.cpp HexCell.cpp Player.cpp PathFinder.cpp FileLoader.cpp main.cpp
TARGET = escape_the_grid

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

Luego compilar con:
```bash
Presionando F5
```

## Cómo Jugar

### Objetivo:
Navegar desde el punto de inicio (círculos verdes concéntricos) hasta la meta (círculo rojo con estrella) recolectando gemas y evitando obstáculos.

### Elementos del Juego:

| Elemento | Descripción |
|----------|-------------|
| **Jugador** | Tu posición actual - Círculo naranja con "D" y efecto pulsante |
| **Punto de Inicio** | Donde comienzas - Círculos verdes concéntricos |
| **Meta** | Objetivo a alcanzar - Círculo rojo con estrella de 8 puntas |
| **Gemas** | Recoléctalas para puntos (+100) - Círculo dorado brillante con efecto de brillo |
| **Paredes** | No se pueden atravesar - Patrón de ladrillos rojos 3x3 |
| **Compuertas Abiertas** | Puedes pasar - Barras azules verticales separadas |
| **Compuertas Cerradas** | Bloqueadas temporalmente - Barra horizontal naranja con X roja |
| **Paredes Temporales** | Se abren después de N turnos - Reloj amarillo con contador |

### Mecánicas Especiales:

1. **Sistema de Turnos**: Las compuertas y paredes temporales cambian según el número de turnos
2. **Puntuación Dinámica**: 
   - +100 puntos por gema recolectada
   - -50 puntos por revisitar celdas (backtracking)
   - Puntuación inicial: 1000 puntos
3. **Grid Hexagonal**: Navegación en 6 direcciones desde cada celda
4. **Patrones de Compuertas**: Cada compuerta sigue un patrón binario específico (ej: "1100" = abierta 2 turnos, cerrada 2 turnos)

## Controles

| Acción | Control |
|--------|---------|
| **Mover Jugador** | Click izquierdo en celda hexagonal adyacente |
| **Auto-resolver** | ESPACIO |
| **Reiniciar Nivel** | R |
| **Volver al Menú** | ESC (desde juego terminado) |
| **Continuar Tutorial** | ESPACIO / ENTER / Click |

### Selección de Niveles (desde menú):
- **1**: Nivel Básico (8×6)
- **2**: Nivel Intermedio (10×8)
- **3**: Nivel Experto (12×10)
- **4**: Nivel Nightmare (18×14)
- **T**: Nivel de Debug (para desarrollo)

## Estructura del Proyecto

```
escape-the-grid/
├── src/
│   ├── main.cpp                   # Punto de entrada del programa
│   ├── Game.h / Game.cpp          # Lógica principal y estados del juego
│   ├── Grid.h / Grid.cpp          # Manejo de la cuadrícula hexagonal
│   ├── HexCell.h / HexCell.cpp    # Lógica de celdas individuales
│   ├── Player.h / Player.cpp      # Lógica del jugador y movimiento
│   ├── PathFinder.h / PathFinder.cpp # Algoritmos de pathfinding (A*, BFS, Dijkstra)
│   └── FileLoader.h / FileLoader.cpp # Carga de niveles desde archivos
├── assets/
│   └── levels/
│       ├── level1.txt             # Nivel básico (8×6)
│       ├── level2.txt             # Nivel intermedio (10×8)
│       ├── level_expert.txt       # Nivel experto (12×10)
│       └── level_nightmare.txt    # Nivel nightmare (18×14)
├── Makefile                       # Script de compilación
└── README.md                      # Este archivo
```

### Archivos Principales:

- **main.cpp**: Inicializa el juego y maneja el loop principal
- **Game**: Controlador principal, maneja estados y lógica del juego
- **Grid**: Representa la cuadrícula hexagonal y sus operaciones
- **HexCell**: Celda individual con rendering y lógica específica
- **Player**: Manejo del jugador, movimiento y puntuación
- **PathFinder**: Implementación de algoritmos de búsqueda con optimizaciones
- **FileLoader**: Carga niveles desde archivos de texto con formato específico

## Estados del Juego

El juego maneja los siguientes estados:

1. **MENU**: Pantalla principal con selección de niveles
2. **TUTORIAL**: Explicación de elementos del juego (mostrada antes de cada nivel)
3. **PLAYING**: Jugabilidad normal controlada por el usuario
4. **AUTO_SOLVING**: Resolución automática usando algoritmos optimizados
5. **WIN**: Pantalla de victoria con puntuación final
6. **GAME_OVER**: Pantalla de fin de juego

## Formato de Niveles

Los niveles se definen en archivos `.txt` con el siguiente formato:

### Ejemplo (level1.txt):
```
8 6          # Ancho Alto
0 0          # Posición inicial (x, y)
7 5          # Posición objetivo (x, y)
8            # Ciclo de turnos
S.#.....     # Fila 0 del mapa
##.#.##.     # Fila 1 del mapa
...#.#.#     # Fila 2 del mapa
.###.#.#     # Fila 3 del mapa
.....#..     # Fila 4 del mapa
...#..KG     # Fila 5 del mapa

GATE_A 11001100           # Patrón de compuerta A (8 turnos)
ASSIGN_2_1_A             # Asignar patrón A a celda (2,1)
TEMPORAL_3_2_5           # Pared temporal en (3,2) que se abre en turno 5
```

### Símbolos del Mapa:
- `S`: Punto de inicio
- `G`: Meta
- `#`: Pared permanente
- `.`: Celda libre
- `K`: Gema/Item recolectable
- `O`: Compuerta (se asigna patrón después)
- `X`: Compuerta (se asigna patrón después)
- `T`: Pared temporal (se configura después)

### Configuraciones Especiales:
- **GATE_[nombre] [patrón]**: Define un patrón de apertura/cierre para compuertas
- **ASSIGN_[x]_[y]_[patrón]**: Asigna un patrón a una celda específica
- **TEMPORAL_[x]_[y]_[turnos]**: Pared temporal que se abre después de N turnos

## Algoritmos Implementados

### A* (A-Star) con Optimizaciones
- **Heurística Hexagonal**: Distancia Manhattan adaptada para grids hexagonales
- **Consideración Temporal**: Simula estados futuros para compuertas y paredes temporales
- **Límites de Seguridad**: Timeout de 3 segundos, máximo 5000 iteraciones
- **Gestión de Memoria**: Máximo 2000 nodos para evitar uso excesivo de memoria

### BFS (Breadth-First Search)
- **Garantía de Solución Óptima**: Encuentra el camino con menos movimientos
- **Optimizado**: Timeout de 2 segundos, máximo 3000 iteraciones
- **Fallback Inteligente**: Se activa automáticamente si A* falla

### Dijkstra
- **Implementación**: Usa cola de prioridad para exploración óptima
- **Uso**: Principalmente para validación de algoritmos

### Características Técnicas Avanzadas

#### Renderizado Hexagonal:
- **Triángulos Optimizados**: Usa primitivas triangulares para mejor rendimiento
- **Efectos Visuales**: Sombras, gradientes, efectos de brillo y pulsación
- **Bordes Blancos**: Para máximo contraste y claridad visual

#### Sistema de Coordenadas:
- **Conversión Hexagonal**: Algoritmo matemático para grid → pantalla
- **Detección de Clicks**: Optimizada para formas hexagonales
- **Centrado Dinámico**: Adapta automáticamente el mapa al tamaño de ventana

#### Optimizaciones de Rendimiento:
- **60 FPS estables** en mapas hasta 18×14
- **Renderizado por capas** (fondo, grid, jugador, UI)
- **Lazy loading** de recursos
- **Pool de objetos** para algoritmos de pathfinding

## Características Destacadas

### **Tutorial Sincronizado**
Los iconos del tutorial son idénticos a los elementos del juego real, eliminando confusión visual.

### **Algoritmos Inteligentes**
- Predicen estados futuros del mapa
- Consideran mecánicas temporales complejas
- Incluyen protecciones contra cuelgues

### **UX**
- Interfaz responsiva y moderna
- Efectos visuales inmersivos
- Feedback inmediato de acciones

### **Robustez Técnica**
- Manejo de errores comprehensive
- Gestión de memoria cuidadosa
- Código modular y bien documentado

## Video Demo

*[Enlace al video demo de 10 minutos]*

## Créditos

Desarrollado como proyecto académico para el curso de Algoritmos y Estructuras de Datos.

**Equipo de Desarrollo:**
- [Nombres de los integrantes del equipo]

**Institución:** Universidad Centroamericana "José Simeon Cañas"
**Curso:** [Nombre del Curso]  
**Fecha:** Junio 2025

## Licencia

Este proyecto es de uso académico y no tiene fines comerciales.

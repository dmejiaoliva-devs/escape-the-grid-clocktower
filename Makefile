# Makefile Corregido para Escape the Grid
# Configurar PATH para incluir las herramientas de w64devkit
DEVKIT_PATH = C:/raylib/w64devkit/bin
CC = $(DEVKIT_PATH)/g++
AS = $(DEVKIT_PATH)/as
AR = $(DEVKIT_PATH)/ar
LD = $(DEVKIT_PATH)/ld

# Configurar PATH en el ambiente
export PATH := $(DEVKIT_PATH);$(PATH)

CFLAGS = -Wall -std=c++17 -O2 -Wno-missing-braces
INCLUDES = -I./src -IC:/raylib/raylib/src -IC:/raylib/raylib/src/external
LIBS = -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm

# Directorios
SRCDIR = src
BUILDDIR = build

# Archivos fuente
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
EXECUTABLE = EscapeTheGrid.exe

# Regla principal
all: $(BUILDDIR) $(EXECUTABLE)
	@echo "✅ Compilacion completada exitosamente!"

# Crear directorio build
$(BUILDDIR):
	@if not exist $(BUILDDIR) mkdir $(BUILDDIR)

# Compilar ejecutable
$(EXECUTABLE): $(OBJECTS)
	@echo "🔗 Enlazando ejecutable..."
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Compilar archivos objeto
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "📝 Compilando $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar
clean:
	@echo "🧹 Limpiando archivos..."
	@if exist $(BUILDDIR) rmdir /s /q $(BUILDDIR)
	@if exist $(EXECUTABLE) del $(EXECUTABLE)

# Ejecutar
run: $(EXECUTABLE)
	@echo "🚀 Ejecutando $(EXECUTABLE)..."
	./$(EXECUTABLE)

# Mostrar configuración
info:
	@echo "Compilador: $(CC)"
	@echo "PATH: $(PATH)"
	@echo "Fuentes: $(SOURCES)"
	@echo "Objetos: $(OBJECTS)"

.PHONY: all clean run info
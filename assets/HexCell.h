#pragma once
#include "raylib.h"
#include <string>

enum class CellType {
    FREE,
    WALL,
    START,
    GOAL,
    ITEM,
    GATE,
    TEMPORAL_WALL
};

class HexCell {
public:
    CellType type;
    int x, y;           // Coordenadas en la grid
    Vector2 screenPos;  // Posición en pantalla
    bool isVisited;
    bool isHighlighted;
    
    // Para mecánicas especiales
    std::string gatePattern;     // Para compuertas
    int turnsToOpen;            // Para paredes temporales
    bool isCurrentlyOpen;       // Estado actual de compuertas/paredes temporales
    
    HexCell(int gridX, int gridY, CellType cellType);
    void Draw(float size);
    bool IsPointInside(Vector2 point, float size);
    Vector2 GetScreenPosition(int gridX, int gridY, float hexSize);
    
private:
    Color GetCellColor(float size);
    void DrawHexagon(Vector2 center, float size, Color color);
};
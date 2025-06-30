
#pragma once
#include "raylib.h"
#include <string>
#include <cmath>

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
    int x, y;                   
    Vector2 screenPos;          
    bool isVisited;             
    bool isHighlighted;         
    std::string gatePattern;    
    int turnsToOpen;           
    bool isCurrentlyOpen;      
    
    
    HexCell();
    HexCell(int gridX, int gridY, CellType cellType);
    
    void Draw(float size);
    bool IsPointInside(Vector2 point, float size);
    Vector2 GetScreenPosition(int gridX, int gridY, float hexSize);
    
private:
    Color GetCellColor(float size);
    void DrawHexagon(Vector2 center, float size, Color color);
};
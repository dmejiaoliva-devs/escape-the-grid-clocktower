// Grid.h, este archivo define la clase Grid que representa una cuadrícula de celdas hexagonales en un juego. La clase incluye métodos para cargar datos desde un archivo, actualizar el estado de la cuadrícula, dibujar las celdas y manejar la lógica del juego.
#pragma once
#include "HexCell.h"
#include <vector>
#include <string>
#include <map>

class Grid {
public:
    int width, height;
    std::vector<std::vector<HexCell>> cells;
    Vector2 startPos, goalPos;
    int currentTurn;
    float hexSize;
    
    std::map<std::string, std::vector<bool>> gatePatterns;
    int turnCycleLength;
    
    Grid(int w, int h);
    bool LoadFromFile(const std::string& filename);
    void Update();
    void UpdateGatesAndWalls();
    void Draw();
    HexCell* GetCellAt(Vector2 mousePos);
    bool IsValidMove(int fromX, int fromY, int toX, int toY);
    std::vector<Vector2> GetNeighbors(int x, int y);
    
    Vector2 GetMapOffset();
    Vector2 HexToScreen(int x, int y, Vector2 offset);
    Vector2 GetPlayerScreenPosition(int gridX, int gridY);  
    
private:
    Vector2 HexToScreen(int x, int y);
};
// Grid.cpp, este implementa la lógica del grid y las celdas
#include "Grid.h"
#include "FileLoader.h"
#include <fstream>
#include <iostream>

Grid::Grid(int w, int h) : width(w), height(h), currentTurn(0), hexSize(25.0f), turnCycleLength(8) {
    cells.clear();
    cells.reserve(height);
    
    for (int y = 0; y < height; y++) {
        std::vector<HexCell> row;
        row.reserve(width);
        
        for (int x = 0; x < width; x++) {
            row.emplace_back(x, y, CellType::FREE);
        }
        
        cells.push_back(std::move(row));
    }
}

Vector2 Grid::GetMapOffset() {
    // offset para centrar el mapa en la pantalla
    float panelWidth = 250.0f; // Ancho de cada panel lateral
    float availableWidth = GetScreenWidth() - (panelWidth * 2);
    float availableHeight = GetScreenHeight() - 100.0f;
    
    // tamaño aproximado del mapa
    float hexWidth = hexSize * 2.0f;
    float hexHeight = sqrtf(3.0f) * hexSize;
    
    float mapWidth = hexWidth * 0.75f * width + hexSize;
    float mapHeight = hexHeight * height;
    
    float offsetX = panelWidth + (availableWidth - mapWidth) / 2.0f;
    float offsetY = 50.0f + (availableHeight - mapHeight) / 2.0f;
    
    return {offsetX, offsetY};
}

Vector2 Grid::GetPlayerScreenPosition(int gridX, int gridY) {
    Vector2 offset = GetMapOffset();
    return HexToScreen(gridX, gridY, offset);
}

std::vector<Vector2> Grid::GetNeighbors(int x, int y) {
    std::vector<Vector2> neighbors;
    
    int evenCol[6][2] = {{0, -1}, {1, -1}, {1, 0}, {0, 1}, {-1, 0}, {-1, -1}};
    int oddCol[6][2] = {{0, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    
    int (*offsets)[2] = (x % 2 == 0) ? evenCol : oddCol;
    
    for (int i = 0; i < 6; i++) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];
        
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            neighbors.push_back({(float)nx, (float)ny});
        }
    }
    
    return neighbors;
}

void Grid::Update() {
    UpdateGatesAndWalls();
}

void Grid::UpdateGatesAndWalls() {
    int cyclePosition = currentTurn % turnCycleLength;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            HexCell& cell = cells[y][x];
            
            if (cell.type == CellType::GATE) {
                if (gatePatterns.count(cell.gatePattern) > 0) {
                    std::vector<bool>& pattern = gatePatterns[cell.gatePattern];
                    if (cyclePosition < static_cast<int>(pattern.size())) {
                        cell.isCurrentlyOpen = pattern[cyclePosition];
                    }
                }
            } else if (cell.type == CellType::TEMPORAL_WALL) {
                cell.isCurrentlyOpen = (currentTurn >= cell.turnsToOpen);
            }
        }
    }
}

void Grid::Draw() {
    // offset de centrado del mapa
    Vector2 mapOffset = GetMapOffset();
    
    // Dibujar todas las celdas
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Actualizar posición de pantalla
            cells[y][x].screenPos = HexToScreen(x, y, mapOffset);
            // Dibujar la celda
            cells[y][x].Draw(hexSize);
        }
    }
    
    
}

Vector2 Grid::HexToScreen(int x, int y, Vector2 offset) {
    float hexWidth = hexSize * 2.0f;
    float hexHeight = sqrtf(3.0f) * hexSize;
    
    float screenX = hexWidth * 0.75f * x;
    float screenY = hexHeight * (y + 0.5f * (x & 1));
    
    return {screenX + offset.x, screenY + offset.y};
}

Vector2 Grid::HexToScreen(int x, int y) {
    Vector2 offset = GetMapOffset();
    return HexToScreen(x, y, offset);
}

HexCell* Grid::GetCellAt(Vector2 mousePos) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].IsPointInside(mousePos, hexSize)) {
                return &cells[y][x];
            }
        }
    }
    return nullptr;
}

bool Grid::IsValidMove(int fromX, int fromY, int toX, int toY) {
    if (toX < 0 || toX >= width || toY < 0 || toY >= height) {
        return false;
    }
    
    HexCell& targetCell = cells[toY][toX];
    
    if (targetCell.type == CellType::WALL || 
        (targetCell.type == CellType::GATE && !targetCell.isCurrentlyOpen) ||
        (targetCell.type == CellType::TEMPORAL_WALL && !targetCell.isCurrentlyOpen)) {
        return false;
    }
    
    std::vector<Vector2> neighbors = GetNeighbors(fromX, fromY);
    for (const Vector2& neighbor : neighbors) {
        if ((int)neighbor.x == toX && (int)neighbor.y == toY) {
            return true;
        }
    }
    
    return false;
}

bool Grid::LoadFromFile(const std::string& filename) {
    LevelData levelData;
    
    if (!FileLoader::LoadLevel(filename, levelData)) {
        return false;
    }
    
    width = levelData.width;
    height = levelData.height;
    startPos = {(float)levelData.startX, (float)levelData.startY};
    goalPos = {(float)levelData.goalX, (float)levelData.goalY};
    turnCycleLength = levelData.turnCycleLength;
    currentTurn = 0;
    
    // Ajustar tamaño de hexágono
    float maxDimension = std::max(width, height);
    if (maxDimension > 12) {
        hexSize = 20.0f;
    } else if (maxDimension > 8) {
        hexSize = 25.0f;
    } else {
        hexSize = 30.0f;
    }
    
    cells.clear();
    cells.reserve(height);
    
    for (int y = 0; y < height; y++) {
        std::vector<HexCell> row;
        row.reserve(width);
        
        for (int x = 0; x < width; x++) {
            CellType type = CellType::FREE;
            char cellChar = levelData.cellTypes[y][x];
            
            switch (cellChar) {
                case 'S': type = CellType::START; break;
                case 'G': type = CellType::GOAL; break;
                case '#': type = CellType::WALL; break;
                case '.': type = CellType::FREE; break;
                case 'T': type = CellType::TEMPORAL_WALL; break;
                default: type = CellType::FREE; break;
            }
            
            row.emplace_back(x, y, type);
            HexCell& cell = row.back();
            
            if (levelData.gateAssignments.count({x, y})) {
                cell.type = CellType::GATE;
                cell.gatePattern = levelData.gateAssignments.at({x, y});
            }
            
            if (levelData.temporalWalls.count({x, y})) {
                cell.type = CellType::TEMPORAL_WALL;
                cell.turnsToOpen = levelData.temporalWalls.at({x, y});
                cell.isCurrentlyOpen = false;
            }
        }
        
        cells.push_back(std::move(row));
    }
    
    for (const auto& item : levelData.items) {
        if (item.first >= 0 && item.first < width && item.second >= 0 && item.second < height) {
            cells[item.second][item.first].type = CellType::ITEM;
        }
    }
    
    gatePatterns = levelData.gatePatterns;
    UpdateGatesAndWalls();
    
    std::cout << "Grid cargado: " << width << "x" << height << " celdas" << std::endl;
    
    return true;
}
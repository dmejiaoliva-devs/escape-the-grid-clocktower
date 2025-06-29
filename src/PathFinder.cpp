// PathFinder.cpp, algorithm de búsqueda de caminos para un juego de hexágonos con mecánicas especiales.
#include "PathFinder.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <chrono>

PathFinder::PathFinder(Grid* g) : grid(g) {}

std::vector<Vector2> PathFinder::FindPathAStar() {
    const int MAX_ITERATIONS = 10000;
    const int MAX_TIME_MS = 5000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<PathNode*> openList;
    std::vector<PathNode*> closedList;
    
    int startX = (int)grid->startPos.x;
    int startY = (int)grid->startPos.y;
    int goalX = (int)grid->goalPos.x;
    int goalY = (int)grid->goalPos.y;
    
    std::cout << "A* iniciando desde (" << startX << "," << startY << ") hacia (" << goalX << "," << goalY << ")" << std::endl;
    
    // Verificar posiciones válidas
    if (startX < 0 || startX >= grid->width || startY < 0 || startY >= grid->height ||
        goalX < 0 || goalX >= grid->width || goalY < 0 || goalY >= grid->height) {
        std::cout << "ERROR: Posiciones inválidas!" << std::endl;
        return std::vector<Vector2>();
    }
    
    PathNode* startNode = new PathNode(startX, startY, 0);
    startNode->gCost = 0;
    startNode->hCost = CalculateHeuristic(startX, startY, goalX, goalY);
    startNode->fCost = startNode->gCost + startNode->hCost;
    
    openList.push_back(startNode);
    
    int iterations = 0;
    
    while (!openList.empty() && iterations < MAX_ITERATIONS) {
        iterations++;
        
        // Progress check cada 100 iteraciones
        if (iterations % 100 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            
            if (elapsed > MAX_TIME_MS) {
                std::cout << "TIMEOUT: A* excedió " << MAX_TIME_MS << "ms" << std::endl;
                break;
            }
            
            std::cout << "A* progreso: " << iterations << " iteraciones, " 
                      << openList.size() << " abiertos, " << closedList.size() << " cerrados" << std::endl;
        }
        
        // Encontrar nodo con menor fCost
        PathNode* currentNode = openList[0];
        for (size_t i = 1; i < openList.size(); i++) {
            if (openList[i]->fCost < currentNode->fCost || 
                (openList[i]->fCost == currentNode->fCost && openList[i]->hCost < currentNode->hCost)) {
                currentNode = openList[i];
            }
        }
        
        // Mover de openList a closedList
        openList.erase(std::find(openList.begin(), openList.end(), currentNode));
        closedList.push_back(currentNode);
        
        // ¿Llegamos al objetivo?
        if (currentNode->x == goalX && currentNode->y == goalY) {
            std::cout << "¡ÉXITO A*! Camino encontrado en " << iterations << " iteraciones." << std::endl;
            std::vector<Vector2> path = ReconstructPath(currentNode);
            
            // Limpiar memoria
            for (PathNode* node : openList) delete node;
            for (PathNode* node : closedList) delete node;
            
            return path;
        }
        
        // Límite de profundidad razonable
        if (currentNode->turn > 100) {
            continue;
        }
        
        // Examinar vecinos
        std::vector<Vector2> neighbors = grid->GetNeighbors(currentNode->x, currentNode->y);
        
        for (const Vector2& neighbor : neighbors) {
            int nx = (int)neighbor.x;
            int ny = (int)neighbor.y;
            int newTurn = currentNode->turn + 1;
            
            // Validar movimiento
            if (!IsValidMoveAtTurn(currentNode->x, currentNode->y, nx, ny, newTurn)) {
                continue;
            }
            
            // Verificar si ya está en closedList (VERSIÓN SIMPLE)
            bool inClosedList = false;
            for (PathNode* closedNode : closedList) {
                if (closedNode->x == nx && closedNode->y == ny && closedNode->turn == newTurn) {
                    inClosedList = true;
                    break;
                }
            }
            
            if (inClosedList) {
                continue;
            }
            
            int tentativeGCost = currentNode->gCost + 1;
            
            // Buscar en openList (VERSIÓN SIMPLE)
            PathNode* existingNode = nullptr;
            for (PathNode* openNode : openList) {
                if (openNode->x == nx && openNode->y == ny && openNode->turn == newTurn) {
                    existingNode = openNode;
                    break;
                }
            }
            
            if (existingNode == nullptr) {
                // Nuevo nodo
                PathNode* neighborNode = new PathNode(nx, ny, newTurn);
                neighborNode->gCost = tentativeGCost;
                neighborNode->hCost = CalculateHeuristic(nx, ny, goalX, goalY);
                neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
                neighborNode->parent = currentNode;
                openList.push_back(neighborNode);
            } else if (tentativeGCost < existingNode->gCost) {
                // Mejor camino al nodo existente
                existingNode->gCost = tentativeGCost;
                existingNode->fCost = existingNode->gCost + existingNode->hCost;
                existingNode->parent = currentNode;
            }
        }
        
        // Límite de memoria simple
        if (openList.size() + closedList.size() > 10000) {
            std::cout << "Límite de memoria alcanzado" << std::endl;
            break;
        }
    }
    
    // No se encontró camino
    std::cout << "A* FALLÓ después de " << iterations << " iteraciones." << std::endl;
    std::cout << "Nodos explorados: " << closedList.size() << ", En cola: " << openList.size() << std::endl;
    
    for (PathNode* node : openList) delete node;
    for (PathNode* node : closedList) delete node;
    
    return std::vector<Vector2>();
}

std::vector<Vector2> PathFinder::FindPathBFS() {
    const int MAX_ITERATIONS = 5000;
    const int MAX_TIME_MS = 3000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::queue<PathNode*> queue;
    std::vector<PathNode*> visited;
    
    int startX = (int)grid->startPos.x;
    int startY = (int)grid->startPos.y;
    int goalX = (int)grid->goalPos.x;
    int goalY = (int)grid->goalPos.y;
    
    std::cout << "BFS iniciando desde (" << startX << "," << startY << ") hacia (" << goalX << "," << goalY << ")" << std::endl;
    
    PathNode* startNode = new PathNode(startX, startY, 0);
    queue.push(startNode);
    visited.push_back(startNode);
    
    int iterations = 0;
    
    while (!queue.empty() && iterations < MAX_ITERATIONS) {
        iterations++;
        
        if (iterations % 100 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            
            if (elapsed > MAX_TIME_MS) {
                std::cout << "TIMEOUT BFS: " << elapsed << "ms" << std::endl;
                break;
            }
            
            std::cout << "BFS progreso: " << iterations << " iteraciones" << std::endl;
        }
        
        PathNode* currentNode = queue.front();
        queue.pop();
        
        // ¿Llegamos al objetivo?
        if (currentNode->x == goalX && currentNode->y == goalY) {
            std::cout << "¡ÉXITO BFS! Camino encontrado en " << iterations << " iteraciones." << std::endl;
            std::vector<Vector2> path = ReconstructPath(currentNode);
            
            // Limpiar memoria
            for (PathNode* node : visited) delete node;
            
            return path;
        }
        
        // Límite de profundidad
        if (currentNode->turn > 50) {
            continue;
        }
        
        // Examinar vecinos
        std::vector<Vector2> neighbors = grid->GetNeighbors(currentNode->x, currentNode->y);
        
        for (const Vector2& neighbor : neighbors) {
            int nx = (int)neighbor.x;
            int ny = (int)neighbor.y;
            int newTurn = currentNode->turn + 1;
            
            if (!IsValidMoveAtTurn(currentNode->x, currentNode->y, nx, ny, newTurn)) {
                continue;
            }
            
            // Verificar si ya fue visitado (VERSIÓN SIMPLE)
            bool alreadyVisited = false;
            for (PathNode* visitedNode : visited) {
                if (visitedNode->x == nx && visitedNode->y == ny && visitedNode->turn == newTurn) {
                    alreadyVisited = true;
                    break;
                }
            }
            
            if (alreadyVisited) {
                continue;
            }
            
            PathNode* neighborNode = new PathNode(nx, ny, newTurn);
            neighborNode->parent = currentNode;
            queue.push(neighborNode);
            visited.push_back(neighborNode);
        }
        
        // Límite de memoria
        if (visited.size() > 5000) {
            std::cout << "Límite de memoria BFS alcanzado" << std::endl;
            break;
        }
    }
    
    // Limpiar memoria
    for (PathNode* node : visited) delete node;
    
    std::cout << "BFS FALLÓ después de " << iterations << " iteraciones" << std::endl;
    return std::vector<Vector2>();
}

// VERSIÓN SIMPLIFICADA de IsValidMoveAtTurn (elimina complejidad innecesaria)
bool PathFinder::IsValidMoveAtTurn(int fromX, int fromY, int toX, int toY, int turn) {
    // Verificaciones básicas
    if (toX < 0 || toX >= grid->width || toY < 0 || toY >= grid->height) {
        return false;
    }
    
    // Verificar que sea vecino hexagonal válido
    std::vector<Vector2> neighbors = grid->GetNeighbors(fromX, fromY);
    bool isNeighbor = false;
    for (const Vector2& neighbor : neighbors) {
        if ((int)neighbor.x == toX && (int)neighbor.y == toY) {
            isNeighbor = true;
            break;
        }
    }
    
    if (!isNeighbor) {
        return false;
    }
    
    // Verificar tipo de celda
    HexCell& targetCell = grid->cells[toY][toX];
    
    switch (targetCell.type) {
        case CellType::WALL:
            return false;
            
        case CellType::GATE:
            // Verificar estado de compuerta
            if (grid->gatePatterns.find(targetCell.gatePattern) != grid->gatePatterns.end()) {
                int cyclePosition = turn % grid->turnCycleLength;
                std::vector<bool>& pattern = grid->gatePatterns[targetCell.gatePattern];
                if (cyclePosition < static_cast<int>(pattern.size())) {
                    return pattern[cyclePosition];
                }
            }
            return true; // Default abierta
            
        case CellType::TEMPORAL_WALL:
            return (turn >= targetCell.turnsToOpen);
            
        default:
            return true; // FREE, START, GOAL, ITEM son válidos
    }
}

int PathFinder::CalculateHeuristic(int x1, int y1, int x2, int y2) {
    // Heurística simple Manhattan (funciona bien para todos los casos)
    return abs(x2 - x1) + abs(y2 - y1);
}

std::vector<Vector2> PathFinder::ReconstructPath(PathNode* endNode) {
    std::vector<Vector2> path;
    PathNode* current = endNode;
    
    while (current != nullptr) {
        path.push_back({(float)current->x, (float)current->y});
        current = current->parent;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

// Funciones de compatibilidad (implementaciones simples)
bool PathFinder::IsNodeInList(std::vector<PathNode*>& list, int x, int y, int turn) {
    for (PathNode* node : list) {
        if (node->x == x && node->y == y && node->turn == turn) {
            return true;
        }
    }
    return false;
}

PathNode* PathFinder::GetNodeFromList(std::vector<PathNode*>& list, int x, int y, int turn) {
    for (PathNode* node : list) {
        if (node->x == x && node->y == y && node->turn == turn) {
            return node;
        }
    }
    return nullptr;
}

// Funciones optimizadas como alias de las simples
bool PathFinder::IsNodeInListOptimized(std::vector<PathNode*>& list, int x, int y, int turn) {
    return IsNodeInList(list, x, y, turn);
}

PathNode* PathFinder::GetNodeFromListOptimized(std::vector<PathNode*>& list, int x, int y, int turn) {
    return GetNodeFromList(list, x, y, turn);
}

std::vector<Vector2> PathFinder::FindPathDijkstra() {
    std::cout << "Usando BFS en lugar de Dijkstra..." << std::endl;
    return FindPathBFS();
}
#pragma once
#include "Grid.h"
#include <vector>
#include <queue>

struct PathNode {
    int x, y;
    int turn;       
    int gCost, hCost, fCost;
    PathNode* parent;
    
    PathNode(int x, int y, int turn) : x(x), y(y), turn(turn), 
                                       gCost(0), hCost(0), fCost(0), parent(nullptr) {}
};

class PathFinder {
public:
    Grid* grid;
    
    PathFinder(Grid* g);
    
    
    std::vector<Vector2> FindPathAStar();
    std::vector<Vector2> FindPathDijkstra();
    std::vector<Vector2> FindPathBFS();
    
    
    bool IsValidMoveAtTurn(int fromX, int fromY, int toX, int toY, int turn);
    int CalculateHeuristic(int x1, int y1, int x2, int y2);
    std::vector<Vector2> ReconstructPath(PathNode* endNode);
    
private:
    
    bool IsNodeInList(std::vector<PathNode*>& list, int x, int y, int turn);
    PathNode* GetNodeFromList(std::vector<PathNode*>& list, int x, int y, int turn);
    
    
    bool IsNodeInListOptimized(std::vector<PathNode*>& list, int x, int y, int turn);
    PathNode* GetNodeFromListOptimized(std::vector<PathNode*>& list, int x, int y, int turn);
};
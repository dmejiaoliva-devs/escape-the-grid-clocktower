#pragma once
#include "raylib.h"
#include <vector>

class Player {
public:
    int x, y;           
    int score;
    std::vector<Vector2> path;      
    std::vector<Vector2> items;     
    
    Player(int startX, int startY);
    void MoveTo(int newX, int newY);
    void AddToPath(int x, int y);
    bool HasVisited(int x, int y);
    void ReduceScoreForBacktrack();
    void Draw(float hexSize);
    
private:
    Vector2 GetScreenPos(int x, int y, float hexSize);
};
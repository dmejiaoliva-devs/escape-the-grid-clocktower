// Game.h
#pragma once
#include "Grid.h"
#include "Player.h"
#include "PathFinder.h"
#include <string>

enum class GameState {
    MENU,
    TUTORIAL,       
    PLAYING,
    AUTO_SOLVING,
    GAME_OVER,
    WIN
};

class Game {
public:
    Grid* grid;
    Player* player;
    PathFinder* pathFinder;
    GameState state;
    std::string currentLevel;
    std::string pendingLevel;  
    
    
    std::vector<Vector2> solutionPath;
    int solutionStep;
    float stepTimer;
    
    Game();
    ~Game();
    
    void Initialize();
    void LoadLevel(const std::string& filename);
    void Update();
    void Draw();
    void HandleMouseInput();
    void StartAutoSolve();
    void UpdateAutoSolve();
    void Reset();
    void ShowTutorial(const std::string& levelToLoad);  
    
private:
    void DrawUI();
    void DrawMenu();
    void DrawTutorial();            
    void DrawGameBackground();
    void DrawSidePanels();
    void DrawWinScreen();
    void DrawGameOverScreen();
    void DrawPlayerAtPosition(Vector2 pos, float hexSize);
    void DrawPlayerPath();
    void DrawTutorialElement(int x, int y, const char* title, const char* description, Color elementColor);
    void DrawHexagonTutorial(Vector2 center, float size, Color color);  
    bool IsGameWon();
};
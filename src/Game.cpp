// Pantalla de inicio del juego
#include "Game.h"
#include "FileLoader.h"
#include <iostream>
#include <cstring>

// Función pa crear colores
Color CreateColor(int r, int g, int b, int a) {
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

Game::Game() : grid(nullptr), player(nullptr), pathFinder(nullptr), 
               state(GameState::MENU), solutionStep(0), stepTimer(0.0f) {
}

Game::~Game() {
    delete grid;
    delete player;
    delete pathFinder;
}

void Game::Initialize() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1400, 900, "Escape the Grid - Clocktower Edition");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  
    
    state = GameState::MENU;
}

void Game::ShowTutorial(const std::string& levelToLoad) {
    pendingLevel = levelToLoad;
    state = GameState::TUTORIAL;
}

void Game::LoadLevel(const std::string& filename) {
    delete grid;
    delete player;
    delete pathFinder;
    
    grid = new Grid(10, 8);
    
    if (grid->LoadFromFile(filename)) {
        player = new Player((int)grid->startPos.x, (int)grid->startPos.y);
        pathFinder = new PathFinder(grid);
        state = GameState::PLAYING;
        currentLevel = filename;
        std::cout << "Nivel cargado: " << filename << std::endl;
    } else {
        std::cout << "Error cargando nivel: " << filename << std::endl;
        state = GameState::MENU;
    }
}

void Game::Update() {
    switch (state) {
        case GameState::MENU:
            // NIVELES - Ahora muestran tutorial antes de cargar
            if (IsKeyPressed(KEY_ONE)) ShowTutorial("assets/levels/level1.txt");
            if (IsKeyPressed(KEY_TWO)) ShowTutorial("assets/levels/level2.txt");
            if (IsKeyPressed(KEY_THREE)) ShowTutorial("assets/levels/level_expert.txt");
            if (IsKeyPressed(KEY_FOUR)) ShowTutorial("assets/levels/level_nightmare.txt");
            
            // PARA DEBUG: cargar nivel simple con T (también con tutorial)
            if (IsKeyPressed(KEY_T)) {
                ShowTutorial("DEBUG_LEVEL");
            }

            if (IsKeyPressed(KEY_ESCAPE)) {

                std::cout << "Presiona un número para seleccionar nivel" << std::endl;
            }
            break;
            
        case GameState::TUTORIAL:
            // En el tutorial, presionar cualquier tecla continúa al nivel
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (pendingLevel == "DEBUG_LEVEL") {
                    // Crear nivel de debug, este nivel no es necesario, solo era para pruebas, tengo que eliminarlo, pero siempre aparecen solo 4 :D
                    delete grid;
                    delete player;
                    delete pathFinder;
                    
                    grid = new Grid(8, 6);
                    grid->cells[0][0].type = CellType::START;
                    grid->cells[5][7].type = CellType::GOAL;
                    grid->cells[2][3].type = CellType::WALL;
                    grid->cells[3][4].type = CellType::ITEM;
                    
                    grid->startPos = {0, 0};
                    grid->goalPos = {7, 5};
                    
                    player = new Player(0, 0);
                    pathFinder = new PathFinder(grid);
                    state = GameState::PLAYING;
                    currentLevel = "DEBUG_LEVEL";
                    
                    std::cout << "Nivel de debug cargado" << std::endl;
                } else {
                    LoadLevel(pendingLevel);
                }
            }
            
            // ESC regresa al menú
            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "Volviendo al menú desde tutorial..." << std::endl;
                state = GameState::MENU;
            }
            break;
            
        case GameState::PLAYING:
            HandleMouseInput();
            if (grid) grid->Update();
            if (IsGameWon()) {
                state = GameState::WIN;
            }
            if (IsKeyPressed(KEY_R)) Reset();
            if (IsKeyPressed(KEY_SPACE)) StartAutoSolve();

            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "Volviendo al menú desde juego..." << std::endl;
                state = GameState::MENU;
            }
            break;
            
        case GameState::AUTO_SOLVING:
            UpdateAutoSolve();
            if (grid) grid->Update();
            if (IsKeyPressed(KEY_R)) Reset();
            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "Volviendo al menú desde auto-solve..." << std::endl;
                state = GameState::MENU;
            }
            break;
            
        case GameState::WIN:
            if (IsKeyPressed(KEY_R)) Reset();
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::MENU;
            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "Volviendo al menú desde victoria..." << std::endl;
                state = GameState::MENU;
            }
            break;
            
        case GameState::GAME_OVER:
            if (IsKeyPressed(KEY_R)) Reset();
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::MENU;
            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "Volviendo al menú desde game over..." << std::endl;
                state = GameState::MENU;
            }
            break;
    }
}

void Game::HandleMouseInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        HexCell* clickedCell = grid->GetCellAt(mousePos);
        
        if (clickedCell != nullptr) {
            if (grid->IsValidMove(player->x, player->y, clickedCell->x, clickedCell->y)) {
                if (player->HasVisited(clickedCell->x, clickedCell->y)) {
                    player->ReduceScoreForBacktrack();
                }
                
                player->MoveTo(clickedCell->x, clickedCell->y);
                player->AddToPath(clickedCell->x, clickedCell->y);
                clickedCell->isVisited = true;
                
                if (clickedCell->type == CellType::ITEM) {
                    player->items.push_back({(float)clickedCell->x, (float)clickedCell->y});
                    clickedCell->type = CellType::FREE;
                    player->score += 100;
                }
                
                grid->currentTurn++;
            }
        }
    }
    
    Vector2 mousePos = GetMousePosition();
    
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            grid->cells[y][x].isHighlighted = false;
        }
    }
    
    HexCell* hoveredCell = grid->GetCellAt(mousePos);
    if (hoveredCell != nullptr) {
        hoveredCell->isHighlighted = true;
    }
}

void Game::StartAutoSolve() {
    std::cout << "Iniciando resolución automática..." << std::endl;
    solutionPath = pathFinder->FindPathAStar();
    
    if (!solutionPath.empty()) {
        state = GameState::AUTO_SOLVING;
        solutionStep = 0;
        stepTimer = 0.0f;
        std::cout << "Camino encontrado con " << solutionPath.size() << " pasos." << std::endl;
    } else {
        std::cout << "No se encontró solución!" << std::endl;
    }
}

void Game::UpdateAutoSolve() {
    stepTimer += GetFrameTime();
    
    if (stepTimer >= 0.5f && solutionStep < static_cast<int>(solutionPath.size())) {
        Vector2 nextPos = solutionPath[solutionStep];
        
        if (player->HasVisited((int)nextPos.x, (int)nextPos.y)) {
            player->ReduceScoreForBacktrack();
        }
        
        player->MoveTo((int)nextPos.x, (int)nextPos.y);
        player->AddToPath((int)nextPos.x, (int)nextPos.y);
        grid->cells[(int)nextPos.y][(int)nextPos.x].isVisited = true;
        
        HexCell& cell = grid->cells[(int)nextPos.y][(int)nextPos.x];
        if (cell.type == CellType::ITEM) {
            player->items.push_back(nextPos);
            cell.type = CellType::FREE;
            player->score += 100;
        }
        
        solutionStep++;
        stepTimer = 0.0f;
        grid->currentTurn++;
        
        if (solutionStep >= static_cast<int>(solutionPath.size())) {
            if (IsGameWon()) {
                state = GameState::WIN;
            } else {
                state = GameState::PLAYING;
            }
        }
    }
}

void Game::Draw() {
    BeginDrawing();
    
    // Gradiente de fondo usando colores
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), DARKBLUE, DARKGRAY);
    
    switch (state) {
        case GameState::MENU:
            DrawMenu();
            break;
            
        case GameState::TUTORIAL:
            DrawTutorial();
            break;
            
        case GameState::PLAYING:
        case GameState::AUTO_SOLVING:
            if (grid != nullptr) {
                DrawGameBackground();
                DrawSidePanels();
                grid->Draw();
                
                if (player != nullptr) {
                    // Dibujar el jugador usando la posición exacta del grid
                    Vector2 playerPos = grid->GetPlayerScreenPosition(player->x, player->y);
                    DrawPlayerAtPosition(playerPos, grid->hexSize);
                    
                    // Dibuja el camino del jugador
                    DrawPlayerPath();
                }
                
                if (state == GameState::AUTO_SOLVING && !solutionPath.empty()) {
                    for (int i = 0; i < static_cast<int>(solutionPath.size()) - 1; i++) {
                        Vector2 from = grid->cells[(int)solutionPath[i].y][(int)solutionPath[i].x].screenPos;
                        Vector2 to = grid->cells[(int)solutionPath[i+1].y][(int)solutionPath[i+1].x].screenPos;
                        DrawLineEx(from, to, 4.0f, RED);
                    }
                }
                
                DrawUI();
            }
            break;
            
        case GameState::WIN:
            if (grid != nullptr) {
                DrawGameBackground();
                DrawSidePanels();
                grid->Draw();
                if (player != nullptr) {
                    Vector2 playerPos = grid->GetPlayerScreenPosition(player->x, player->y);
                    DrawPlayerAtPosition(playerPos, grid->hexSize);
                    DrawPlayerPath();
                }
                DrawUI();
            }
            DrawWinScreen();
            break;
            
        case GameState::GAME_OVER:
            DrawGameOverScreen();
            break;
    }
    
    EndDrawing();
}

void Game::DrawTutorial() {
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    
    // Fondo semi-transparente
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CreateColor(0, 0, 0, 180));
    
    // Panel principal del tutorial
    Rectangle tutorialPanel = {100, 50, (float)(GetScreenWidth() - 200), (float)(GetScreenHeight() - 100)};
    DrawRectangleRounded(tutorialPanel, 0.02f, 8, DARKBLUE);
    DrawRectangleRoundedLines(tutorialPanel, 0.02f, 8, SKYBLUE);
    
    // Título
    DrawText("GUÍA DE ELEMENTOS DEL JUEGO", centerX - 250, 80, 32, GOLD);
    DrawText("Aprende qué encontrarás en el mapa", centerX - 150, 120, 18, LIGHTGRAY);
    
    // Two columnas 
    int leftColumnX = 150;
    int rightColumnX = centerX + 100;
    int startY = 160;
    int elementSpacing = 80;
    
    // Columna izquierda
    DrawTutorialElement(leftColumnX, startY, "Jugador (TÚ)", "Muévete con click izquierdo", ORANGE);
    
    DrawTutorialElement(leftColumnX, startY + elementSpacing, "Punto de Inicio", "Donde comienzas", GREEN);
    
    DrawTutorialElement(leftColumnX, startY + elementSpacing * 2, "Meta", "Objetivo a alcanzar", RED);
    
    DrawTutorialElement(leftColumnX, startY + elementSpacing * 3, "Gemas", "Recoléctalas para puntos (+100)", GOLD);
    
    // Columna derecha
    DrawTutorialElement(rightColumnX, startY, "Paredes", "No se pueden atravesar", BROWN);
    
    DrawTutorialElement(rightColumnX, startY + elementSpacing, "Compuertas Abiertas", "Puedes pasar", BLUE);
    
    DrawTutorialElement(rightColumnX, startY + elementSpacing * 2, "Compuertas Cerradas", "Bloqueadas temporalmente", PURPLE);
    
    DrawTutorialElement(rightColumnX, startY + elementSpacing * 3, "Paredes Temporales", "Se abren después de N turnos", DARKGRAY);
    
    // Instrucciones para continuar
    Rectangle continuePanel = {(float)(centerX - 200), (float)(GetScreenHeight() - 120), 400, 60};
    DrawRectangleRounded(continuePanel, 0.05f, 8, DARKGREEN);
    DrawRectangleRoundedLines(continuePanel, 0.05f, 8, LIME);
    
    DrawText("ESPACIO / ENTER / CLICK - Continuar al nivel", centerX - 180, GetScreenHeight() - 105, 16, WHITE);
    DrawText("ESC - Volver al menú", centerX - 70, GetScreenHeight() - 85, 14, LIGHTGRAY);
}

void Game::DrawTutorialElement(int x, int y, const char* title, const char* description, Color elementColor) {
    
    int iconSize = 30;
    Vector2 iconCenter = {(float)(x + iconSize/2), (float)(y + iconSize/2)};
    
    
    DrawHexagonTutorial(iconCenter, iconSize/2.0f, elementColor);
    
    // Símbolos específicos IGUALES a los del juego
    if (strcmp(title, "Jugador (TÚ)") == 0) {
        // Igual que en el juego pero mas peque
        float radius = iconSize * 0.3f;
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, radius + 2, YELLOW);
        DrawCircleGradient((int)iconCenter.x, (int)iconCenter.y, radius, ORANGE, GOLD);
        DrawCircleLines((int)iconCenter.x, (int)iconCenter.y, radius, BROWN);
        DrawText("D", (int)(iconCenter.x - 4), (int)(iconCenter.y - 6), 12, DARKBROWN);
    }
    else if (strcmp(title, "Punto de Inicio") == 0) {
        // Círculos concéntricos verdes como en el juego
        float symbolSize = iconSize * 0.4f;
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, symbolSize * 0.8f, CreateColor(0, 150, 0, 255));
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, symbolSize * 0.6f, CreateColor(50, 200, 50, 255));
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, symbolSize * 0.3f, CreateColor(100, 255, 100, 255));
    }
    else if (strcmp(title, "Meta") == 0) {
        // Estrella igual que en el juego
        float symbolSize = iconSize * 0.4f;
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, symbolSize * 0.8f, CreateColor(150, 0, 0, 255));
        
        // Estrella simple (cruz + aspa)
        float starSize = symbolSize * 0.5f;
        Color starColor = CreateColor(255, 255, 100, 255);
        
        // Cruz principal
        DrawLineEx({iconCenter.x - starSize, iconCenter.y}, {iconCenter.x + starSize, iconCenter.y}, 2.0f, starColor);
        DrawLineEx({iconCenter.x, iconCenter.y - starSize}, {iconCenter.x, iconCenter.y + starSize}, 2.0f, starColor);
        
        // Aspa
        DrawLineEx({iconCenter.x - starSize * 0.7f, iconCenter.y - starSize * 0.7f}, 
                  {iconCenter.x + starSize * 0.7f, iconCenter.y + starSize * 0.7f}, 1.5f, starColor);
        DrawLineEx({iconCenter.x + starSize * 0.7f, iconCenter.y - starSize * 0.7f}, 
                  {iconCenter.x - starSize * 0.7f, iconCenter.y + starSize * 0.7f}, 1.5f, starColor);
    }
    else if (strcmp(title, "Gemas") == 0) {
        // Círculo dorado brillante igual que en el juego
        float symbolSize = iconSize * 0.4f;
        float gemRadius = symbolSize * 0.4f;
        
        // Sombra
        DrawCircle((int)(iconCenter.x + 1), (int)(iconCenter.y + 1), gemRadius + 1, CreateColor(0, 0, 0, 100));
        
        // Círculo exterior dorado oscuro
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, gemRadius + 1, CreateColor(200, 150, 0, 255));
        
        // Círculo principal dorado
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, gemRadius, CreateColor(255, 215, 0, 255));
        
        // Círculo interior brillante
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, gemRadius * 0.6f, CreateColor(255, 255, 150, 255));
        
        // Punto de brillo
        DrawCircle((int)(iconCenter.x - gemRadius * 0.3f), (int)(iconCenter.y - gemRadius * 0.3f), 1, CreateColor(255, 255, 255, 255));
    }
    else if (strcmp(title, "Paredes") == 0) {
        // Patrón de ladrillos 
        Color brickColor = CreateColor(255, 0, 0, 255);
        float brickSize = iconSize * 0.3f;
        
        // Patrón de ladrillos 2x2, indica que es una pared
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                float offsetX = (col - 0.5f) * brickSize * 0.8f;
                float offsetY = (row - 0.5f) * brickSize * 0.8f;
                
                DrawRectangle(
                    (int)(iconCenter.x + offsetX - brickSize * 0.3f),
                    (int)(iconCenter.y + offsetY - brickSize * 0.3f),
                    (int)(brickSize * 0.6f),
                    (int)(brickSize * 0.6f),
                    brickColor
                );
            }
        }
    }
    else if (strcmp(title, "Compuertas Abiertas") == 0) {
        // Barras separadas esta open
        float symbolSize = iconSize * 0.4f;
        Color gateColor = CreateColor(0, 100, 255, 255);
        
        DrawRectangle(
            (int)(iconCenter.x - symbolSize * 0.6f), 
            (int)(iconCenter.y - symbolSize * 0.3f),
            (int)(symbolSize * 0.3f), 
            (int)(symbolSize * 0.6f), 
            gateColor
        );
        DrawRectangle(
            (int)(iconCenter.x + symbolSize * 0.3f), 
            (int)(iconCenter.y - symbolSize * 0.3f),
            (int)(symbolSize * 0.3f), 
            (int)(symbolSize * 0.6f), 
            gateColor
        );
    }
    else if (strcmp(title, "Compuertas Cerradas") == 0) {
        // Barra sólida con X, indica que esta closed
        float symbolSize = iconSize * 0.4f;
        Color gateColor = CreateColor(255, 140, 0, 255);
        
        // Barra 
        DrawRectangle(
            (int)(iconCenter.x - symbolSize * 0.6f), 
            (int)(iconCenter.y - symbolSize * 0.1f),
            (int)(symbolSize * 1.2f), 
            (int)(symbolSize * 0.2f), 
            gateColor
        );
        // X indica que esta closed
        DrawLineEx(
            {iconCenter.x - symbolSize * 0.3f, iconCenter.y - symbolSize * 0.3f}, 
            {iconCenter.x + symbolSize * 0.3f, iconCenter.y + symbolSize * 0.3f}, 
            2.0f, CreateColor(255, 0, 0, 255)
        );
        DrawLineEx(
            {iconCenter.x + symbolSize * 0.3f, iconCenter.y - symbolSize * 0.3f}, 
            {iconCenter.x - symbolSize * 0.3f, iconCenter.y + symbolSize * 0.3f}, 
            2.0f, CreateColor(255, 0, 0, 255)
        );
    }
    else if (strcmp(title, "Paredes Temporales") == 0) {
        // Reloj igualito al que aparece en el juego
        float symbolSize = iconSize * 0.4f;
        Color clockColor = CreateColor(255, 203, 0, 255);
        
        // Círculo del reloj
        DrawCircleLines((int)iconCenter.x, (int)iconCenter.y, symbolSize * 0.6f, clockColor);
        DrawCircle((int)iconCenter.x, (int)iconCenter.y, 1, clockColor);
        
        // Manecillas del reloj
        DrawLineEx(iconCenter, {iconCenter.x, iconCenter.y - symbolSize * 0.4f}, 1.5f, clockColor);
        DrawLineEx(iconCenter, {iconCenter.x + symbolSize * 0.3f, iconCenter.y}, 1.5f, clockColor);
        
        // Número de ejemplo
        DrawText("3", (int)(iconCenter.x - 3), (int)(iconCenter.y + symbolSize * 0.7f), 10, CreateColor(255, 255, 255, 255));
    }
    
    // Texto explicativo
    DrawText(title, x + iconSize + 10, y, 16, WHITE);
    DrawText(description, x + iconSize + 10, y + 20, 12, LIGHTGRAY);
}

// Dibuja hexágonos del tutorial
void Game::DrawHexagonTutorial(Vector2 center, float size, Color color) {
    Vector2 points[6];
    for (int i = 0; i < 6; i++) {
        float angle = i * PI / 3.0f;
        points[i].x = center.x + size * cosf(angle);
        points[i].y = center.y + size * sinf(angle);
    }
    
    // Hexágono principal relleno
    for (int i = 1; i < 5; i++) {
        DrawTriangle(center, points[i], points[i + 1], color);
    }
    DrawTriangle(center, points[5], points[0], color);
    DrawTriangle(center, points[0], points[1], color);
    
    // Bordes blancos
    for (int i = 0; i < 6; i++) {
        int next = (i + 1) % 6;
        DrawLineEx(points[i], points[next], 1.5f, WHITE);
    }
}

void Game::DrawPlayerAtPosition(Vector2 pos, float hexSize) {
    // Animación de pulso
    static float pulseTimer = 0.0f;
    pulseTimer += GetFrameTime() * 3.0f;
    float pulseScale = 1.0f + sinf(pulseTimer) * 0.1f;
    float radius = hexSize * 0.4f * pulseScale;
    
    // Sombra de juagdor
    DrawCircle((int)(pos.x + 3), (int)(pos.y + 3), radius, GRAY);
    
    // Círculo exterior 
    DrawCircle((int)pos.x, (int)pos.y, radius + 3, YELLOW);
    
    // Círculo principal 
    DrawCircleGradient((int)pos.x, (int)pos.y, radius, ORANGE, GOLD);
    
    // Borde de fiucha de jugador
    DrawCircleLines((int)pos.x, (int)pos.y, radius, BROWN);
    
    // Simbolo de jugador 
    DrawText("D", (int)(pos.x - 6), (int)(pos.y - 8), 16, DARKBROWN);
}

//Camino del jugador
void Game::DrawPlayerPath() {
    if (player->path.size() > 1) {
        for (int i = 1; i < static_cast<int>(player->path.size()); i++) {
            Vector2 from = grid->GetPlayerScreenPosition((int)player->path[i-1].x, (int)player->path[i-1].y);
            Vector2 to = grid->GetPlayerScreenPosition((int)player->path[i].x, (int)player->path[i].y);
            
            // Sombra
            DrawLineEx({from.x + 2, from.y + 2}, {to.x + 2, to.y + 2}, 3.0f, GRAY);
            
            Color lineColor = (i == static_cast<int>(player->path.size()) - 1) ? ORANGE : GOLD;
            DrawLineEx(from, to, 2.0f, lineColor);
            
            if (i < static_cast<int>(player->path.size()) - 1) {
                DrawCircle((int)to.x, (int)to.y, 2, ORANGE);
            }
        }
    }
}

void Game::DrawGameBackground() {
    int panelX = 250;
    int panelY = 50;
    int panelWidth = GetScreenWidth() - 500;
    int panelHeight = GetScreenHeight() - 100;
    
    DrawRectangle(panelX + 5, panelY + 5, panelWidth, panelHeight, GRAY);
    Rectangle panelRect = {(float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight};
    DrawRectangleRounded(panelRect, 0.02f, 8, DARKGRAY);
    DrawRectangleRoundedLines(panelRect, 0.02f, 8, LIGHTGRAY);
}

void Game::DrawSidePanels() {
    Rectangle leftPanel = {20, 20, 200, (float)(GetScreenHeight() - 40)};
    DrawRectangleRounded(leftPanel, 0.05f, 8, DARKGRAY);
    DrawRectangleRoundedLines(leftPanel, 0.05f, 8, GRAY);
    
    float rightX = GetScreenWidth() - 220.0f;
    Rectangle rightPanel = {rightX, 20, 200, (float)(GetScreenHeight() - 40)};
    DrawRectangleRounded(rightPanel, 0.05f, 8, DARKGRAY);
    DrawRectangleRoundedLines(rightPanel, 0.05f, 8, GRAY);
}

void Game::DrawMenu() {
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    DrawText("ESCAPE THE GRID", centerX - 280, 120, 60, GRAY);
    DrawText("ESCAPE THE GRID", centerX - 285, 115, 60, WHITE);
    DrawText("Clocktower Edition", centerX - 120, 190, 24, SKYBLUE);

    Rectangle infoPanel = {50, 240, (float)(GetScreenWidth() - 100), 350};
    DrawRectangleRounded(infoPanel, 0.02f, 8, DARKBLUE);
    DrawRectangleRoundedLines(infoPanel, 0.02f, 8, BLUE);

    DrawText("Mecánicas Especiales:", 80, 270, 24, GOLD);
    DrawText("• Compuertas que se abren/cierran por turnos", 100, 305, 18, LIGHTGRAY);
    DrawText("• Paredes temporales que se abren después de N turnos", 100, 330, 18, LIGHTGRAY);
    DrawText("• Sistema de puntuación con penalización por backtracking", 100, 355, 18, LIGHTGRAY);
    DrawText("• Controles solo por mouse en grid hexagonal", 100, 380, 18, LIGHTGRAY);

    DrawText("Controles:", 80, 420, 24, GOLD);
    DrawText("• Click izquierdo: Mover a celda hexagonal", 100, 450, 18, LIGHTGRAY);
    DrawText("• ESPACIO: Resolver automáticamente", 100, 475, 18, LIGHTGRAY);
    DrawText("• R: Reiniciar nivel", 100, 500, 18, LIGHTGRAY);

    Rectangle levelPanel = {(float)(centerX - 200), 550, 400, 120};
    DrawRectangleRounded(levelPanel, 0.02f, 8, DARKBLUE);
    DrawRectangleRoundedLines(levelPanel, 0.02f, 8, BLUE);

    DrawText("Selecciona un nivel:", centerX - 100, 570, 20, WHITE);
    DrawText("1 - Nivel Básico", centerX - 180, 600, 20, GREEN);
    DrawText("2 - Nivel Intermedio", centerX - 10, 600, 20, YELLOW); 
    DrawText("3 - EXPERTO", centerX - 180, centerY + 180, 20, RED);
    DrawText("4 - NIGHTMARE", centerX - 10, centerY + 180, 20, MAROON);
}

void Game::DrawUI() {
    if (player != nullptr) {
        DrawText("INFORMACIÓN", 40, 40, 18, GOLD);
        DrawText(TextFormat("Turno: %d", grid->currentTurn), 40, 70, 16, WHITE);
        DrawText(TextFormat("Puntuación: %d", player->score), 40, 95, 16, LIME);
        DrawText(TextFormat("Items: %d", (int)player->items.size()), 40, 120, 16, GOLD);
        DrawText(TextFormat("Posición: (%d, %d)", player->x, player->y), 40, 145, 16, SKYBLUE);
        
        float rightX = GetScreenWidth() - 200.0f;
        DrawText("CONTROLES", (int)rightX + 20, 40, 18, GOLD);
        DrawText("ESPACIO:", (int)rightX + 20, 70, 14, LIGHTGRAY);
        DrawText("Auto-resolver", (int)rightX + 20, 85, 12, GRAY);
        DrawText("R: Reiniciar", (int)rightX + 20, 110, 14, LIGHTGRAY);
        DrawText("ESC: Menú", (int)rightX + 20, 130, 14, LIGHTGRAY);

        if (state == GameState::AUTO_SOLVING) {
            DrawText("RESOLVIENDO...", (int)rightX + 20, 160, 14, RED);
        }
    }
}
// Pantalla de victoria
void Game::DrawWinScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CreateColor(0, 0, 0, 180));

    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;

    Rectangle winPanel = {(float)(centerX - 250), (float)(centerY - 100), 500, 200};
    DrawRectangleRounded(winPanel, 0.05f, 8, DARKGREEN);
    DrawRectangleRoundedLines(winPanel, 0.05f, 8, LIME);

    DrawText("¡VICTORIA!", centerX - 120, centerY - 60, 48, LIME);
    DrawText(TextFormat("Puntuación Final: %d", player->score), centerX - 100, centerY - 10, 20, WHITE);
    DrawText("R: Reiniciar | ESC: Menú", centerX - 100, centerY + 30, 16, LIGHTGRAY);
}


// Pantalla de Game Over
void Game::DrawGameOverScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CreateColor(0, 0, 0, 180));
    
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    
    DrawText("GAME OVER", centerX - 150, centerY - 30, 48, RED);
    DrawText("R: Reiniciar | ESC: Menú", centerX - 100, centerY + 30, 20, LIGHTGRAY);
}

bool Game::IsGameWon() {
    if (player == nullptr || grid == nullptr) return false;
    return (player->x == (int)grid->goalPos.x && player->y == (int)grid->goalPos.y);
}

void Game::Reset() {
    if (grid != nullptr && !currentLevel.empty()) {
        LoadLevel(currentLevel);
    }
}
#include "Player.h"
#include <algorithm>
#include <cmath>

Player::Player(int startX, int startY) : x(startX), y(startY), score(1000) {
    path.clear();
    items.clear();
    AddToPath(x, y);
}

void Player::MoveTo(int newX, int newY) {
    x = newX;
    y = newY;
}

void Player::AddToPath(int x, int y) {
    path.push_back({(float)x, (float)y});
}

bool Player::HasVisited(int x, int y) {
    for (const Vector2& pos : path) {
        if ((int)pos.x == x && (int)pos.y == y) {
            return true;
        }
    }
    return false;
}

void Player::ReduceScoreForBacktrack() {
    score -= 50;
    if (score < 0) score = 0;
}

Vector2 Player::GetScreenPos(int x, int y, float hexSize) {
    // Usar exactamente la misma lógica que Grid::HexToScreen
    float hexWidth = hexSize * 2.0f;
    float hexHeight = sqrtf(3.0f) * hexSize;
    
    float screenX = hexWidth * 0.75f * x;
    float screenY = hexHeight * (y + 0.5f * (x & 1));
    
    // Calcular el mismo offset que usa Grid::GetMapOffset()
    // Esto debería coincidir exactamente con Grid.cpp
    float mapWidth = hexWidth * 0.75f * 9 + hexSize * 2;  
    float mapHeight = hexHeight * 7 + hexHeight * 0.5f;   
    
    float availableWidth = GetScreenWidth() - 500; // 250px cada panel lateral
    float availableHeight = GetScreenHeight() - 100; // 50px arriba y abajo
    
    float offsetX = 250 + (availableWidth - mapWidth) / 2;
    float offsetY = 50 + (availableHeight - mapHeight) / 2;
    
    return {screenX + offsetX, screenY + offsetY};
}

void Player::Draw(float hexSize) {
    Vector2 pos = GetScreenPos(x, y, hexSize);
    
    // Efecto de respiración (pulsating)
    static float pulseTimer = 0.0f;
    pulseTimer += GetFrameTime() * 3.0f;
    float pulseScale = 1.0f + sinf(pulseTimer) * 0.1f;
    float radius = hexSize * 0.4f * pulseScale; // Hacer más pequeño para que se vea mejor
    
    // Sombra del jugador
    DrawCircle((int)(pos.x + 3), (int)(pos.y + 3), radius, GRAY);
    
    // Círculo exterior (glow)
    DrawCircle((int)pos.x, (int)pos.y, radius + 3, YELLOW);
    
    // Círculo principal del jugador
    DrawCircleGradient((int)pos.x, (int)pos.y, radius, ORANGE, GOLD);
    
    // Borde del jugador
    DrawCircleLines((int)pos.x, (int)pos.y, radius, BROWN);
    
    // Símbolo del jugador
    DrawText("P", (int)(pos.x - 6), (int)(pos.y - 8), 16, DARKBROWN);
    
    // Dibujar el camino recorrido con mejor precisión
    if (path.size() > 1) {
        for (int i = 1; i < static_cast<int>(path.size()); i++) {
            Vector2 from = GetScreenPos((int)path[i-1].x, (int)path[i-1].y, hexSize);
            Vector2 to = GetScreenPos((int)path[i].x, (int)path[i].y, hexSize);
            
            // Línea de sombra
            DrawLineEx({from.x + 2, from.y + 2}, {to.x + 2, to.y + 2}, 3.0f, GRAY);
            
            // Línea principal
            float alpha = 0.5f + (0.5f * i / path.size()); // Más opaco hacia el final
            Color lineColor = (i == static_cast<int>(path.size()) - 1) ? ORANGE : GOLD;
            DrawLineEx(from, to, 2.0f, lineColor);
            
            // Puntos en el camino (excepto posición actual)
            if (i < static_cast<int>(path.size()) - 1) {
                DrawCircle((int)to.x, (int)to.y, 2, ORANGE);
            }
        }
    }
}
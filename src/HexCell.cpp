#include "HexCell.h"

Color CreateCustomColor(int r, int g, int b, int a) {
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

// Constructor por defecto, crea una celda libre en (0,0)
HexCell::HexCell() 
    : type(CellType::FREE),           
      x(0), y(0),                     
      screenPos{0.0f, 0.0f},         
      isVisited(false),              
      isHighlighted(false),          
      gatePattern(""),               
      turnsToOpen(0),                
      isCurrentlyOpen(true) {        
}

// Constructor con parámetros, deja elegir la posición y el tipo de celda al crearla
HexCell::HexCell(int gridX, int gridY, CellType cellType) 
    : type(cellType),                
      x(gridX), y(gridY),            
      screenPos{0.0f, 0.0f},        
      isVisited(false),              
      isHighlighted(false),          
      gatePattern(""),               
      turnsToOpen(0),                
      isCurrentlyOpen(true) {        
}

Vector2 HexCell::GetScreenPosition(int gridX, int gridY, float hexSize) {
    float hexWidth = hexSize * 2.0f;
    float hexHeight = sqrtf(3.0f) * hexSize;
    
    float posX = hexWidth * 0.75f * gridX;
    float posY = hexHeight * (gridY + 0.5f * (gridX & 1));
    
    return {posX + 100.0f, posY + 100.0f};
}

void HexCell::DrawHexagon(Vector2 center, float size, Color color) {
    Vector2 points[6];
    for (int i = 0; i < 6; i++) {
        float angle = i * PI / 3.0f;
        points[i].x = center.x + size * cosf(angle);
        points[i].y = center.y + size * sinf(angle);
    }
    
    // Efecto de sombra 
    Vector2 shadowCenter = {center.x + 1, center.y + 1};
    Color shadowColor = CreateCustomColor(0, 0, 0, 80);
    
    for (int i = 1; i < 5; i++) {
        DrawTriangle(shadowCenter, 
                    {points[i].x + 1, points[i].y + 1}, 
                    {points[i + 1].x + 1, points[i + 1].y + 1}, 
                    shadowColor);
    }
    DrawTriangle(shadowCenter, 
                {points[5].x + 1, points[5].y + 1}, 
                {points[0].x + 1, points[0].y + 1}, 
                shadowColor);
    DrawTriangle(shadowCenter, 
                {points[0].x + 1, points[0].y + 1}, 
                {points[1].x + 1, points[1].y + 1}, 
                shadowColor);
    
    // Hexágono principal relleno
    for (int i = 1; i < 5; i++) {
        DrawTriangle(center, points[i], points[i + 1], color);
    }
    DrawTriangle(center, points[5], points[0], color);
    DrawTriangle(center, points[0], points[1], color);
    
    // Bordes SIEMPRE BLANCOS 
    Color borderColor = WHITE;
    float borderWidth = isHighlighted ? 4.0f : 2.0f;
    
    for (int i = 0; i < 6; i++) {
        int next = (i + 1) % 6;
        DrawLineEx(points[i], points[next], borderWidth, borderColor);
    }
    
    // Highlight effect adicional
    if (isHighlighted) {
        // Glow effect amarillo
        for (int i = 0; i < 6; i++) {
            float angle = i * PI / 3.0f;
            Vector2 glowPoint = {
                center.x + (size + 4) * cosf(angle),
                center.y + (size + 4) * sinf(angle)
            };
            DrawCircle((int)glowPoint.x, (int)glowPoint.y, 3, YELLOW);
        }
    }
}

Color HexCell::GetCellColor(float size) {
    
    (void)size;
    
    if (isHighlighted) {
        // Colores más brillantes cuando está resaltado
        switch (type) {
            case CellType::FREE:
                return isVisited ? 
                    CreateCustomColor(200, 200, 240, 255) : // Azul muy claro
                    CreateCustomColor(255, 255, 255, 255);  // Blanco puro
            case CellType::WALL:
                return CreateCustomColor(120, 80, 80, 255);  // Marrón claro resaltado
            case CellType::START:
                return CreateCustomColor(150, 255, 150, 255); // Verde brillante
            case CellType::GOAL:
                return CreateCustomColor(255, 150, 150, 255); // Rojo brillante
            case CellType::ITEM:
                return CreateCustomColor(255, 255, 100, 255); // Amarillo brillante
            case CellType::GATE:
                return isCurrentlyOpen ? 
                    CreateCustomColor(100, 200, 255, 255) : // Azul brillante
                    CreateCustomColor(255, 100, 255, 255);  // Magenta brillante
            case CellType::TEMPORAL_WALL:
                return isCurrentlyOpen ? 
                    CreateCustomColor(255, 255, 255, 255) : // Blanco
                    CreateCustomColor(180, 180, 180, 255);  // Gris claro
            default:
                return CreateCustomColor(255, 255, 255, 255);
        }
    }
    
    
    switch (type) {
        case CellType::FREE:
            return isVisited ? 
                CreateCustomColor(180, 180, 200, 255) : // Gris azulado claro para visitado
                CreateCustomColor(240, 240, 240, 255);  // Casi blanco para libre
                
        case CellType::WALL:
            return CreateCustomColor(80, 50, 50, 255);   // MARRÓN OSCURO - muy distintivo
            
        case CellType::START:
            return CreateCustomColor(100, 200, 100, 255); // Verde medio
            
        case CellType::GOAL:
            return CreateCustomColor(200, 100, 100, 255); // Rojo medio
            
        case CellType::ITEM:
            return CreateCustomColor(255, 200, 50, 255);  // Dorado brillante
            
        case CellType::GATE:
            return isCurrentlyOpen ? 
                CreateCustomColor(80, 150, 255, 255) :  // Azul claro para abierta
                CreateCustomColor(180, 80, 180, 255);   // Morado para cerrada
                
        case CellType::TEMPORAL_WALL:
            return isCurrentlyOpen ? 
                CreateCustomColor(220, 220, 220, 255) : // Gris claro cuando abierta
                CreateCustomColor(140, 140, 140, 255);  // Gris medio cuando cerrada
                
        default:
            return CreateCustomColor(240, 240, 240, 255);
    }
}

void HexCell::Draw(float size) {
    
    Color color = GetCellColor(size);
    
    DrawHexagon(screenPos, size, color);
    
    
    float symbolSize = size * 0.6f;
    
    switch (type) {
        case CellType::ITEM:
    {
        // Círculo dorado simple y efectivo
        float gemRadius = symbolSize * 0.4f;
        
        // Sombra
        DrawCircle((int)(screenPos.x + 2), (int)(screenPos.y + 2), gemRadius + 2, 
                  CreateCustomColor(0, 0, 0, 100));
        
        // Círculo exterior dorado oscuro
        DrawCircle((int)screenPos.x, (int)screenPos.y, gemRadius + 2, 
                  CreateCustomColor(200, 150, 0, 255));
        
        // Círculo principal dorado
        DrawCircle((int)screenPos.x, (int)screenPos.y, gemRadius, 
                  CreateCustomColor(255, 215, 0, 255));
        
        // Círculo interior brillante
        DrawCircle((int)screenPos.x, (int)screenPos.y, gemRadius * 0.6f, 
                  CreateCustomColor(255, 255, 150, 255));
        
        // Punto de brillo
        DrawCircle((int)(screenPos.x - gemRadius * 0.3f), (int)(screenPos.y - gemRadius * 0.3f), 
                  2, CreateCustomColor(255, 255, 255, 255));
    }
    break;
            
        case CellType::START:
            {
                // Dibujar una flecha hacia arriba o punto de inicio
                DrawCircle((int)screenPos.x, (int)screenPos.y, symbolSize * 0.8f, 
                          CreateCustomColor(0, 150, 0, 255));
                DrawCircle((int)screenPos.x, (int)screenPos.y, symbolSize * 0.6f, 
                          CreateCustomColor(50, 200, 50, 255));
                DrawCircle((int)screenPos.x, (int)screenPos.y, symbolSize * 0.3f, 
                          CreateCustomColor(100, 255, 100, 255));
            }
            break;
            
        case CellType::GOAL:
            {
                
                DrawCircle((int)screenPos.x, (int)screenPos.y, symbolSize * 0.8f, 
                          CreateCustomColor(150, 0, 0, 255));
                
                
                float starSize = symbolSize * 0.5f;
                Color starColor = CreateCustomColor(255, 255, 100, 255);
                
                
                DrawLineEx(
                    {screenPos.x - starSize, screenPos.y}, 
                    {screenPos.x + starSize, screenPos.y}, 
                    3.0f, starColor
                );
                DrawLineEx(
                    {screenPos.x, screenPos.y - starSize}, 
                    {screenPos.x, screenPos.y + starSize}, 
                    3.0f, starColor
                );
                
               
                DrawLineEx(
                    {screenPos.x - starSize * 0.7f, screenPos.y - starSize * 0.7f}, 
                    {screenPos.x + starSize * 0.7f, screenPos.y + starSize * 0.7f}, 
                    2.0f, starColor
                );
                DrawLineEx(
                    {screenPos.x + starSize * 0.7f, screenPos.y - starSize * 0.7f}, 
                    {screenPos.x - starSize * 0.7f, screenPos.y + starSize * 0.7f}, 
                    2.0f, starColor
                );
            }
            break;
            
        case CellType::GATE:
            {
                // Dibujar compuerta con barras
                Color gateColor = isCurrentlyOpen ? 
                    CreateCustomColor(0, 100, 255, 255) : 
                    CreateCustomColor(255, 140, 0, 255); 
                
                if (isCurrentlyOpen) {
                    // Compuerta open: barras separadas
                    DrawRectangle(
                        (int)(screenPos.x - symbolSize * 0.6f), 
                        (int)(screenPos.y - symbolSize * 0.3f),
                        (int)(symbolSize * 0.4f), 
                        (int)(symbolSize * 0.6f), 
                        gateColor
                    );
                    DrawRectangle(
                        (int)(screenPos.x + symbolSize * 0.2f), 
                        (int)(screenPos.y - symbolSize * 0.3f),
                        (int)(symbolSize * 0.4f), 
                        (int)(symbolSize * 0.6f), 
                        gateColor
                    );
                } else {
                    // Compuerta close: barras juntas
                    DrawRectangle(
                        (int)(screenPos.x - symbolSize * 0.6f), 
                        (int)(screenPos.y - symbolSize * 0.1f),
                        (int)(symbolSize * 1.2f), 
                        (int)(symbolSize * 0.2f), 
                        gateColor
                    );
                    // X significa closed
                    DrawLineEx(
                        {screenPos.x - symbolSize * 0.3f, screenPos.y - symbolSize * 0.3f}, 
                        {screenPos.x + symbolSize * 0.3f, screenPos.y + symbolSize * 0.3f}, 
                        3.0f, CreateCustomColor(255, 0, 0, 255)
                    );
                    DrawLineEx(
                        {screenPos.x + symbolSize * 0.3f, screenPos.y - symbolSize * 0.3f}, 
                        {screenPos.x - symbolSize * 0.3f, screenPos.y + symbolSize * 0.3f}, 
                        3.0f, CreateCustomColor(255, 0, 0, 255)
                    );
                }
            }
            break;
            
        case CellType::TEMPORAL_WALL:
            {
                if (!isCurrentlyOpen) {
                    // Dibujar reloj
                    Color clockColor = CreateCustomColor(255, 203, 0, 255);
                    
                    // Círculo del reloj
                    DrawCircleLines((int)screenPos.x, (int)screenPos.y, symbolSize * 0.7f, clockColor);
                    DrawCircle((int)screenPos.x, (int)screenPos.y, 2, clockColor);
                    
                    // Manecillas del reloj
                    DrawLineEx(
                        screenPos, 
                        {screenPos.x, screenPos.y - symbolSize * 0.5f}, 
                        2.0f, clockColor
                    );
                    DrawLineEx(
                        screenPos, 
                        {screenPos.x + symbolSize * 0.3f, screenPos.y}, 
                        2.0f, clockColor
                    );
                    
                    // Mostrar número de turnos restantes
                    if (turnsToOpen > 0) {
                        DrawText(
                            TextFormat("%d", turnsToOpen), 
                            (int)(screenPos.x - 4), 
                            (int)(screenPos.y + symbolSize * 0.8f), 
                            12, 
                            CreateCustomColor(255, 255, 255, 255)
                        );
                    }
                }
            }
            break;
            
        case CellType::WALL:
            {
                // Dibujar patrón de ladrillos, indica que no se puede pasar
                Color brickColor = CreateCustomColor(255, 0, 0, 255);
                float brickSize = symbolSize * 0.5f;
                
                // Patrón de ladrillos 3x3
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 3; col++) {
                        float offsetX = (col - 1) * brickSize * 1.1f;
                        float offsetY = (row - 1) * brickSize * 1.1f;
                        
                        DrawRectangle(
                            (int)(screenPos.x + offsetX - brickSize * 0.4f),
                            (int)(screenPos.y + offsetY - brickSize * 0.3f),
                            (int)(brickSize * 0.8f),
                            (int)(brickSize * 0.6f),
                            brickColor
                        );
                    }
                }
            }
            break;
            
        default:
            break;
    }
    
    // Indicador de visitado más sutil
    if (isVisited && type == CellType::FREE) {
        DrawCircle((int)(screenPos.x + size * 0.6f), (int)(screenPos.y - size * 0.6f), 3, 
                  CreateCustomColor(100, 150, 200, 150));
    }
}

bool HexCell::IsPointInside(Vector2 point, float size) {
    Vector2 center = screenPos;
    float dx = fabsf(point.x - center.x);
    float dy = fabsf(point.y - center.y);
    
    float distance = sqrtf(dx * dx + dy * dy);
    return distance <= size * 0.9f;
}
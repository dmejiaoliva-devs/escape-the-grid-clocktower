// FileLoader.cpp, esto es para cargar los niveles desde los .txt
#include "FileLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool FileLoader::LoadLevel(const std::string& filename, LevelData& levelData) {
    // Solo aguantan .txt por ahora
    if (filename.find(".txt") != std::string::npos) {
        return LoadFromTXT(filename, levelData);
    }
    
    std::cout << "Solo se soportan archivos .txt en esta versión." << std::endl;
    return false;
}

bool FileLoader::LoadFromTXT(const std::string& filename, LevelData& levelData) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    
    try {
        // dimensiones del nivel
        if (!std::getline(file, line)) {
            std::cout << "Error leyendo dimensiones" << std::endl;
            return false;
        }
        std::istringstream dimStream(line);
        if (!(dimStream >> levelData.width >> levelData.height)) {
            std::cout << "Error parseando dimensiones: " << line << std::endl;
            return false;
        }
        
        // posición inicial
        if (!std::getline(file, line)) {
            std::cout << "Error leyendo posición inicial" << std::endl;
            return false;
        }
        std::istringstream startStream(line);
        if (!(startStream >> levelData.startX >> levelData.startY)) {
            std::cout << "Error parseando posición inicial: " << line << std::endl;
            return false;
        }
        
        // posición objetivo
        if (!std::getline(file, line)) {
            std::cout << "Error leyendo posición objetivo" << std::endl;
            return false;
        }
        std::istringstream goalStream(line);
        if (!(goalStream >> levelData.goalX >> levelData.goalY)) {
            std::cout << "Error parseando posición objetivo: " << line << std::endl;
            return false;
        }
        
        // ciclo de turnos
        if (!std::getline(file, line)) {
            std::cout << "Error leyendo ciclo de turnos" << std::endl;
            return false;
        }
        levelData.turnCycleLength = std::stoi(line);
        
        // Inicializar grid
        levelData.cellTypes.resize(levelData.height);
        for (int y = 0; y < levelData.height; y++) {
            levelData.cellTypes[y].resize(levelData.width, '.');
        }
        
        // Leer el mapa
        for (int y = 0; y < levelData.height; y++) {
            if (!std::getline(file, line)) {
                std::cout << "Error leyendo fila " << y << " del mapa" << std::endl;
                return false;
            }
            
            int x = 0;
            for (char c : line) {
                if (c != ' ' && x < levelData.width) {
                    levelData.cellTypes[y][x] = ParseCellChar(c);
                    
                    // celdas especiales
                    if (c == 'K') {
                        levelData.items.push_back({x, y});
                        levelData.cellTypes[y][x] = '.'; 
                    }
                    
                    x++;
                }
            }
        }
        
        // Leer configuraciones especiales
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            if (line.find("GATE_") == 0) {
                // Formato: GATE_A 11001100
                std::istringstream gateStream(line);
                std::string gateName, patternStr;
                if (gateStream >> gateName >> patternStr) {
                    std::vector<bool> pattern;
                    for (char c : patternStr) {
                        pattern.push_back(c == '1');
                    }
                    levelData.gatePatterns[gateName.substr(5)] = pattern; // Quitar "GATE_"
                    std::cout << "Patrón de compuerta cargado: " << gateName.substr(5) << std::endl;
                }
            } else if (line.find("ASSIGN_") == 0) {
                // Formato: ASSIGN_3_4_A
                std::istringstream assignStream(line);
                std::string assignCmd;
                if (assignStream >> assignCmd) {
                    // Parsear ASSIGN_x_y_pattern
                    size_t pos1 = assignCmd.find('_', 7);
                    size_t pos2 = assignCmd.find('_', pos1 + 1);
                    
                    if (pos1 != std::string::npos && pos2 != std::string::npos) {
                        int x = std::stoi(assignCmd.substr(7, pos1 - 7));
                        int y = std::stoi(assignCmd.substr(pos1 + 1, pos2 - pos1 - 1));
                        std::string pattern = assignCmd.substr(pos2 + 1);
                        
                        levelData.gateAssignments[{x, y}] = pattern;
                        levelData.cellTypes[y][x] = 'G'; // Marcar como compuerta
                        std::cout << "Compuerta asignada en (" << x << ", " << y << ") patrón: " << pattern << std::endl;
                    }
                }
            } else if (line.find("TEMPORAL_") == 0) {
                // Formato: TEMPORAL_5_3_10
                std::istringstream tempStream(line);
                std::string tempCmd;
                if (tempStream >> tempCmd) {
                    size_t pos1 = tempCmd.find('_', 9);
                    size_t pos2 = tempCmd.find('_', pos1 + 1);
                    
                    if (pos1 != std::string::npos && pos2 != std::string::npos) {
                        int x = std::stoi(tempCmd.substr(9, pos1 - 9));
                        int y = std::stoi(tempCmd.substr(pos1 + 1, pos2 - pos1 - 1));
                        int turns = std::stoi(tempCmd.substr(pos2 + 1));
                        
                        levelData.temporalWalls[{x, y}] = turns;
                        levelData.cellTypes[y][x] = 'T'; // Marcar como pared temporal
                        std::cout << "Pared temporal en (" << x << ", " << y << ") se abre en turno: " << turns << std::endl;
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error parseando archivo: " << e.what() << std::endl;
        file.close();
        return false;
    }
    
    file.close();
    std::cout << "Nivel cargado exitosamente: " << filename << std::endl;
    std::cout << "Dimensiones: " << levelData.width << "x" << levelData.height << std::endl;
    std::cout << "Inicio: (" << levelData.startX << ", " << levelData.startY << ")" << std::endl;
    std::cout << "Meta: (" << levelData.goalX << ", " << levelData.goalY << ")" << std::endl;
    
    return true;
}

char FileLoader::ParseCellChar(char c) {
    switch (c) {
        case 'S': return 'S';  // Start
        case 'G': return 'G';  // Goal
        case '#': return '#';  // Wall
        case '.': return '.';  // Free
        case 'K': return 'K';  // Item
        case 'O': return 'G';  // Gate (open por defecto)
        case 'X': return 'G';  // Gate (closed por defecto)
        case 'T': return 'T';  // Muro temporal
        default:  return '.';  // Default to free
    }
}

// Función placeholder para JSON (no implementada), mala practica ya que no lo ocupo XD
bool FileLoader::LoadFromJSON(const std::string& filename, LevelData& levelData) {
    std::cout << "Carga JSON no implementada. Use archivos .txt" << std::endl;
    return false;
}
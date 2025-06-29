#pragma once
#include <string>
#include <vector>
#include <map>

struct LevelData {
    int width, height;
    int startX, startY;
    int goalX, goalY;
    std::vector<std::vector<char>> cellTypes;
    std::vector<std::pair<int, int>> items;
    std::map<std::string, std::vector<bool>> gatePatterns;
    std::map<std::pair<int, int>, std::string> gateAssignments;
    std::map<std::pair<int, int>, int> temporalWalls;
    int turnCycleLength;
};

class FileLoader {
public:
    static bool LoadLevel(const std::string& filename, LevelData& levelData);
    static bool LoadFromJSON(const std::string& filename, LevelData& levelData);
    static bool LoadFromTXT(const std::string& filename, LevelData& levelData);
    
private:
    static char ParseCellChar(char c);
};

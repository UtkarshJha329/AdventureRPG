#pragma once

#include <string>
#include <vector>
#include <fstream>

class TileMapData {

public:

	std::vector<std::string> tileMapData;

	std::vector<std::string> enemyTileMapData;
};

void FillTileMapDataFromFile(TileMapData& tmd) {

    for (int y = 0; y < totalRoomsY; y++)
    {
        for (int x = 0; x < totalRoomsX; x++)
        {
            {
                std::string tileMapDataLoc = roomsTileMapDataFileLoc + "roomMapData.map";

                std::ifstream file(tileMapDataLoc);

                if (!file.is_open()) {
                    std::cerr << "Error opening file: " << tileMapDataLoc << std::endl;
                    return;
                }

                std::string line;

                while (std::getline(file, line)) {

                    tmd.tileMapData.push_back(line);
                }

                file.close();
            }

            {
                std::string enemyTileMapDataLoc = roomsTileMapDataFileLoc + "roomEnemyMapData.map";

                std::ifstream file(enemyTileMapDataLoc);

                if (!file.is_open()) {
                    std::cerr << "Error opening file: " << enemyTileMapDataLoc << std::endl;
                    return;
                }

                std::string line;

                while (std::getline(file, line)) {

                    tmd.enemyTileMapData.push_back(line);
                }

                file.close();
            }
        }
    }
}
#pragma once
#include <string>
#include <vector>

#include "Tools/nlohmann/json.hpp"

struct SpawnAreaData
{
    std::string name;   // スポーンエリアの種類
    std::vector<std::string> enemyTypes;    // わく敵の種類
};

// 代入を簡略化するマクロ
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SpawnAreaData, name, enemyTypes
)

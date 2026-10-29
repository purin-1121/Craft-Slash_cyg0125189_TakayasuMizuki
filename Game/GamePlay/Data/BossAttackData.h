#pragma once

#include <iostream>

#include "Tools/nlohmann/json.hpp"

struct BossAttackData
{
    std::string attackID = "None";  // 行動ID
    float maxHpRate = 1.0f;         // この行動ができる最大HP割合
    float minHpRate = 1.0f;         // この行動ができる最低HP割合
    float minDistance = 0.0f;       // 敵との最小距離
    float maxDistance = 0.0f;       // 敵との最大距離
    int priority = 0;               // 優先順位（高いほど）
    bool isOnce = false;            // 一度きりの行動か
};

// 代入を簡略化するマクロ
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    BossAttackData, attackID,maxHpRate, minHpRate,minDistance, maxDistance,priority,isOnce
)

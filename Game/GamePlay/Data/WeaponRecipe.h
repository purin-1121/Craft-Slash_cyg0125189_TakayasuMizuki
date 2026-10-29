//====================================================//
// ファイル名   : WeaponRecipe.h
// 作成者       : 高安瑞貴
// 作成日時     : 2026/10/27 7:45:09
//
// 概要 : 
//
// 更新履歴 :
// 2026/10/27 7:45:09 新規作成
//====================================================//

#pragma once

#include "Tools/nlohmann/json.hpp"

struct RequiredMaterial
{
    uint8_t type;
    int num;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    RequiredMaterial,
    type,
    num
)

// 進化レシピの形
struct WeaponRecipe
{
    std::string recipeName;        // レシピ名（デバッグ用）
    int priorityLevel;            // 優先度（数字が大きいほど上位の武器）
    //std::string resultWeaponType;  // 条件達成時に変化する武器名

    uint8_t requiestMask = 0;   // 必要な素材のビットマスク（一次判定用）
    // 各素材が「最低何個以上必要か」の閾値
    std::vector<RequiredMaterial> requiredMaterials;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    WeaponRecipe, recipeName, priorityLevel, requiestMask, requiredMaterials)

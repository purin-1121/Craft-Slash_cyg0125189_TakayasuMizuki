#pragma once

#include <string>
#include <vector>

#include "Tools/nlohmann/json.hpp"
#include "Common/Collider/Collider.h"
#include "Game/GamePlay/Actor/ActorData.h"

struct EnemyData
{
    std::string id = "-1";                          // 識別番号
    std::string name = "None";                      // モンスターの名前
    std::string weaponName = "Sword";               // 武器の名前
    float maxHp = -100;                             // 最大体力
    float attackDistance = 0.0f;                    // アタック行動に入るまでの距離
    float maxSpeed = 0.0f;                          // 最大速度
    float maxForce = 0.0f;                          // 最大
    float attackDuration = 0.0f;                    // アタック時間
    float noticeTime = 0.0f;                        // 発見モーションをする時間
    int power = -10;                                // 攻撃力
    int defense = 1;                                // 防御力
    std::vector<ColliderData> colliders{};          // コライダー
    std::vector<StateID> stateIDs{StateID::Idle};   // この敵が使用するステート列挙型のリスト
};

// 代入を簡略化するマクロ
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    EnemyData, id, name, weaponName, maxHp, attackDistance, 
    maxSpeed, maxForce, attackDuration, 
    power, defense, noticeTime, colliders, stateIDs
)

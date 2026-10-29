#pragma once
#include "pch.h"
#include "Tools/JsonConverter.h"
#include "Game/GamePlay/Item/InventryData.h"
#include "Common/Collider/Collider.h"
#include "Message/EventBus.h"
#include <string_view>

enum class AttackType { Melee, Ranged };

NLOHMANN_JSON_SERIALIZE_ENUM(AttackType, {
    {AttackType::Melee, "Melee"},
    {AttackType::Ranged,"Ranged"},
    })

struct Transform
{
    SimpleMath::Vector3 pos = SimpleMath::Vector3::Zero;
    SimpleMath::Quaternion rot = SimpleMath::Quaternion::Identity;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Transform,
    pos,
    rot
)

// 武器の基本情報
struct WeaponData {
    std::string name = "Unknown";
    AttackType attackType = AttackType::Melee;
    std::string bulletName = "UnKnown";
    float coolTime = 1.0f;
    float power = 5.0f;
    float knockBack = 1.0f;
    int durability = 10;
    bool isHoldInputEnabled = false;
    
    std::string modelName = "Sword";
    std::vector<ColliderData> colliders{};

    // 持ち手の位置と回転オフセット
    std::unordered_map<std::string, Transform> offsets{};
};


// 代入を簡略化するマクロ
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    WeaponData, name, attackType, bulletName, coolTime, power, knockBack, 
    durability, isHoldInputEnabled, modelName, colliders, offsets
)

// JSONの1ステップ分（フレームごとの座標など）を入れる構造体
struct AttackStep 
{
    float time;               // このステップまでの時間
    SimpleMath::Vector3 start; // 攻撃し始めの位置
    SimpleMath::Vector3 end;  // 攻撃終了の位置
};

// 武器の種類
enum class WeaponType
{
    Sword,      // 片手剣
    Hammer,     // ハンマー
    GreatSword, // 大剣
    Max
};

// ステップ間の補完方法
enum class AnimComplemate
{
    Linear, // 線形 
    Slerp,  // 球形
    EaseIn, // イーズイン
    EaseOut,// イーズアウト
};

static int StrningToWeaponType(std::string type)
{
    if (type == "Sword") return (int)WeaponType::Sword;
    else if (type == "Hammer") return (int)WeaponType::Hammer;
    else if (type == "GreatSword") return (int)WeaponType::GreatSword;
    else return (int)WeaponType::Sword;
}

static constexpr std::string_view INIT_WEAPON = "Sword";


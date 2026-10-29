#pragma once

// Pythonによる自動生成ファイル。直接編集しないこと。

#include "Game/GamePlay/Data/BossAttackData.h"
#include "Game/GamePlay/Data/BreakableObjectData.h"
#include "Game/GamePlay/Data/BulletData.h"
#include "Game/GamePlay/Data/EnemyData.h"
#include "Game/GamePlay/Data/SpawnAreaData.h"
#include "Game/GamePlay/Data/WeaponData.h"

template<class T>
struct JsonDataPath;

template<>
struct JsonDataPath<BossAttackData>
{
    static constexpr const char* value = "Resources/Jsons/BossAttackData.json";
};

template<>
struct JsonDataPath<BreakableObjectData>
{
    static constexpr const char* value = "Resources/Jsons/BreakableObjectData.json";
};

template<>
struct JsonDataPath<BulletData>
{
    static constexpr const char* value = "Resources/Jsons/BulletData.json";
};

template<>
struct JsonDataPath<EnemyData>
{
    static constexpr const char* value = "Resources/Jsons/EnemyData.json";
};

template<>
struct JsonDataPath<SpawnAreaData>
{
    static constexpr const char* value = "Resources/Jsons/SpawnAreaData.json";
};

template<>
struct JsonDataPath<WeaponData>
{
    static constexpr const char* value = "Resources/Jsons/WeaponData.json";
};

template struct JsonDataPath<BossAttackData>;
template struct JsonDataPath<BreakableObjectData>;
template struct JsonDataPath<BulletData>;
template struct JsonDataPath<EnemyData>;
template struct JsonDataPath<SpawnAreaData>;
template struct JsonDataPath<WeaponData>;

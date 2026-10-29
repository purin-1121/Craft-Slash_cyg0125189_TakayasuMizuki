#pragma once

// Pythonによる自動生成ファイル。直接編集しないこと。

class BossAttack;
class BreakableObject;
class Bullet;
class Enemy;
class SpawnArea;

template<class T>
struct ObjectDataType;

template<>
struct ObjectDataType<BossAttack>
{
    using Type = BossAttackData;
};

template<>
struct ObjectDataType<BreakableObject>
{
    using Type = BreakableObjectData;
};

template<>
struct ObjectDataType<Bullet>
{
    using Type = BulletData;
};

template<>
struct ObjectDataType<Enemy>
{
    using Type = EnemyData;
};

template<>
struct ObjectDataType<SpawnArea>
{
    using Type = SpawnAreaData;
};

//====================================================//
// ファイル名   : Enemy.h
// 作成者       : 高安瑞貴
//
// 概要 : 敵クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Game/GamePlay/Actor/Actor.h"
#include "Game/GamePlay/Item/InventryData.h"
#include "Game/GamePlay/Item/DropData.h"
#include "Game/GamePlay/Blocks/Drop.h"
#include "Game/GamePlay/Data/EnemyData.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyIdle.h"

//====================================================//
// 宣言
//====================================================//

class EnemyIdle;
class Player;

enum class EnemyType : char
{
	Slime,
	Boss,
};

class Enemy : public Actor, public Drop
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="playerPos">プレイヤーの位置</param>
	/// <param name="pos">初期座標</param>
	Enemy(Player* player, SimpleMath::Vector3 pos, int id, const EnemyData& data);

	// ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view) override;
	void RenderText() override;
	void Finalize() override;

	// 待機状態を取得
	IGameObjectState* GetIdleState() override { return m_stateMachine.GetState<EnemyIdle>(); };
	// プレイヤーを取得
	Player* GetPlayer() { return m_targetPlayer; }
	// プレイヤーの位置を取得
	SimpleMath::Vector3 GetPlayerPos() const;
	// プレイヤーが探索範囲にいるか
	bool IsPlayerInSearchArea();
	// 敵の情報を取得
	const EnemyData* GetData() { return m_myData; }
	// 死亡関数を実行
	void PlayDieCallback() { m_dieCallback(this, this, MIN_DROP_DIE, MAX_DROP_DIE); }
	// 生きてるか
	bool IsAlive() { return !m_isStop; };
	// 生まれた場所のID取得
	int GetID() { return m_id; }
	// 攻撃力の参照
	int& GetDamage() { return m_buffManager.Calculate(m_myData->power, BuffManager::StatesEffectType::Power); }
	// バフマネージャの取得
	BuffManager& GetBuffManager() { return m_buffManager; }
	// 使用している武器を消去
	void DisposeWeapon();

	// ---- 武器関連 ---- //
	IWeapon& GetWeapon() { return *m_weapons->GetCurrentWeapon(); }
	WeaponManager& GetWeaponManager() { return *m_weapons; }
	const std::string& GetIdleAnimationName() const override { return m_idleAnimationName; }

	// ---- 攻撃処理用 ---- //
	void AttackOn();
	void AttackOff();

	// ------ Other ------ //
	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;
	void OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj) override;
public:
	
	// 死んだときにアイテムを落とす最大個数と最低個数
	static constexpr int MAX_DROP_DIE = 20;
	static constexpr int MIN_DROP_DIE = 10;

private:

	// この敵のデータ
	const EnemyData* m_myData;
	const std::string m_idleAnimationName;

	// ---- ステート管理 ---- //
	// 移動
	// ジャンプ
	// 攻撃

	// ---- 当たり判定 ---- //
	std::vector<Collider*> bodys;
	Collider* playerSearch;
	std::vector<Collider*> m_attackColliders;
	// 今接触中の相手(武器など)と、その接触コライダー数
	std::unordered_map<GameObject*, int> m_hitCount;
	// プレイヤーの位置ポインタ
	Player* m_targetPlayer;
	// 生まれた場所のID
	int m_id;

	// ---- デバッグ用 ---- //

private:

	// ステート作成
	void CreateState();
	// コライダー作成
	void CreateColldiers(const EnemyData& data);

	
};


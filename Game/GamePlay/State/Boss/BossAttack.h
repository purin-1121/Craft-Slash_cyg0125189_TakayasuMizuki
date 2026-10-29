#pragma once
#include "BossState.h"
#include "BossAttack/SlashAttack.h"
#include "BossAttack/ChargeAttack.h"
#include "BossAttack/DashAttack.h"
#include "Game/GamePlay/Data/BossAttackData.h"
#include <unordered_set>

class Enemy;

// ひるみ状態
class BossAttack : public BossState
{
public:

	BossAttack(Enemy* owner, const std::vector<BossAttackData>& data);
	~BossAttack() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void Exit() override;

	virtual bool CanChanged() override { 
		return dynamic_cast<ChargeAttack*>(m_currentAttack) == nullptr &&
			   dynamic_cast<DashAttack*>(m_currentAttack) == nullptr
		; 
	};

	BossAttackBase* SelectAttack();

private:

	// 攻撃インスタンス
	std::unordered_map<std::string, std::unique_ptr<BossAttackBase>> m_attacks;
	// 攻撃分木のデータ
	std::vector<BossAttackData> m_data;
	// チャージ中か
	bool m_isCharge = false;
	// 一度きりの行動を実行したリスト
	std::unordered_set<std::string> m_executeAttacks;


	// 現在実行している攻撃
	BossAttackBase* m_currentAttack = nullptr;

	// 攻撃ポインタを返す
	BossAttackBase* GetAttack(const std::string& id);
	// 攻撃のインスタンスを作成
	inline void CreateAttackMethod();

};


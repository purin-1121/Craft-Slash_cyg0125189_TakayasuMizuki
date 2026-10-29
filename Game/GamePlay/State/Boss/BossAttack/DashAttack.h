#pragma once
#include "BossAttackBase.h"

class Enemy;

// タックル行動
class DashAttack : public BossAttackBase
{
public:

	DashAttack(Enemy* owner);
	~DashAttack() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

	// 突進する方向ベクトル
	SimpleMath::Vector3 m_targetVec;
	// 開始地点から目的地までの距離
	SimpleMath::Vector3 m_distance;
	// 減速し始める位置
	SimpleMath::Vector3 m_deceleration;
	// 突進するときのスピード
	static constexpr float DASH_SPEED = 2.0f;

};



#pragma once
#include "BossState.h"
#include "Game/GamePlay/Enemy/Enemy.h"

class Boss;

// プレイヤー発見状態
class BossNotice : public BossState
{
public:

	BossNotice(Enemy* owner);
	~BossNotice() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return (m_owner->GetStateMachine().GetTimer() > m_owner->GetData()->noticeTime); };

private:

	// 発見時のプレイヤーの位置
	SimpleMath::Vector3 m_playerPos;
	// プレイヤーへの方向ベクトル
	SimpleMath::Vector3 m_direction;
	// すでにプレイヤーを見つけたかどうか
	bool m_found;

};


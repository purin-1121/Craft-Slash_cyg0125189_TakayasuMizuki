#pragma once
#include "Game/GamePlay/State/Boss/BossState.h"
class Enemy;

// ひるみ状態
class BossAttackBase : public BossState
{
public:

	BossAttackBase(Enemy* owner);
	~BossAttackBase() override;

	virtual void Enter() override;
	virtual void Update(const GameContext& gameContext, float elapsedTime) override;
	virtual void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	virtual void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	virtual void Exit() override;

	virtual bool CanChanged() override { return true; };

	// 攻撃が終了したか
	bool IsFinished() const
	{
		return m_isFinished;
	}

protected:

	// 派生クラスから攻撃終了を設定する
	void Finish()
	{
		m_isFinished = true;
	}

	// 攻撃開始時にリセットする
	void ResetFinished()
	{
		m_isFinished = false;
	}

	void AddTime(float elapsedTime) { m_time += elapsedTime; }
	float GetTime() { return m_time; }
	void ResetTime() { m_time = 0; }

private:

	bool m_isFinished = false;
	// 計測用
	float m_time;

};


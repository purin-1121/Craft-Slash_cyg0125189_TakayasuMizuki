#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Game/GamePlay/GameObject/GameObject.h"

class Goal : public GameObject
{

public:

	Direction d;
	Goal(SimpleMath::Vector3 pos, std::function<void()> goal);

	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view) override;
	void RenderText() override;
	void Finalize() override;

	// ------ Get ------ //
	bool IsActive() override { return m_isHidden; };	// アクティブフラグ

	// ------ Set ------ //
	void SetActive(bool active) { m_isHidden = active; };

	// ------ Add ------ //

	// ------ Other ------ //
	// 攻撃処理
	void OnTrigger(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;


private:

	// ---- 基本のパラメータ ---- //
	// 表示されるか
	bool m_isHidden;
	// ゴールしたときのコールバック関数
	std::function<void()> m_goalCallBack;
	// もうゴールしたか
	bool m_isGoaled;

	// ---- 衝突判定 ---- //
	Collider* body;

	// ---- デバッグ用 ---- //
};


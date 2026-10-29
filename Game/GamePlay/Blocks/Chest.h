#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Game/GamePlay/GameObject/BreakableObject.h"
#include "InterFace/IInputObserver.h"

class Chest : public IInputObserver, public GameObject, public Drop
{
public:

	Chest(SimpleMath::Vector3 pos, TypeInfo* info, InputGroup* group);

	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view) override;
	void RenderText() override;
	void Finalize() override;

	// ------ Get ------ //
	bool IsAlive() { return true; }	// 生きているかどうか
	bool IsActive() override { return IsAlive(); };	// アクティブフラグ

	// ------ Set ------ //
	void SetCallBack(std::function<void(GameObject*, Drop*, int, int)> f) { m_opendCallBack = f; }


	// ------ Add ------ //

	// ------ Other ------ //n
	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;
	void OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)   override;
	bool OnInputAccept(Message::InputEvent input) override;

private:

	// ---- 基本のパラメータ ---- //
	bool m_isOpend;	// 開封済みか
	bool m_canOpend;	// 開封できるか 

	std::function<void(GameObject*, Drop*, int, int)> m_opendCallBack;

	static constexpr SimpleMath::Vector3 SCALE{0.5f, 0.5f, 0.5f};

	// ---- 当たり判定 ---- //
	Collider* body;
	Collider* search;

	static constexpr SimpleMath::Vector3 bodySize = { 0.6f, 0.4f, 0.2f };
	static constexpr float searchSize = 0.5f;

	// ---- デバッグ用 ---- //

private:

	void DropItem();


};


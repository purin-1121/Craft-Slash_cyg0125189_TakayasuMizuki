//====================================================//
// ファイル名   : Player.h
// 作成者       : 高安瑞貴
//
// 概要 : プレイヤークラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Actor.h"
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "InterFace/IGameObjectState.h"
#include "Message/InputGroup.h"
#include "Game/GamePlay/Item/InventryData.h"
#include "Game/GamePlay/Weapon/WeaponManager.h"
#include "Game/GamePlay/Weapon/EvolutionManager.h"
#include "Game/GamePlay/State/PlayerState/PlayerIdle.h"
#include "Game/GamePlay/State/PlayerState/PlayerWolk.h"
#include "Game/GamePlay/State/PlayerState/PlayerJump.h"
#include "Game/GamePlay/State/PlayerState/PlayerAttack.h"


//====================================================//
// 宣言
//====================================================//

class InputGroup;
class Camera;

class Player : public InputGroup, public Actor
{

public:
	// コンストラクタ
	Player(InputGroup* group);

	// ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view) override;
	void RenderText() override;
	void Finalize() override;

	// ------ Get ------ //
	int GetItem(uint8_t type);	// 指定の種類のアイテムが何個あるか
	bool PressedJump() { return m_isJump; } // ジャンプボタンが押されているか
	bool IsAttacking() override { return m_stateMachine.IsState<PlayerAttack>(); }
	std::array<InventryData::Slot, InventryData::MAX_SLOT>& GetSlots() {return m_inventry;}
	IGameObjectState* GetIdleState() override { return GetStateMachine().GetState<PlayerIdle>(); }
	const std::string& GetIdleAnimationName() const override { static std::string name = "PlayerIdle";return name; };
	
	void SetHold(bool is) { m_holding = is; }
	bool Holding() { return m_holding; }


	// ------ Add ------ //

	// ------ Set ------ //
	void SetJumped(bool jump) { m_isJump = jump; }

	// ------ Other ------ //
	void ProcessAttack(); // 攻撃判定の設定
	bool CollectItem(uint8_t type);
	bool OnInputAccept(Message::InputEvent input) override;
	
	// ------ 衝突処理 ------ //
	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;
	void OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)  override;
	void OnCollision(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;
	// 移動
	void Move(SimpleMath::Vector3 vec);

public:

	// 速度
	static constexpr float SPEED = 1.0f;
	// 最大HP
	static constexpr int MAX_HP = 100;
	// 初期位置
	static constexpr SimpleMath::Vector3 START_POS = { 0,0,0 };
	static constexpr SimpleMath::Vector3 START_LEG_POS = { 0,0.1f,0 };
	// プレイヤーの最大移動ベクトル
	static constexpr float MAX_SPEED = 2.0f;

private:

	// ---- 武器・アイテム関連 ---- //
	// 各アイテムの所持数
	std::array <InventryData::Slot, InventryData::MAX_SLOT> m_inventry;
	// 進化マネージャ
	EvolutionManager m_evolutionManager;
	// ジャンプボタンを押してるかどうか
	bool m_isJump;
	// 武器を持っているか
	bool m_holding;

	// ---- 当たり判定 ---- //
	Collider* body;	// 体
	Collider* leg;	// 足
	
	// ---- デバッグ用 ---- //

	// ---- メンバ関数 ---- //
private:

	// ステート作成
	void CreateState();


};


//--------------------------------------------------------------------------------------
// File: Item.h
//
// 武器の素材になるクラス
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------

// ------ このクラスの流れ ------ //


#pragma once
#include "Game/GamePlay/GameObject/GameObject.h"
#include "InventryData.h"

struct ItemSetting;
// アイテムのステート
enum class ItemState : unsigned int
{
	Drop,	// 散らばり中
	Idle,	// 静止中
	Action,	// 追尾中
};

class Item : public GameObject
{

public:

	// ------ コンストラクタ ------ //
	Item();
	Item(int index,
		SimpleMath::Vector3 pos,
		SimpleMath::Vector3 vec,
		ItemSetting* setting
	);

	~Item() override;

	// ------ 基本関数 ------ //
	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera)  override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view)  override;
	void RenderText()  override;
	void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)override;
	void Finalize()  override;

	// アイテムの種類
	uint8_t GetType();
	// コードを実行していいかか
	bool IsActive() override { return m_active; }
	void SetActive(bool active) { m_active = active; }
	// 生成処理
	void Create(SimpleMath::Vector3 pos, ItemSetting* pool, SimpleMath::Vector3 vec = SimpleMath::Vector3::Zero);
	// 識別番号取得
	int GetIndex() { return m_index; }
	// 状態変化
	void ChangeState(ItemState state) { m_state = state; }
	// トリガー
	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;

private:

	// ---- 定数 ---- //
	ItemState m_state;
	// アイテムの情報ポインタ
	const ItemSetting* m_setting;
	// アクティブフラグ
	bool m_active;
	// 識別番号
	int m_index;
	// 生成されてからの時間
	float m_aliveTime;

	// ---- 当たり判定 ---- //
	Collider* body;

};





//--------------------------------------------------------------------------------------
// File: MeleeWeapon.h
//
// 武器のクラス
//
// Date: 2026.6.10
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------

#pragma once
#include "Game/GamePlay/GameObject/GameObject.h"
#include "INterFace/IWeapon.h"

class MeleeWeapon : public GameObject, public IWeapon
{

public:

	// ------ コンストラクタ ------ //
	MeleeWeapon(WeaponData* data);

	~MeleeWeapon() override;

	// ------ 基本関数 ------ //
	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera)  override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view)  override;
	void RenderText()  override;
	void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)override;
	void Finalize()  override;

	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* obj, const CollisionInfo& info) override;

	// コードを実行していいかか
	bool IsActive() override { return m_active; }
	void SetActive(bool active) { m_active = active; }
	// 識別番号取得
	std::string_view GetName() const { return m_data->name; }
	// 攻撃命令
	void AttackOn(ColliderTag tag);
	void AttackOff() override;
	// 攻撃中かどうか
	bool IsAttackOn() const { return m_attack; }
	// 攻撃力を取得
	int GetDamage() const { return m_data->power; }
	void SetOwner(GameObject* owner) override { m_owner = owner; } ;
	// データ取得
	const WeaponData* GetData() const override { return m_data; }

	// 装着位置(手のボーン行列など)を武器に伝える共通の窓口
	void SetAttachTransform(const SimpleMath::Matrix& m) override { SetWorldMatrix(m) ; }
	float GetCoolTime() { return m_coolTime; };

	// 持ち手オフセットの取得
	SimpleMath::Matrix GetHoldMatrix() override {
		return
			SimpleMath::Matrix::CreateFromQuaternion(m_data->offsets.at(m_stateName).rot) *
			SimpleMath::Matrix::CreateTranslation(m_data->offsets.at(m_stateName).pos);
	};

	// オフセットの種類変更
	void SetOffsetOfState(std::string state) { 
		// 見つかったら適応
		if(m_data->offsets.find(state) != m_data->offsets.end())
			m_stateName = state; 
	}

	void UpdateCollider() override;

private:

	// ---- 変数 ---- //
	// アクティブフラグ
	bool m_active;
	// 攻撃処理の実行フラグ
	bool m_attack;
	// 攻撃処理の時間
	float m_attackTime;
	// クールタイム
	float m_coolTime;

	// ---- 基本パラメータ ---- //
	// この武器の情報
	const WeaponData* m_data;
	// 初期位置
	SimpleMath::Vector3 m_initPosition;
	// アニメーションステップ終了位置
	SimpleMath::Vector3 m_animStepEndPos;
	// 対象オフセット
	std::string m_stateName;

	GameObject* m_owner;

};




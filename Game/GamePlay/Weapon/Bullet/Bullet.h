#pragma once

#include "Game/GamePlay/GameObject/GameObject.h"
#include "Game/GamePlay/Data/BulletData.h"
#include "InterFace/IDamageDealer.h"

class Bullet : public GameObject, public IDamageDealer
{
public:

	// ------ コンストラクタ ------ //
	Bullet();

	~Bullet() override;

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
	// 攻撃命令
	void AttackOn(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, BulletData* data, ColliderTag tag);
	// 攻撃力を取得
	int GetDamage() const override { return m_bulletData->power; }

	// 着弾後の爆発時間
	static constexpr float IMPACT_TIME = 1.0f;
	// 飛行可能時間
	static constexpr float FLY_TIME = 1.7f;

private:

	// ---- 基本パラメータ ---- //
	// 生存時間
	float m_aliveTime;
	// 生存中か
	bool m_active;
	// 着弾後か
	bool m_afterImpact;
	// 飛行時間
	float m_flyTime;

	// 弾丸のデータ
	BulletData* m_bulletData;

	// 砲弾コライダー
	Collider* body;
	// 爆破コライダー
	Collider* bomb;

	void Bomb();
};



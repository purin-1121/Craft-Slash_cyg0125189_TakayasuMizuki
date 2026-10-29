//--------------------------------------------------------------------------------------
// File: BreakableObject.h
//
// 破壊可能なオブジェクト
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "GameObject.h"
#include "Game/GamePlay/Blocks/Drop.h"
#include "Game/GamePlay/Data/BreakableObjectData.h"
class Camera;

class BreakableObject : public GameObject, public Drop
{

public:

	// ------ コンストラクタ ------ //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期位置</param>
	/// <param name="vec">回転</param>
	/// <param name="collider">コライダー</param>
	/// <param name="info">ドロップ情報</param>
	/// <param name="kinematic">静的か否か</param>
	BreakableObject(
		SimpleMath::Vector3 pos,
		SimpleMath::Vector3 vec,
		TypeInfo* info,
		BreakableObjectData* data
	);

	~BreakableObject() = default;

	// ------ 基本関数 ------ //
	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view) override;
	void RenderText() override;
	void Finalize() override;
	bool IsActive() override { return !m_isBreaked; };

	void CreateWorldMatrix();

	// ------ Get ------ //
	bool IsBreaked() { return m_isBreaked; };
	const BreakableObjectData& GetData() { return *m_data; }

	// ------ Set ------ //
	void SetDropCallBack(std::function<void(GameObject*, const Drop*, int, int)> f) { m_breakCallBack = f; }
	void SetData(BreakableObjectData* data) { m_data = data; }

	// ------ Add ------ //

	// ------ 処理 ------ //
	// 攻撃判定と接触したときの処理
	void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  override;

protected:

	Collider* body;

	BreakableObjectData* m_data;

	// 体力
	int m_hp;
	// 破壊されたかどうか
	bool m_isBreaked;
	// 破壊時コールバック
	std::function<void(GameObject*, const Drop*, int, int)> m_breakCallBack;
};


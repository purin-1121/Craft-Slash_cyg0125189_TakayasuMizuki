//--------------------------------------------------------------------------------------
// File: Ground.h
//
// 地面
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once

#include "Game/GamePlay/GameObject/GameObject.h"
#include "Common/Collider/CollisionManager.h"

class Earth : public GameObject
{

public:

	// ------ コンストラクタ ------ //
	Earth();

	// ------ 基本関数 ------ //
	void Initialize(const GameContext& gameContext) override;
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) override;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view)override;
	void RenderText();
	void Finalize() override;

	// ------ Get ------ //
	bool IsActive() override { return true; }

	// ------ Set ------ //

	// ------ Add ------ //

	// ------ other ------ //

};


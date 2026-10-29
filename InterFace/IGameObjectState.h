//--------------------------------------------------------------------------------------
// File: State.h
//
// プレイヤーが使うステートパターンの基底クラス
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------

#pragma once
#include "Tools/GameContext.h"
class GameObject;

class IGameObjectState
{

public:

	virtual ~IGameObjectState() = default;

	virtual void Enter() = 0;
	virtual void Update(const GameContext& gameContext, float elapsedTime) = 0;
	virtual void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) = 0;
	virtual void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) = 0;
	virtual void Exit() = 0;

	// 変わっていいか判断する関数
	virtual bool CanChanged() = 0;

	virtual const std::string& GetStateName() const = 0;


};


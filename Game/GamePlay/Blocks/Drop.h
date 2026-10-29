#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include "Game/GamePlay/Item/DropData.h"

class Drop
{

public:

	// ------ Get ------ //
	const TypeInfo& GetTypeInfo() const { return m_drop; }

public:

	Drop(const TypeInfo& info);
	// ドロップの単位
	static constexpr int MAX_DROP = 5;

private:

	// ---- 基本のパラメータ ---- //
	// このオブジェクトがドロップする種類
	const TypeInfo& m_drop;

	// ---- デバッグ用 ---- //

};


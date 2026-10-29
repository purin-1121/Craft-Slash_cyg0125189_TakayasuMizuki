#pragma once
#include "Collider.h"

class CollisionResolver
{
public:

	/// <summary>
	/// 衝突した面を計算する関数
	/// </summary>
	/// <param name="a">ボックスのコライダー</param>
	/// <param name="b">ボックスのコライダー</param>
	/// <returns>衝突した面</returns>
	static Direction GetCollisionSide(BoxCollider& a, BoxCollider& b);
	static Direction GetCollisionSide(Collider* a, Collider* b);

	/// <summary>
	/// 衝突した面から速度の補正を行う関数
	/// </summary>
	/// <param name="a">衝突した面</param>
	/// <param name="b">オブジェクト</param>
	/// <returns></returns>
	static void RestrictMovementOnContact(Direction d, GameObject& obj);
};


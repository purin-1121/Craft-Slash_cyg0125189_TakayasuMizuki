#pragma once
#include "Collider.h"

namespace NarrowPhase
{
	// どのコライダーがぶつかっているのか判定する関数
	CollisionInfo IsColliding(Collider* a, Collider* b);

	// 2D衝突判定の検知（点と平面）
	CollisionInfo IsColliding(const  SimpleMath::Vector2& a, const PlaneCollider& b);

	// 2D衝突判定の検知（平面と平面）
	CollisionInfo IsColliding(PlaneCollider& a, PlaneCollider& b);

	// 3D衝突判定の検知（点と矩形）
	CollisionInfo IsColliding(SimpleMath::Vector3& a, BoxCollider& b);

	// 3D衝突判定の検知（矩形と矩形）
	CollisionInfo IsColliding(BoxCollider& a, BoxCollider& b);

	// 3D衝突判定の検知（矩形と球）
	CollisionInfo IsColliding(BoxCollider& a, SphereCollider& b);

	// 3D衝突判定の検知（球と球）
	CollisionInfo IsColliding(SphereCollider& a, SphereCollider& b);

	// 3D衝突判定の検知（カプセルとカプセル）
	CollisionInfo IsColliding(CapsuleCollider& a, CapsuleCollider& b);
	
	// 3D衝突判定の検知（カプセルと球）
	CollisionInfo IsColliding(CapsuleCollider& capsule, SphereCollider& sphere);

	// 3D衝突判定の検知（カプセルとボックス)
	CollisionInfo IsColliding(CapsuleCollider& capsule, BoxCollider& box);
	
	// 3D衝突判定の検知（円柱とボックス)
	CollisionInfo IsColliding(CylinderCollider& cylinder, BoxCollider& box);

	// 3D衝突判定の検知（OBBとOBB)
	CollisionInfo IsColliding(OBBCollider& obbA, OBBCollider& obbB);
	// 3D衝突判定の検知（OBBとボックス)
	CollisionInfo IsColliding(OBBCollider& obb, BoxCollider& box);
	// 3D衝突判定の検知（OBBと球)
	CollisionInfo IsColliding(OBBCollider& obb, SphereCollider& box);
	// 3D衝突判定の検知（OBBと円柱)
	CollisionInfo IsColliding(OBBCollider& obb, CylinderCollider& cylinder);

	CollisionInfo ReverseCollisionInfo(CollisionInfo info);
}

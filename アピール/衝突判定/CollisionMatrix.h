#pragma once
#include "Collider.h"

class CollisionMatrix
{
public:
	static bool CanCollide(ColliderLayer a, ColliderLayer b) { return m_collisionMatrix[(int)a][(int)b]; };
private:
	// 衝突マトリクス
	static constexpr bool m_collisionMatrix[(int)ColliderLayer::All][(int)ColliderLayer::All] =
	{
		// D,   P,     E,      ES,    L,    C,     CB,    PA,    B
		{ true,  true,  true, false,  true, false,  true,  true, false},		// Default
		{ true, false,  true,  true,  true,  true,  true, false,  true},		// Player
		{ true,  true,  true, false,  true, false,  true,  true,  true},		// Enemy
		{false,  true, false, false, false, false, false, false, false},		// EnemySearch
		{ true,  true,  true, false,  true,  true,  true,  true,  true},		// Leg
		{false,  true, false, false,  true, false, false, false, false},		// Chest
		{ true,  true,  true, false,  true, false,  true, false,  true},		// ChestBody
		{ true, false,  true, false,  true, false, false, false,  true},		// PlayerAttack
		{false,  true,  true, false,  true, false,  true,  true, false},		// Breakble
	};
};

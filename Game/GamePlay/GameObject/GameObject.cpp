#include "pch.h"
#include "GameObject.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/Draw/DrawSystem.h"

using namespace DirectX;
using namespace SimpleMath;

GameObject::GameObject(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, float mass, bool kinematic)
	: m_parent{}
	, m_children{}
	, m_groundState{GroundState::Fly}
	, m_position{pos}
	, m_posOffset{}
	, m_scale{}
	, m_velocity{vec}
	, m_quaternion{}
	, m_isKinematic{kinematic}
	, m_colliders{}
	, m_mass{mass}
	, body{}
{
	m_worldMatrix =
		SimpleMath::Matrix::CreateFromQuaternion(m_quaternion) *
		SimpleMath::Matrix::CreateTranslation(m_position);
	PhysicsUtility::GetRefarence().RegisterObject(this);
	DrawSystem::GetInstance()->RegisterObject(this);
}

GameObject::~GameObject()
{
	for (auto* c : m_colliders) {
		CollisionManager::GetInstance()->RemoveDestroyedCollider(c); // “o˜^•ë‚©‚çŠO‚·
		delete c;                                            // ŽÀ‘Ì‚ð‰ð•ú
	}
	PhysicsUtility::GetRefarence().UnRegisterObject(this);
	DrawSystem::GetRefarence().UnRegisterObject(this);
}

void GameObject::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{
	for (const auto& c : m_colliders)
	{
		c->Draw();
	}
}


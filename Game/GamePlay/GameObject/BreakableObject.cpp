#include "pch.h"
#include "BreakableObject.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "InterFace/IDamageDealer.h"
#include "Game/GamePlay/Factory/DropFactory.h"

using namespace DirectX;
using namespace SimpleMath;

BreakableObject::BreakableObject(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, TypeInfo* info, BreakableObjectData* data)
	: GameObject(pos, vec, data->mass, data->isKinematic)
    , Drop(*info)
	, m_data{data}
    , m_hp{}
	, m_isBreaked{}
	, body{}
{
}

void BreakableObject::Initialize(const GameContext& gameContext)
{
	m_hp = m_data->maxHp;
    m_isBreaked = false;
    if (m_data->collider.type == "Box") 
        body = AddCollider<BoxCollider>(m_data->collider.position, m_data->collider.size);
    if (m_data->collider.type == "Sphere") 
        body = AddCollider<SphereCollider>(m_data->collider.size.x, m_data->collider.position, m_data->collider.vec);
    if (m_data->collider.type == "Capsule") 
        body = AddCollider<CapsuleCollider>(m_data->collider.size.x, m_data->collider.size.y, m_data->collider.position, m_data->collider.vec);
    body->SetLayer(ColliderLayer::Breakble);

    CreateWorldMatrix();
    m_breakCallBack = DropFactory::GetInstance().GetDropMethod();
}

void BreakableObject::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
}

void BreakableObject::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    // ‰Ò“­ó‘Ô‚¾‚Á‚½‚ç•`‰æ‚·‚é
    if (!m_isBreaked)
        Draw::GetInstance().DrawModel(gameContext, GetWorldMatrix(), view, ObjectLib::m_models[m_data->name].get());
}

void BreakableObject::RenderText()
{
}

void BreakableObject::Finalize()
{
}

void BreakableObject::CreateWorldMatrix()
{
    SetWorldMatrix(SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion()) * // ‰ñ“]
        SimpleMath::Matrix::CreateTranslation(GetPosition()));      // ˆÊ’u
}

void BreakableObject::OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info) 
{
    if (other->IsTag(ColliderTag::PlayerAttack))
    {
        m_hp -= dynamic_cast<IDamageDealer*>(otherObj)->GetDamage();
        if (m_hp <= 0)
        {
            if (!IsBreaked())
            {
                m_breakCallBack(this, this, MAX_DROP * 2, MAX_DROP * 3);
                body->SetEnable(false);
                m_isBreaked = true;
            }
        }
        else
        {
            m_breakCallBack(this, this, 1, MAX_DROP);
        }
    }

}



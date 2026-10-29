#include "pch.h"
#include "Chest.h"
#include "Common/Model/ObjectManager.h"
#include "Lib/Camera.h"
#include <Common/Manager/InputManager.h>

using SimpleMath::Vector3;

Chest::Chest(SimpleMath::Vector3 pos, TypeInfo* info, InputGroup* group)
    : GameObject(pos, SimpleMath::Vector3::Zero, 100.0f, false)
    , Drop(*info)
    , IInputObserver(group)
    , m_isOpend{}
    , m_canOpend{}
    , body{}
    , search{}
{
}

void Chest::Initialize(const GameContext& gameContext)
{
    body = AddCollider<BoxCollider>(GetPosition(), bodySize, true);
    body->SetLayer(ColliderLayer::ChestBody);
    search = AddCollider<SphereCollider>(searchSize, GetPosition(), SimpleMath::Vector3::Zero, true);
    search->SetLayer(ColliderLayer::Chest);

    // 更新しないのであらかじめワールド行列を計算していく
    SetWorldMatrix(SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion()) * // ★回転（ここで計算した角度を入れる）
        SimpleMath::Matrix::CreateTranslation(GetPosition())      // 位置
    );
}

void Chest::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
    // 死んだら何もしない
    if (!IsAlive()) return;
}

void Chest::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    Draw::GetInstance().DrawModel(gameContext, GetWorldMatrix(), view, ObjectLib::m_models["Chest"].get());
}

void Chest::RenderText()
{
}

void Chest::Finalize()
{
    IInputObserver::m_parent->DeleteObserver(this);
}

void Chest::OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info) 
{
    if (other->GetTag() == ColliderTag::Player)
    {
        m_canOpend = true;
    }
}

void Chest::OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)  
{
    if (other->GetTag() == ColliderTag::Player)
    {
        m_canOpend = false;
    }
}

bool Chest::OnInputAccept(Message::InputEvent input)
{
    if (input == Message::InputEvent::OPEN)
    {
        if (m_canOpend)
        {
            m_opendCallBack(this, this, MAX_DROP * 2, MAX_DROP * 3);
            m_isOpend = true;
            search->SetEnable(false);
        }
    }

    return false;
}

void Chest::DropItem()
{

}



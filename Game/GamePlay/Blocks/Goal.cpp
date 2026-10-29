#include "pch.h"
#include "Goal.h"
#include "Common/Model/ObjectManager.h"
#include "Lib/Camera.h"

using namespace DirectX;

Goal::Goal(SimpleMath::Vector3 pos, std::function<void()> goal)
    : GameObject(SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, true)
    , m_isHidden{false}
    , m_goalCallBack{goal}
    , m_isGoaled{}
    , d{}
{
    //std::make_unique<SphereCollider>(1.0f, pos, SimpleMath::Vector3::Zero);
}

void Goal::Initialize(const GameContext& gameContext)
{
    m_isHidden = false;
    m_isGoaled = false;
    //body = AddCollider<BoxCollider>();
    //GetBounding()[0]->SetPos(GetPosition() + GetPosOffset());
}

void Goal::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
    // 死んだら何もしない
    if (!IsActive()) return;

}

void Goal::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    if (!IsActive()) return;

    // ワールド行列の計算
    SimpleMath::Matrix world = SimpleMath::Matrix::CreateTranslation(GetBounding()[0]->GetCenter());      // 位置

}

void Goal::RenderText()
{
}

void Goal::Finalize()
{
}

void Goal::OnTrigger(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info) 
{
}



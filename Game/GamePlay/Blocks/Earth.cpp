#include "pch.h"
#include "Earth.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Lib/Camera.h"

using namespace DirectX;
using namespace SimpleMath;

Earth::Earth()
    : GameObject(
        SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
        SimpleMath::Vector3::Zero,
        false
    )
{
    SetColliderOffset({ -1000, -2, -1000});
}

void Earth::Initialize(const GameContext& gameContext)
{
    //std::make_unique<BoxCollider>(SimpleMath::Vector3{ 0, 0, 0 }, SimpleMath::Vector3{ 10, 2, 10 }, SimpleMath::Vector3::Zero),
    AddCollider<BoxCollider>();
}

void Earth::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{

}

void Earth::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    auto pos = GetPosition();
    pos.z += 10.0f;
    pos.x += 10.0f;
    pos.y += 0.0f;
    auto world =SimpleMath::Matrix::CreateTranslation(pos);
    Draw::GetInstance().DrawModel(gameContext, world, view, ObjectLib::m_models["Ground"].get());
}

void Earth::RenderText()
{
}

void Earth::Finalize()
{
}


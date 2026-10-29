#include "pch.h"
#include "Bullet.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Message/EventBus.h"

using namespace DirectX;

Bullet::Bullet()
    : GameObject(SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero)
    , m_active{ false }
    , m_aliveTime{}
    , m_flyTime{}
    , body{}
    , bomb{}
    , m_afterImpact{}
    , m_bulletData{}
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(const GameContext& gameContext)
{
    m_active = false;
    GetBounding().clear();
}

void Bullet::Update(float elapsedTime, const GameContext&, Camera&)
{
    SetVelocity_Y(0);
    if (m_afterImpact)
    {
        m_aliveTime += elapsedTime;
        if (m_aliveTime >= IMPACT_TIME)
        {
            m_active = false;
            m_bulletData = nullptr;
            CollisionManager::GetInstance()->RemoveCollider(bomb);
            bomb = nullptr;
        }
    }
    else if(m_flyTime > FLY_TIME || GetGround() == GroundState::Ground)
    {
        Bomb();
    }
    else
    {
        SetPosition(GetPosition() + GetVelocity());
        m_flyTime += elapsedTime;
    }
}

void Bullet::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    if (!body)
        return;
    using namespace DirectX::SimpleMath;
    auto world = Matrix::CreateScale(GetScale()) * Matrix::CreateFromQuaternion(GetQuaternion()) * Matrix::CreateTranslation(GetPosition());
    Draw::GetInstance().DrawModel(gameContext, world, view, ObjectLib::m_models[m_bulletData->modelPath].get());
}

void Bullet::RenderText()
{
}

void Bullet::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{
}

void Bullet::Finalize()
{
}

void Bullet::OnTriggerEnter(Collider* mine, Collider* other, GameObject* obj, const CollisionInfo& info)
{
    if (!m_active || m_afterImpact) return;
    if (!other->IsTrigger() && mine == body)
    {
        Bomb();
    }
}

void Bullet::AttackOn(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, BulletData* data, ColliderTag tag)
{
    // Reuse happens outside collision traversal.
    for (auto* collider : GetBounding())
    {
        CollisionManager::GetInstance()->RemoveDestroyedCollider(collider);
        delete collider;
    }
    GetBounding().clear();
    body = nullptr;
    bomb = nullptr;
    m_aliveTime = 0;
    m_flyTime = 0;
    m_afterImpact = false;
    m_active = true;
    m_bulletData = data;
    body = AddCollider<SphereCollider>(m_bulletData->bulletRadius, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, true);
    body->SetTag(tag);
    body->SetLayer((tag == ColliderTag::Player ? ColliderLayer::Player : ColliderLayer::Enemy));
    SetPosition(pos);
    SetVelocity(vec / 10);
    UpdateCollider();
    Flying(GroundState::Fly);
    SetScale(SimpleMath::Vector3::One * body->GetSize().x);
    bomb = nullptr;
}

// 爆破
void Bullet::Bomb()
{
    if (!m_active || m_afterImpact || !body) return;
    bomb = AddCollider<SphereCollider>(m_bulletData->bombRadius, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, true);
    UpdateCollider();
    bomb->SetTag((body->GetTag() == ColliderTag::Player ? ColliderTag::PlayerAttack : ColliderTag::EnemyAttack));
    CollisionManager::GetInstance()->RemoveCollider(body);
    body = nullptr;
    m_afterImpact = true;
    SetVelocity(SimpleMath::Vector3::Zero);
    m_aliveTime = 0;
    m_flyTime = 0;
}

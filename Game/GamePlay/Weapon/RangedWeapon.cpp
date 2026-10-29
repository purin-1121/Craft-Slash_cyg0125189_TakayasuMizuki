#include "pch.h"
#include "RangedWeapon.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include "Game/GamePlay/Actor/Player.h"

RangedWeapon::RangedWeapon(WeaponData* data)
    : IWeapon()
    , m_data{data}
    , m_owner{}
    , m_muzzleOffset{}
    , m_bulletManager{}
    , m_model{data->modelName}
    , m_currentData{}
    , m_coolTimer{}
{
}

void RangedWeapon::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
    
    if (m_coolTimer > 0.0f) m_coolTimer -= elapsedTime;

    m_bulletManager.Update(elapsedTime, gameContext, camera);
    
}

void RangedWeapon::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{

    m_bulletManager.Render(gameContext, view);

}

void RangedWeapon::AttackOn(ColliderTag tag)
{
    using Tag = ColliderTag;

    if (m_coolTimer > 0.0f) return; // クールタイム中は撃てない

    SimpleMath::Vector3 pos = m_model.GetWorldMatrix().Translation();
    SimpleMath::Vector3 dir = m_owner->GetForward();

    // Bulletを生成して発射する
    m_bulletManager.Shoot(pos, dir, m_data->bulletName, (tag == Tag::PlayerAttack ? Tag::Player : Tag::Enemy));

    m_coolTimer = m_data->coolTime;    
}

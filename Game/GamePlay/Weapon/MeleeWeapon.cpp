#include "pch.h"
#include "MeleeWeapon.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "WeaponManager.h"
#include "Message/EventBus.h"
#include "Common/Draw/DrawSystem.h"

using namespace DirectX;

MeleeWeapon::MeleeWeapon(WeaponData* data)
    : GameObject(SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero)
    , IWeapon()
    , m_active{}
    , m_data{ data }
    , m_initPosition{ SimpleMath::Vector3::Zero }
    , m_attack{}
    , m_attackTime{}
    , m_animStepEndPos{}
    , m_owner{}
    , m_coolTime{}
{
}

MeleeWeapon::~MeleeWeapon()
{
}

void MeleeWeapon::Initialize(const GameContext& gameContext)
{
    m_active = false;
    // 読み込んだ分だけコライダーを生成
    for (const auto& col : m_data->colliders)
    {
        if (col.type == "Box")
        {
            AddCollider<BoxCollider>(
                col.position,
                col.size,
                SimpleMath::Vector3{ 0,0,0 },
                true);
        }
        else if (col.type == "Sphere")
        {
            AddCollider<SphereCollider>(
                col.size.x,
                col.position,
                SimpleMath::Vector3{ 0,0,0 },
                true);
        }
    }

    if (GetBounding().empty())
    {
        AddCollider<BoxCollider>();
    }
    for (auto& c : GetBounding())
    {
        c->SetEnable(false);
        c->SetTag(ColliderTag::PlayerAttack);
        c->SetLayer(ColliderLayer::PlayreAttack);
    }
}

void MeleeWeapon::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
    Flying(GroundState::Ground);
}
void MeleeWeapon::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    // 現在のベクトルからオブジェクトの向きを設定
    auto vec = GetVelocity();
    SimpleMath::Matrix r;
    if (m_attack)
    {
        auto off = GetPosition();
        r = GetRotateMatrix();
    }
    else
    {
        r = SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion());
    }
    auto world = r * SimpleMath::Matrix::CreateTranslation(GetPosition() + GetPosOffset()) * GetWorldMatrix();
    Draw::GetInstance().DrawModel(gameContext, world, view, ObjectLib::m_models[m_data->modelName].get());
}

void MeleeWeapon::RenderText()
{
}

void MeleeWeapon::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{
    //GetBounding()->Draw(Colors::Yellow);  
}

void MeleeWeapon::Finalize()
{
    // GameObject's destructor unregisters and deletes the owned colliders.
    AttackOff();
}

void MeleeWeapon::OnTriggerEnter(Collider* mine, Collider* other, GameObject* obj, const CollisionInfo& info)
{
    if (other->GetTag() == ColliderTag::Enemy)
    {
        int damage = GetDamage();
        EventBus::GetInstance()->Publish(Message::EventID::ENEMY_TAKE_DAMAGE, &damage);
    }
}

void MeleeWeapon::AttackOn(ColliderTag tag)
{
    m_active = true;
    for (auto& b : GetBounding())
    {
        b->SetEnable(true);
        b->SetTag(tag);
        m_attack = true;
    }
}

void MeleeWeapon::AttackOff()
{
    m_active = false;
    for (auto& b : GetBounding())
    {
        b->SetEnable(false);
        m_attack = false;
    }
}

void MeleeWeapon::UpdateCollider()
{
    // 武器自身のワールド行列
    SimpleMath::Matrix world = GetWorldMatrix();

    // 描画と同じ回転行列を取得
    SimpleMath::Matrix r;

    if (m_attack)
    {
        r = GetRotateMatrix();
    }
    else
    {
        r = SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion());
    }

    // 描画時と同じ行列を作成
    world =
        r
        * SimpleMath::Matrix::CreateTranslation(
            GetPosition() + GetPosOffset())
        * world;

    // コライダーの位置と回転を更新
    for (auto& b : GetBounding())
    {
        // コライダーのローカルオフセットをワールド変換
        SimpleMath::Vector3 offset = b->GetPosOffset();

        SimpleMath::Vector3 worldOffset =
            SimpleMath::Vector3::TransformNormal(offset, world);

        // コライダーの中心位置
        SimpleMath::Vector3 center =
            world.Translation() + worldOffset;

        // SetPosが左下奥の座標を受け取る場合
        SimpleMath::Vector3 finalPos =
            center - b->GetSize() / 2.0f;

        b->SetPos(finalPos);
        b->SetRotatetion(GetQuaternion().ToEuler());
    }
}
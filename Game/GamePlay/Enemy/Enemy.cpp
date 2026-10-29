#include "pch.h"
#include "Enemy.h"
#include "Common/Model/ObjectManager.h"
#include "Message/EventBus.h"
#include "Message/AnnouncementManager.h"
#include "Lib/Camera.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyAttack.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyChase.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyNotice.h"
#include "Game/GamePlay/State/Actor/StunState.h"
#include "Game/GamePlay/Factory/EnemyStateFactory.h"
#include "Game/GamePlay/Factory/DropFactory.h"
#include "Game/GamePlay/Weapon/MeleeWeapon.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Common/Draw/ShadowRenderer.h"

using namespace DirectX;
class Stun;

Enemy::Enemy(Player* player, SimpleMath::Vector3 pos, int id, const EnemyData& data)
    : Actor(data.maxHp, pos,SimpleMath::Vector3::Zero, 1000.0f)
    , Drop(*DropFactory::GetInstance().GetInfo(data.name))
    , m_targetPlayer{player}
    , bodys{}
    , playerSearch{}
    , m_attackColliders{}
    , m_myData{&data}
    , m_idleAnimationName{data.name + "Idle"}
    , m_hitCount{}
    , m_id{id}
{
    // コライダーを作成する分だけあらかじめ確保しておく
    GetBounding().reserve(data.colliders.size());

    // データから判定を作成
    CreateColldiers(data);

    // 全判定に敵レイヤーを設定
    for (auto& b : GetBounding())
    {
        if(b->GetLayer() == ColliderLayer::Default)b->SetLayer(ColliderLayer::Enemy);
        if (b->GetTag() == ColliderTag::Default)b->SetTag(ColliderTag::Enemy);
        b->SetEnable(true);
    }

    for (auto& a : m_attackColliders)
    {
        a->SetTag(ColliderTag::EnemyAttack);
    }

    AttackOff();

    // ステートの作成と登録
    for (auto& state : m_myData->stateIDs)
    {
        std::unique_ptr<IGameObjectState> s = EnemyStateFactory::GetRefarence().GetEnemyState(m_myData->name, state, this);
        if (s != nullptr)
        {
            m_stateMachine.RegisterState(std::move(s), state);
        }
        else
        {
            // nullptrが返ってきた場合のログ
            OutputDebugStringA("Failed to create state!\n");
        }
    }

}

void Enemy::Initialize(const GameContext& gameContext)
{

    SetVelocity(SimpleMath::Vector3::Zero);

    m_stateMachine.RequestChangeState(StateID::Idle);

    UpdateCollider();
    m_dieCallback = DropFactory::GetInstance().GetDropMethod();

    auto device = gameContext.deviceResources.GetD3DDevice();

    // 描画リソースを作成
    if (!m_skinnedRenderer.Initialize(device))
    {
        OutputDebugStringA("描画クラスの初期化に失敗しました。\n");
    }

    if (!m_skinnedRenderer.CreateFromResource(
        device, *ObjectLib::m_animatedModels[GetIdleAnimationName()]))
    {
        OutputDebugStringA("メッシュのGPUリソース作成に失敗しました。\n");
    }

	// 武器の初期化  
    m_weapons->ChangeWeapon(m_myData->weaponName, gameContext);
}

void Enemy::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    // 死んだら何もしない
    if (!IsAlive()) return;

    auto context = gameContext.deviceResources.GetD3DDeviceContext(); 
    // カリング方向を変更
	context->RSSetState(gameContext.commonStates.CullClockwise());

	SetRotateMatrix(SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion()));
	SimpleMath::Matrix world = GetRotateMatrix() *
		SimpleMath::Matrix::CreateTranslation(GetPosition());

	// 深度ステンシルステートをデフォルトに設定
	context->OMSetDepthStencilState(gameContext.commonStates.DepthDefault(), 0);

    // 影の描画
	const float radius = 0.7f;

	// groundHeightは、プレイヤーの真下の地面の高さ
	const SimpleMath::Matrix shadowWorld =
		SimpleMath::Matrix::CreateScale(radius, 1.0f, radius) *
		SimpleMath::Matrix::CreateTranslation(
			GetPosition().x,
			GetHeight() + 0.01f,
			GetPosition().z);

	const SimpleMath::Matrix shadowWVP =
		shadowWorld * view *
		Draw::GetInstance().GetRenderingContext().proj;

	ShadowRenderer::GetInstance()->Draw(gameContext, shadowWVP, 0.4f);
    
	// プレイヤーの描画
	m_skinnedRenderer.Draw(
		gameContext.deviceResources.GetD3DDeviceContext(),
		world,
		view,
		Draw::GetInstance().GetRenderingContext().proj, (m_invisibleTime ? 0.5f : 0.0f), m_invisibleTime);


    SetWorldMatrix(world);  // Renderでも更新しておく（描画用に最新を保証）

	// 子供にワールド行列を登録
	for (auto& child : GetChildren())
	{
		child->SetWorldMatrix(GetWorldMatrix());
	}
	
	context->RSSetState(gameContext.commonStates.CullCounterClockwise());

	// 武器の描画
    m_weapons->Render(gameContext, view);
}

void Enemy::RenderText()
{
}

void Enemy::Finalize()
{
}
    
SimpleMath::Vector3 Enemy::GetPlayerPos() const
{
    if (m_targetPlayer)
    {
        return m_targetPlayer->GetPosition(); 
    }
    return SimpleMath::Vector3::Zero;
}

bool Enemy::IsPlayerInSearchArea()
{
    if (!m_targetPlayer) return false;
    return playerSearch->IsPointInside(m_targetPlayer->GetPosition());
}
void Enemy::AttackOn()
{
    for (auto& col : m_attackColliders)
    {
        col->SetEnable(true);
    }
    m_stateMachine.ResetTimer();
}

void Enemy::AttackOff()
{
    if(m_currentWeapon)
        m_currentWeapon->AttackOff();
    for (auto& col : m_attackColliders)
    {
        col->SetEnable(false);
    }
}


void Enemy::CreateColldiers(const EnemyData& data)
{
    // データに合わせてコライダーを作成
    for (auto& col : data.colliders)
    {
        switch (col.tag)
        {
            // 足コライダー
        case ColliderTag::Leg:
            if (col.type == "Box")
                m_legs.push_back(AddCollider<BoxCollider>(col.position, col.size));
            else if (col.type == "Sphere")
                m_legs.push_back(AddCollider<SphereCollider>(col.size.x, col.position, col.vec));
            break;
            // 攻撃コライダー
        case ColliderTag::EnemyAttack:
            if (col.type == "Box")
                m_attackColliders.push_back(AddCollider<BoxCollider>(col.position, col.size, true));
            else if (col.type == "Sphere")
                m_attackColliders.push_back(AddCollider<SphereCollider>(col.size.x, col.position, col.vec, true));
            break;
        default:
            // デフォルト
            if (col.type == "Box")
                bodys.push_back(AddCollider<BoxCollider>(col.position, col.size, true));
            else if(col.type == "OBB")
                bodys.push_back(AddCollider<BoxCollider>(col.position, col.size));
            else if (col.type == "Sphere")
                bodys.push_back(AddCollider<SphereCollider>(col.size.x, col.position, col.vec));
            else if(col.type == "Capsule")
                bodys.push_back(AddCollider<CapsuleCollider>(col.size.x, col.size.y, col.position, col.vec));
            // 探索範囲コライダー
            else if (col.type == "Search")
                playerSearch = AddCollider<CylinderCollider>(col.size.x, col.size.y, col.position, col.vec, true)->SetLayer(ColliderLayer::EnemySearch);
            break;
        }
    }
}

void Enemy::OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)
{
    // プレイヤーの攻撃だったら
    if (other->GetTag() == ColliderTag::PlayerAttack && mine != playerSearch)
    {
        // 自分の攻撃コライダー同士の判定は無視
        if (std::find(m_attackColliders.begin(), m_attackColliders.end(), mine) != m_attackColliders.end())
            return;

        if (!ShouldProcessHit(otherObj)) return; // ← Actor側の共通処理を呼ぶだけ

        if (IsInvisible())
            return;

        // ダメージ処理
        float damage = dynamic_cast<IDamageDealer*>(otherObj)->GetDamage();
        float d = m_buffManager.Calculate(m_myData->defense, BuffManager::StatesEffectType::Defence);
        m_hp -= damage / d;
        if (m_hp <= 0)
        {
            m_stateMachine.RequestChangeState(StateID::Defeat);
        }
        else
        {
            m_stateMachine.RequestChangeState<Stun>();

            // 吹き飛ばす
			/*SimpleMath::Vector3 impuctVec = GetCenter() - otherObj->GetCenter();
			impuctVec.Normalize();
			impuctVec *= static_cast<Enemy*>(otherObj)->GetData()->power / 10;
			SetVelocity(impuctVec);*/
        }
    }

    // 自分の判定がsearch用で相手がプレイヤーだったら
    if (mine == playerSearch && other->GetTag() == ColliderTag::Player)
    {
        // 追跡中ではない場合、発見モードに移行
        if (!m_stateMachine.IsState(StateID::Attack) && !m_stateMachine.IsState(StateID::Chase))
            m_stateMachine.RequestChangeState(StateID::Notice);
    }
}

void Enemy::OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)
{
    if (other->GetTag() == ColliderTag::PlayerAttack)
        ClearHit(otherObj);
}

void Enemy::DisposeWeapon()
{
    m_currentWeapon->Finalize();
    m_currentWeapon = nullptr;
}




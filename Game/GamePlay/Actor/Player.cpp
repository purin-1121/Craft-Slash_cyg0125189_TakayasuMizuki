#include "pch.h"
#include "Player.h"
#include "Common/Draw/Draw.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Manager/InputManager.h"
#include "Lib/MathLib.h"
#include "Lib/Camera.h"
#include "Message/InputGroup.h"
#include "Message/EventBus.h"
#include "Game/GamePlay/Item/Item.h"
#include "Game/GamePlay/State/Actor/StunState.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Common/Draw/ShaderManager.h"
#include "Common/Draw/ShadowRenderer.h"

using namespace DirectX;



Player::Player(InputGroup* group)
	: Actor(MAX_HP, START_POS, SimpleMath::Vector3(0.0f, 0.0f, 0.0f), 600.0f)
	, InputGroup(group)
	, m_inventry{}
	, body{}
	, leg{}
	, m_holding{}
{
}

void Player::Initialize(const GameContext& gameContext)
{
	// 足コライダーの設定
	leg = AddCollider<SphereCollider>(0.2f, START_LEG_POS, SimpleMath::Vector3::Zero);
	leg->SetTag(ColliderTag::Player);
	leg->SetLayer(ColliderLayer::Player);
	leg->SetPosOffset(START_LEG_POS);

	// 体コライダーの設定
	body = AddCollider<BoxCollider>(START_POS, HUMAN_SIZE, SimpleMath::Vector3::Zero);
	body->SetLayer(ColliderLayer::Player);
	body->SetTag(ColliderTag::Player);
	//body->SetPosOffset(SimpleMath::Vector3{ 0, HUMAN_SIZE.y / 2, 0});
	body->SetPosOffset(SimpleMath::Vector3{ HUMAN_SIZE.x, 0, HUMAN_SIZE.z } / -4.0f);

	// 武器の設定
	m_weapons->Initialize(gameContext);

	// ステートの作成
	CreateState();

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
}

void Player::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	// Actorの更新
	Actor::Update(elapsedTime, gameContext, camera);

	if(m_currentWeapon != nullptr)
		m_weapons->Update(elapsedTime, gameContext, camera);
}

void Player::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
	auto context = gameContext.deviceResources.GetD3DDeviceContext(); 
	context->RSSetState(gameContext.commonStates.CullClockwise());

	SetRotateMatrix(SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion()));
	SimpleMath::Matrix world = GetRotateMatrix() *
		SimpleMath::Matrix::CreateTranslation(GetPosition());

	context->OMSetDepthStencilState(gameContext.commonStates.DepthDefault(), 0);

	auto worldViewProj = world * view * Draw::GetInstance().GetRenderingContext().proj;

	ShaderManager::GetInstance()->ApplyShadow(
		gameContext.deviceResources.GetD3DDeviceContext(),
		worldViewProj,
		GetPosition().y
	);

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
	if(m_holding)
		m_weapons->Render(gameContext, view);
}

void Player::RenderText()
{
	
}

void Player::Finalize()
{
	m_evolutionManager.Finalize();
}

int Player::GetItem(uint8_t type)
{
	// アイテムの数
	int sum = 0;

	// 各スロットから同じ種類のアイテムを計算
	for (const auto& item : m_inventry)
	{
		if (item.active && item.type == type) sum += item.num;
	}

	return sum;
}

void Player::ProcessAttack()
{
}

bool Player::CollectItem(uint8_t type)
{
	// 最初のスロットを取得
	InventryData::Slot* it = nullptr;
	// アイテムを回収できたか
	bool added = false;

	for (auto& slot : m_inventry)
	{
		// 種類が一緒でスロットの上限以下だったら
		if (slot.type == type && slot.num < InventryData::MAX_ITEM)
		{
			// activeにして数を増やす
			slot.active = true;
			slot.num++;
			added = true;
			break;
		}
		// それ以外でスロットが未使用だったら
		else if (!slot.active && !it)
		{
			it = &slot;
		}
	}

	// 未だ取得していない、空のスロットがある、数が最大数以下だったら
	if (!added &&
		it &&
		it->num < InventryData::MAX_ITEM)
	{
		it->num++;
		it->type = type;
		it->active = true;
		added = true;
	}

	if (added)
	{
		// アイテム追加後に進化チェック
		EventBus::GetInstance()->Publish(Message::EventID::PICKUP_ITEM,&m_inventry);
	}

	return added;
}
void Player::OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)
{
	// アイテムだったら
	if (other->GetTag() == ColliderTag::Item)
	{
		OutputDebugStringA(mine == leg ? "leg hit\n" : "body hit\n");
		// アイテムの取得処理
		auto i = dynamic_cast<Item*>(otherObj);
		CollectItem(i->GetType());
	}
	// 敵の攻撃だったら
	if (other->GetTag() == ColliderTag::EnemyAttack)
	{
		if (!ShouldProcessHit(otherObj)) return; // ← Actor側の共通処理を呼ぶだけ

		// ---- ここから先は「1回だけ」実行される ---- //
		// 無敵だったら
		if (IsInvisible())
			return;

		m_hp -= static_cast<Enemy*>(otherObj)->GetDamage();
		if (m_hp <= 0)
		{
			// 負けメッセージ送信
			EventBus::GetInstance()->Publish(Message::EventID::FAIL, this);
		}
		else
		{
			// スタンステートへ移行
			m_idAfterStun = StateID::Idle;
			m_stateMachine.RequestChangeState<Stun>();

			// 吹き飛ばす
			SimpleMath::Vector3 impuctVec = GetCenter() - otherObj->GetCenter();
			impuctVec.Normalize();
			impuctVec *= static_cast<Enemy*>(otherObj)->GetData()->power / 10;
			SetVelocity(impuctVec);
		}
	}
}

void Player::OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)
{
	if (other->GetTag() == ColliderTag::EnemyAttack)
	{
		ClearHit(otherObj);
	}
}

void Player::OnCollision(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)
{
	// 相手が地面判定を持つものだったら
	if (mine == leg && other->GetTag() == ColliderTag::Ground)
	{
		// 床からプレイヤーに向かう上向き法線
		float upDot = info.normal.Dot(SimpleMath::Vector3::UnitY);
		// 床とみなす角度の下限
		float slopeLimit = 0.7f;
		if (upDot > slopeLimit && GetVelocity().y <= 0.0f)
		{
			Flying(GroundState::Ground);
		}
	}
}

bool Player::OnInputAccept(Message::InputEvent input)
{
	// 移動、ジャンプ、攻撃処理のみ受け付け
	switch (input)
	{
	case Message::InputEvent::MOVEUP:
	case Message::InputEvent::MOVEDOWN:
	case Message::InputEvent::MOVELEFT:
	case Message::InputEvent::MOVERIGHT:
	case Message::InputEvent::ATTACK:
		m_stateMachine.OnInputAccept(input);
		break;
	case Message::InputEvent::JUMP:
		if (GetGround() == GroundState::Ground && GetVelocity().y <= 0.0f)
			m_stateMachine.OnInputAccept(input);
		break;
	default:
		break;
	}

	return false;
}

void Player::Move(SimpleMath::Vector3 vec)
{
	// Camera pitch must not affect vertical movement.
	vec.y = 0.0f;
	// 加速
	AddVelocity(vec);

	SimpleMath::Vector3 v = { GetVelocity().x, 0 , GetVelocity().z };
	// 移動ベクトルが最大速度よりも大きかったら
	if (v.LengthSquared() > MAX_SPEED * MAX_SPEED)
	{
		// 正規化したものを最大速度に拡大
		v.Normalize();
		v *= MAX_SPEED;
		SetVelocity_X(v.x);
		SetVelocity_Z(v.z);
	}
}

void Player::CreateState()
{
	// ステートの作成
	m_stateMachine.RegisterState<PlayerIdle>(std::make_unique<PlayerIdle>(this), StateID::Idle);
	m_stateMachine.RegisterState<PlayerWolk>(std::make_unique<PlayerWolk>(this), StateID::Walk);
	m_stateMachine.RegisterState<PlayerJump>(std::make_unique<PlayerJump>(this), StateID::Jump);
	m_stateMachine.RegisterState<PlayerAttack>(std::make_unique<PlayerAttack>(this), StateID::Attack);
	m_stateMachine.RegisterState<Stun>(std::make_unique<Stun>(this), StateID::Stun);

	// 初期ステートの登録
	m_stateMachine.RequestChangeState<PlayerIdle>();
}


#include "pch.h"
#include "Item.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "ItemManager.h"

using namespace DirectX;

Item::Item()
	: GameObject(SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, true)
	, m_index{-1}
	, m_active{false}
	, m_setting{nullptr}
	, m_state{ItemState::Drop}
	, m_aliveTime{}
	, body{}
{
}

Item::Item(
	int index,
	SimpleMath::Vector3 pos,
	SimpleMath::Vector3 vec,
	ItemSetting* setting
)
	: GameObject(
		pos,
		vec, 
		true)
	, m_index{index}
	, m_active{false}
	, m_setting{setting}
	, m_aliveTime{}
	, m_state{ItemState::Drop}
	, body{}
{
}

Item::~Item()
{
}

void Item::Initialize(const GameContext& gameContext)
{
	m_active = false;
	body = AddCollider<SphereCollider>(0.3f, GetPosition(), SimpleMath::Vector3::Zero, true);
	body->SetTag(ColliderTag::Item);
	body->SetEnable(false);
}

void Item::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	switch (m_state)
	{
	case ItemState::Drop:
		m_aliveTime += elapsedTime;
		break;
	case ItemState::Idle:
		m_aliveTime += elapsedTime;
		break;
	case ItemState::Action:
		SetVelocity(GetVelocity() * 0.95f);
		break;
	default:
		break;
	}
	
	// 生存時間の計測
	m_aliveTime += elapsedTime;

}

void Item::Render(const GameContext& gameContext,  SimpleMath::Matrix view)
{
	// 本来のワールド行列
	auto canWorld = view.Invert();
	// 前方ベクトル
	auto forward = canWorld.Forward() * -1.0f;
	// ビルボード行列
	auto billBoard = SimpleMath::Matrix::CreateBillboard(
		GetBounding()[0]->GetPos(),
		canWorld.Translation(),
		canWorld.Up(),
		&forward
	);
	Draw::GetInstance().DrawModel(gameContext, billBoard, view, ObjectLib::m_models[m_setting->modelName].get(), 
		[&]() {
			gameContext.deviceResources.GetD3DDeviceContext()->OMSetBlendState(gameContext.commonStates.AlphaBlend(), nullptr, 0xFFFFFFFF);
			gameContext.deviceResources.GetD3DDeviceContext()->OMSetDepthStencilState(gameContext.commonStates.DepthRead(), 0);
		});
}

void Item::RenderText()
{
}

void Item::RenderBounding(const GameContext& gameContext,  SimpleMath::Matrix view)
{
}

void Item::Finalize()
{
	CollisionManager::GetInstance()->RemoveCollider(body);
}

uint8_t Item::GetType()
{ 
	return m_setting != nullptr ? m_setting->type : 0; 
}

void Item::Create(SimpleMath::Vector3 pos, ItemSetting* pool, SimpleMath::Vector3 vec)
{
	m_active = true;
	m_setting = pool;
	body->SetPos(pos);
	SetPosition(pos);
	SetVelocity(vec);
	m_aliveTime = 0;
	m_state = ItemState::Drop;
	body->SetEnable(true);
}

void Item::OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info) 
{
	if (other->GetTag() == ColliderTag::Player)
	{
		SetActive(false);
		body->SetEnable(false);
	}
}


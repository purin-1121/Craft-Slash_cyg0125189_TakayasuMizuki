#include "pch.h"
#include "Actor.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"

using namespace DirectX;
using namespace SimpleMath;

namespace
{
	// 持ち手の位置を作成
	SimpleMath::Matrix MakeHandPose(const aiMatrix4x4& modelSpaceTransform)
	{
		aiVector3D scaling;
		aiQuaternion rotation;
		aiVector3D position;
		modelSpaceTransform.Decompose(scaling, rotation, position);
		(void)scaling;

		// Convert Assimp's column-vector rotation for DirectXTK row-vector matrices.
		SimpleMath::Quaternion directXRotation(-rotation.x, -rotation.y, -rotation.z, rotation.w);
		SimpleMath::Matrix handPose = SimpleMath::Matrix::CreateFromQuaternion(directXRotation);
		handPose *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(180.0f));
		handPose *= SimpleMath::Matrix::CreateTranslation(
			position.x * 0.01f, position.y * 0.01f, position.z * 0.01f);
		return handPose;
	}
}

Actor::Actor(int hp, SimpleMath::Vector3 pos,SimpleMath::Vector3 vec, float mass)
    : GameObject(pos, vec, mass, true)
    , m_hp{hp}
    , m_damageCallback{}
    , m_dieCallback{}
    , m_d{Direction::front}
    , m_forword{}
    , m_invisibleTime{}
    , m_idAfterStun{}
    , m_rendVec{}
	, m_weapons{ std::make_unique<WeaponManager>(this) }
	, m_isStop{}
	, m_boneNameForWeapon{}


{
}

Actor::~Actor()
{
}

void Actor::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	if (!IsAlive())
		return;

	auto state = Keyboard::Get().GetState();
	SetElapsedTime(elapsedTime);
	auto context = gameContext.deviceResources.GetD3DDeviceContext();

	// 無敵時間減少
	m_invisibleTime = std::max(m_invisibleTime - elapsedTime, 0.0f);

	// 前方向の保存
	m_forword = camera.GetForward();
	// 描画用ベクトルをリセット
	m_rendVec = SimpleMath::Vector3::Zero;

	// ステート更新
	m_stateMachine.Update(gameContext, elapsedTime);

	SetRotateMatrix(SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion()));
	auto world = (GetRotateMatrix() * SimpleMath::Matrix::CreateTranslation(GetPosition()));
	SetWorldMatrix(world);

	// ---- 子供のオフセット更新 ---- //
	for (auto& child : GetChildren())
	{
		child->SetPosOffset(HOLD_POSITION);
		child->SetWorldMatrix(world);
	}

	m_animator.Update(elapsedTime);

	
	// 減速処理
	auto v = GetVelocity();
	SetVelocity(SimpleMath::Vector3(v.x * 0.95f, v.y, v.z * 0.95f));

	// ---- 武器の更新 ---- //
	if (m_currentWeapon != nullptr)
	{
		m_currentWeapon->SetAttachTransform(HOLD_MATRIX * GetWorldMatrix());

		// 武器を持ち手の位置行列に合わせる
		aiMatrix4x4 handModelTransform;
		// ボーンがあったら
		if (m_animator.GetModelSpaceTransform(m_boneNameForWeapon, handModelTransform))
		{
			// 持ち手の位置行列を計算
			const SimpleMath::Matrix handPose = MakeHandPose(handModelTransform);
			const SimpleMath::Matrix weaponMatrix = 
				m_currentWeapon->GetHoldMatrix() * handPose * world;
			// 武器に適応
			m_currentWeapon->SetAttachTransform(
				weaponMatrix
			);
		}
		else
		{
			m_currentWeapon->SetAttachTransform(HOLD_MATRIX * world);
		}
	}

	// 現在のアニメーションに合わせて頂点バッファを書き換える
	m_skinnedRenderer.UpdateBoneMatrices(
		gameContext.deviceResources.GetD3DDeviceContext(), 
		m_animator, 
		*ObjectLib::m_animatedModels[GetIdleAnimationName()]
	);

}

void Actor::RenderText()
{
}

void Actor::RenderBounding(const GameContext& gameContext,  SimpleMath::Matrix view)
{
    gameContext;
    view;
}


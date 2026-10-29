#include "pch.h"
#include "PhysicsUtility.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include "Game/GamePlay/Actor/Player.h"

std::unique_ptr<PhysicsUtility> PhysicsUtility::s_physicsUtility = nullptr;

// 動的オブジェクトの移動更新
void PhysicsUtility::PhysicsUpdate()
{
	for (auto& o : m_objects)
	{
		// 運動しないオブジェクトなら次へ
		if (!o->IsKinematic())
		{
			continue;
		}

		// 重力設定
		ApplyGravity(&(*o), FIXED_TIME);

		// 終着速度を超えていたら補正する
		if (o->GetVelocity().y < TERMINAL_VELOCITY.y)
		{
			o->SetVelocity_Y(TERMINAL_VELOCITY.y);
		}

		o->AddPosition(o->GetVelocity() * FIXED_TIME);
	}
}

// 重力処理
void PhysicsUtility::ApplyGravity(GameObject* object, float time)
{
	// 着地していたら何もしない
	if (!object->IsActive() && object->GetGround() == GroundState::Ground)
		return;

	const float vy = object->GetVelocity().y;
	float scale;

	if (vy > 0.0f)
	{
		// 上昇中: プレイヤーがボタンを押している間だけ軽い。離したら強い
		Player* p = dynamic_cast<Player*>(object);
		scale = (p && p->PressedJump()) ? 1.0f : JUMPCUT_GRAVITY_SCALE;
	}
	else
	{
		// 落下中
		scale = FALL_GRAVITY_SCALE;
	}

	object->AddVelocity({ 0, scale * GRAVITY * time, 0 });
}
XMVECTOR PhysicsUtility::GetVec(float pitch, float yaw, XMVECTOR init_forward)
{
	// ラジアンに変換
	float pr = XMConvertToRadians(pitch);
	float yr = XMConvertToRadians(yaw);

	// 回転行列の作成
	XMMATRIX rot = XMMatrixRotationX(yr) * XMMatrixRotationY(pr);

	// 発射方向を計算 (forwardを回転させる)
	return XMVector3TransformNormal(init_forward, rot);
}

XMVECTOR PhysicsUtility::GetPos(float pitch, float yaw, XMVECTOR init_forward, SimpleMath::Vector3 origin, SimpleMath::Vector3 d_to_o)
{

	// 発射方向を計算 (forwardを回転させる)
	XMVECTOR launchDir = GetVec(pitch, yaw, init_forward);

	// 発射口の座標を計算
	XMVECTOR pivotPos = XMLoadFloat3(&origin);
	XMVECTOR fireDir = XMLoadFloat3(&d_to_o);
	XMVECTOR firePoint = pivotPos + (launchDir * fireDir);

	return firePoint;
}

void PhysicsUtility::UnRegisterObject(GameObject* object)
{
	m_objects.erase(
		std::remove(m_objects.begin(), m_objects.end(), object),
		m_objects.end()
	);
}

#include "pch.h"
#include "BossChase.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "BossAttack.h"

BossChase::BossChase(Enemy* owner)
	: BossState(owner, "Chase")
{
}

BossChase::~BossChase()
{
}

void BossChase::Enter()
{
	m_owner->PlayAnimation("Run");
}

void BossChase::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;

	auto data = m_owner->GetData();
	
	// Y軸（高さ）を無視した水平位置で距離判定
	auto BossPos = m_owner->GetPosition();
	auto playerPos = m_owner->GetPlayerPos();

	// 水平距離を計算するために Y を揃える
	auto flatBossPos = BossPos;  flatBossPos.y = 0.0f;
	auto flatPlayerPos = playerPos; flatPlayerPos.y = 0.0f;

	float distanceSq = (flatPlayerPos - flatBossPos).LengthSquared();

	// 一定距離以上だったらプレイヤー追尾
	if (distanceSq > data->attackDistance * data->attackDistance)
	{
		// 操舵力を計算
		DirectX::SimpleMath::Vector3 newVel = MathLib::SetStreeringBehavier(
			m_owner->GetVelocity(),
			m_owner->GetPosition(),
			m_owner->GetPlayerPos(),
			data->maxSpeed,
			data->maxForce
		);

		// 垂直方向の力をカット（水平移動のみにする）
		newVel.y = 0.0f;
		m_owner->SetVelocity(newVel);

		// 進行方向に向かってスムーズに回転させる
		DirectX::SimpleMath::Vector3 moveDir = newVel;
		moveDir.y = 0.0f;

		// 速度が十分にある場合のみ回転処理を行う
		constexpr float MIN_MOVE_THRESHOLD_SQ = 0.001f;
		if (moveDir.LengthSquared() > MIN_MOVE_THRESHOLD_SQ)
		{
			moveDir.Normalize();

			// 目標となる回転行列とクォータニオンの作成
			DirectX::SimpleMath::Matrix targetMatrix = DirectX::SimpleMath::Matrix::CreateWorld(
				DirectX::SimpleMath::Vector3::Zero, -moveDir, DirectX::SimpleMath::Vector3::Up
			);
			DirectX::SimpleMath::Quaternion targetRot = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(targetMatrix);

			// 旋回スピード（大きいほど素早く向く）
			constexpr float ROTATE_SPEED = 10.0f;
			float t = std::min(ROTATE_SPEED * elapsedTime, 1.0f);

			// 現在の角度から目標角度へ補間
			m_owner->SetQuaternion(DirectX::SimpleMath::Quaternion::Slerp(m_owner->GetQuaternion(), targetRot, t));
		}
	}
	// 攻撃範囲内に入ったら攻撃処理へ
	else
	{
		m_owner->GetStateMachine().RequestChangeState<BossAttack>();
	}
}

void BossChase::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());

}

void BossChase::Exit()
{
}


#include "pch.h"
#include "DashAttack.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include <Game/GamePlay/State/Boss/BossNotice.h>

DashAttack::DashAttack(Enemy* owner)
	: BossAttackBase(owner)
{
}

DashAttack::~DashAttack()
{
}

void DashAttack::Enter()
{
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	ResetFinished();
	m_targetVec = m_owner->GetPlayerPos() - m_owner->GetPosition();
	m_distance = m_targetVec;
	m_targetVec.Normalize();
}

void DashAttack::Update(const GameContext& gameContext, float elapsedTime)
{
	AddTime(elapsedTime);

	float time = GetTime();

	// ­‚µ’âŽ~‚µ‚Ä‚©‚çI—¹
	if (time >= 3.0f)
	{
		Finish();
	}
	// ’âŽ~ˆÊ’u‚Ü‚ÅŒ¸‘¬
	else if(time >= 1.7f && time < 2.0f)
	{
		float t = time - 1.7f;
		t = MathLib::Clamp(t, 0.0f, 0.3f);

		// 1.7•b‚©‚ç3.0•b‚Ü‚Å‚Ì1.3•bŠÔ‚ÅŒ¸‘¬
		float speed = Easing::EaseIn(t,DASH_SPEED,-DASH_SPEED,0.3f);

		m_owner->SetVelocity(m_targetVec * speed);
	}
	// –Ú“I’n‚ß‚ª‚¯‚Ä“Ëi
	else if(time >= 0.5f)
	{
		m_owner->AttackOn();
		m_owner->SetVelocity(m_targetVec * DASH_SPEED);
	}
	// –Ú“I’n‚Ì‚Ù‚¤‚Ö‰ñ“]
	else
	{
		// ‚Ü‚¾Œü‚«‚ªƒYƒŒ‚Ä‚¢‚é‚Ì‚ÅASlerp‚Å•âŠÔ‚µ‚ÄU‚èŒü‚«‚ð
		SimpleMath::Vector3 targetDir = { m_distance.x, 0.0f, m_distance.z };
		SimpleMath::Matrix targetMatrix = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, -targetDir, SimpleMath::Vector3::Up);
		SimpleMath::Quaternion targetRotation = SimpleMath::Quaternion::CreateFromRotationMatrix(targetMatrix);

		float rotateSpeed = 3.0f; // U‚èŒü‚­ƒXƒs[ƒh
		float t = std::min(rotateSpeed * elapsedTime, 1.0f);

		m_owner->SetQuaternion(SimpleMath::Quaternion::Slerp(m_owner->GetQuaternion(), targetRotation, t));
	}
}

void DashAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void DashAttack::Exit()
{
	m_owner->AttackOff();
	ResetTime();
}

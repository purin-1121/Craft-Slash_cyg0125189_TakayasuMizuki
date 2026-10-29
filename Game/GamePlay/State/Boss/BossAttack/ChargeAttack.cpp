#include "pch.h"
#include "ChargeAttack.h"
#include "Game/GamePlay/State/Boss/BossAttack.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Message/AnnouncementManager.h"

ChargeAttack::ChargeAttack(Enemy* owner)
	: BossAttackBase(owner)
{
}

ChargeAttack::~ChargeAttack()
{
}

void ChargeAttack::Enter()
{
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_owner->InvisibleOn(2.0f);
	ResetFinished();
}

void ChargeAttack::Update(const GameContext& gameContext, float elapsedTime)
{
	m_owner->GetStateMachine().AddTimer(elapsedTime);
	if (m_owner->GetStateMachine().GetTimer() > 2.0f)
	{
		Finish();

		// UŒ‚‚Æ–hŒä‚Ìƒoƒt
		auto& buff = m_owner->GetBuffManager();
		buff.Add({ BuffManager::StatesEffectType::Power, 2.0f, 0, BuffManager::ModifireType::Multiply, true });
		buff.Add({ BuffManager::StatesEffectType::Defence, 1.3f, 0, BuffManager::ModifireType::Multiply, true });
		AnnouncementManager::GetInstance()->Announce(
			{
				AnnouncementManager::AnnouncementType::BossAttack,
				L"ƒ{ƒX‚ÌUŒ‚—Í‚Æ–hŒä—Í‚ªã‚ª‚Á‚½!",
				2.0f
			}
		);
	}
}

void ChargeAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void ChargeAttack::RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
}

void ChargeAttack::Exit()
{
	m_owner->GetStateMachine().ResetTimer();
}

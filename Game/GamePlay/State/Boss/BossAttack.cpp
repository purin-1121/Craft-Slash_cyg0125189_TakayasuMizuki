#include "pch.h"
#include "BossAttack.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "BossChase.h"
#include "BossIdle.h"
#include "Game/GamePlay/Data/BossAttackData.h"

BossAttack::BossAttack(Enemy* owner, const std::vector<BossAttackData>& data)
	: BossState(owner, "Attack")
	, m_data{data}
{
	CreateAttackMethod();
}

BossAttack::~BossAttack()
{
	m_isCharge = false;
}

void BossAttack::Enter()
{
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);

	// çUåÇëIë
	m_currentAttack = SelectAttack();

	// ëIëÇµÇΩçUåÇÇäJén
	if (m_currentAttack)
	{
		m_currentAttack->Enter();
	}
	else
	{
		m_owner->GetStateMachine()
			.RequestChangeState<BossChase>();
	}
}

void BossAttack::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	
	if (!m_currentAttack)
		return;

	// åªç›ÇÃçUåÇÇé¿çs
	m_currentAttack->Update(
		gameContext,
		elapsedTime);

	// çUåÇèIóπÅH
	if (m_currentAttack->IsFinished())
	{
		m_currentAttack->Exit();

		m_currentAttack = nullptr;
		m_owner->GetStateMachine()
			.RequestChangeState<BossChase>();
	}
}

void BossAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());

}

void BossAttack::Exit()
{
	if(m_currentAttack)
		m_currentAttack->Exit();
	m_owner->AttackOff();
}

BossAttackBase* BossAttack::SelectAttack()
{
	// hpäÑçáÇéÊìæ
	float hpRate = m_owner->GetHP() / m_owner->GetData()->maxHp;
	// ÉvÉåÉCÉÑÅ[Ç∆ÇÃãóó£ÇåvéZ
	float distance = SimpleMath::Vector3::Distance(m_owner->GetPosition(), m_owner->GetPlayerPos());
	// çUåÇåÛï‚
	std::vector<BossAttackData> candidate{};
	int s = m_data.size();
	candidate.reserve(s);

	for (const auto& data : m_data)
	{
		// Hpèåè
		if (hpRate > data.maxHpRate || hpRate < data.minHpRate)
		{
			continue;
		}

		// ãóó£èåè
		if (distance > data.maxDistance || distance < data.minDistance)
		{
			continue;
		}

		// àÍìxÇ´ÇËÇÃèÍçáÇ∑Ç≈Ç…é¿çsÇµÇΩÇ©
		if (data.isOnce && m_executeAttacks.contains(data.attackID))
		{
			continue;
		}

		// åÛï‚Ç…í«â¡
		candidate.push_back(data);
	}

	// óDêÊèáà èáÇ…ï¿Ç—ë÷Ç¶
	if (!candidate.empty())
	{
		std::sort(candidate.begin(), candidate.end(),
			[](BossAttackData a, BossAttackData b) {return a.priority > b.priority; });

		// ç≈óDêÊÇÃçUåÇÇéÊìæ
		BossAttackData selected = candidate.front();

		// ëIëÇ≥ÇÍÇΩçUåÇÇ™àÍìxÇ´ÇËÇ»ÇÁé¿çsçœÇ›Ç…Ç∑ÇÈ
		if (selected.isOnce)
		{
			m_executeAttacks.insert(selected.attackID);
		}

		return GetAttack(candidate.begin()->attackID);
	}
	else
	{
		return GetAttack("Slash");
	}

}

BossAttackBase* BossAttack::GetAttack(const std::string& id)
{
	return m_attacks.at(id).get();
}

void BossAttack::CreateAttackMethod()
{
	for (auto& method : m_data)
	{
		if (method.attackID == "Slash") m_attacks[method.attackID] = std::make_unique<SlashAttack>(m_owner);
		if (method.attackID == "Charge") m_attacks[method.attackID] = std::make_unique<ChargeAttack>(m_owner);
		if (method.attackID == "Dash") m_attacks[method.attackID] = std::make_unique<DashAttack>(m_owner);
	}
}

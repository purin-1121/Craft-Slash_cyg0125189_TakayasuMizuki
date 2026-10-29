#include "pch.h"
#include "PlayerWolk.h"
#include "Game/GamePlay/Actor/Player.h"

PlayerWolk::PlayerWolk (Player* owner)
	: PlayerState(owner, "Run")
{
}

PlayerWolk::~PlayerWolk ()
{
}

void PlayerWolk::Enter()
{
	// 地面にいたら歩行アニメーション
	if(m_owner->GetGround() == GroundState::Ground)
		m_owner->PlayAnimation("Run");
	// 武器は背中
	m_owner->SetBoneName("mixamorig6:Hips");
	m_owner->GetWeapon().SetOffsetOfState("Run");
}

void PlayerWolk::Update(const GameContext& gameContext, float elapsedTime)
{
	if (m_owner->GetGround() == GroundState::Ground && 
		m_owner->GetAnimator().GetModel() != ObjectLib::m_animatedModels["PlayerRun"].get())
		m_owner->PlayAnimation("Run");

	// 水平方向のみ取り出す
	SimpleMath::Vector3 flatSpeed = m_owner->GetVelocity();
	flatSpeed.y = 0.0f;
	float velocity = flatSpeed.x * flatSpeed.x + flatSpeed.z * flatSpeed.z;
	//　最低速度を下回ったら待機状態に
	if (velocity < MIN_MOVEMENT_SPEED)
	{
		m_owner->GetStateMachine().RequestChangeState<PlayerIdle>();
	}

	gameContext;
	elapsedTime;
}

void PlayerWolk::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, m_owner->GetModel<PlayerAttack>());
}

void PlayerWolk::Exit()
{
}

bool PlayerWolk::OnInputAccept(Message::InputEvent input)
{

	auto moveAndFace = [this](SimpleMath::Vector3 move)
		{
			m_owner->Move(move);

			// 向きの計算には水平成分だけ使う
			move.y = 0.0f;
			if (move.LengthSquared() > 0.000001f)
			{
				move.Normalize();
				m_owner->AddRendVector(move);
			}
		};

	switch (input)
	{
	case Message::InputEvent::MOVEUP:
		moveAndFace(m_owner->GetForword() * Player::SPEED);
		break;

	case Message::InputEvent::MOVEDOWN:
		moveAndFace(SimpleMath::Vector3::TransformNormal(
			m_owner->GetForword() * Player::SPEED,
			SimpleMath::Matrix::CreateRotationY(XM_PI)));
		break;

	case Message::InputEvent::MOVELEFT:
		moveAndFace(SimpleMath::Vector3::TransformNormal(
			m_owner->GetForword() * Player::SPEED,
			SimpleMath::Matrix::CreateRotationY(XM_PIDIV2)));
		break;

	case Message::InputEvent::MOVERIGHT:
		moveAndFace(SimpleMath::Vector3::TransformNormal(
			m_owner->GetForword() * Player::SPEED,
			SimpleMath::Matrix::CreateRotationY(-XM_PIDIV2)));
		break;
	case Message::InputEvent::JUMP:
		m_owner->GetStateMachine().RequestChangeState<PlayerJump>();
		break;
	case Message::InputEvent::ATTACK:
		m_owner->GetStateMachine().RequestChangeState<PlayerAttack>();
		break;
	default:
		break;
	}

	return false;
}

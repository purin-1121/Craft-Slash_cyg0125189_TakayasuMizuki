#include "pch.h"
#include "BossNotice.h"
#include "BossChase.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Message/AnnouncementManager.h"

BossNotice::BossNotice(Enemy* owner)
	: BossState(owner, "Notice")
    , m_found{}
{
}

BossNotice::~BossNotice()
{
}

void BossNotice::Enter()
{
	m_playerPos = m_owner->GetPlayerPos();
	m_direction = m_playerPos - m_owner->GetPosition();
    if (!m_found)
    {
        AnnouncementManager::GetInstance()->Announce(
            {
                AnnouncementManager::AnnouncementType::BossPhaseChange,
                L"ボスに見つかった!",
                2.0f
            }
        );
        m_found = true;
    }
}

void BossNotice::Update(const GameContext& gameContext, float elapsedTime)
{
    constexpr float MIN_DIRECTION_LENGTH_SQ = 0.5f;
    m_owner->GetStateMachine().AddTimer(elapsedTime);

    // 水平方向のベクトルに矯正
    auto flatDirection = m_direction;
    flatDirection.y = 0;

    // 方向ベクトルの差があったら
    if (flatDirection.LengthSquared() > MIN_DIRECTION_LENGTH_SQ)
    {
        // プレイヤーへの方向ベクトルを正規化
        SimpleMath::Vector3 targetDir = flatDirection;
        targetDir.Normalize();

        // 敵の現在の正面ベクトルを取得する
        // （クォータニオンを使って、Z軸(0,0,1)を現在の回転で変換して正面ベクトルを作る）
        SimpleMath::Vector3 currentForward = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, m_owner->GetQuaternion());

        // 水平成分だけに
        currentForward.y = 0;
        currentForward.Normalize();

        // 敵の正面と、プレイヤーへの方向の内積を計算する
        float dot = currentForward.Dot(targetDir);

        // 向きの差が一定より小さくなった（ほぼ一致した）か判定
        constexpr float finishThresholdDot = 0.996f;
        if (dot > finishThresholdDot)
        {
            // 振り向き完了！待機状態へ
            m_owner->GetStateMachine().RequestChangeState<BossChase>();
        }
        else
        {
            // まだ向きがズレているので、Slerpで補間して振り向きを続ける
            SimpleMath::Matrix targetMatrix = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, -targetDir, SimpleMath::Vector3::Up);
            SimpleMath::Quaternion targetRotation = SimpleMath::Quaternion::CreateFromRotationMatrix(targetMatrix);

            float rotateSpeed = 5.0f; // 振り向くスピード
            float t = std::min(rotateSpeed * elapsedTime, 1.0f);

            m_owner->SetQuaternion(SimpleMath::Quaternion::Slerp(m_owner->GetQuaternion(), targetRotation, t));
        }
    }
    else
    {
        // 振り向き完了！待機状態へ
        m_owner->GetStateMachine().RequestChangeState<BossChase>();
    }
}

void BossNotice::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
    Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());

}

void BossNotice::Exit()
{
}

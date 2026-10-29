#include "pch.h"
#include "EnemyStateFactory.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/State/Actor/StunState.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyIdle.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyChase.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyAttack.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyNotice.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyDefeated.h"
#include "Game/GamePlay/State/Boss/BossIdle.h"
#include "Game/GamePlay/State/Boss/BossChase.h"
#include "Game/GamePlay/State/Boss/BossAttack.h"
#include "Game/GamePlay/State/Boss/BossNotice.h"
#include "Game/GamePlay/State/Boss/BossDefeated.h"

std::unique_ptr<EnemyStateFactory> EnemyStateFactory::s_EnemyStateFactory = nullptr;

std::unique_ptr<IGameObjectState> EnemyStateFactory::GetEnemyState(
    const std::string& enemyType, 
    StateID stateID, 
    Enemy* owner
)
{
    if (enemyType == "Infantry")
    {
        switch (stateID)
        {
        case StateID::Idle:
            return std::make_unique<EnemyIdle>(owner);
        case StateID::Stun:
            return std::make_unique<Stun>(owner);
        case StateID::Walk:
            //return std::make_unique<>(owner);
            return nullptr;
        case StateID::Run:
            return nullptr;
        case StateID::Chase:
            return std::make_unique<EnemyChase>(owner);
        case StateID::Attack:
            return std::make_unique<EnemyAttack>(owner);
        case StateID::Defeat:
            return std::make_unique<EnemyDefeated>(owner);
        case StateID::Notice:
            return std::make_unique<EnemyNotice>(owner);
        }
    }
    else if (enemyType == "Dragon")
    {
        switch (stateID)
        {
        case StateID::Idle:
            return nullptr;
        case StateID::Stun:
            return nullptr;
        case StateID::Walk:
            return nullptr;
        case StateID::Run:
            return nullptr;
        case StateID::Attack:
            return nullptr;
        case StateID::Defeat:
            return nullptr;
        }
    }
    else if (enemyType == "Ork")
    {
        switch (stateID)
        {
        case StateID::Idle:
            return nullptr;
        case StateID::Stun:
            return nullptr;
        case StateID::Walk:
            return nullptr;
        case StateID::Run:
            return nullptr;
        case StateID::Attack:
            return nullptr;
        case StateID::Defeat:
            return nullptr;
        }
    }
    else if (enemyType == "Zonbee")
    {
        switch (stateID)
        {
        case StateID::Idle:
            return nullptr;
        case StateID::Stun:
            return nullptr;
        case StateID::Walk:
            return nullptr;
        case StateID::Run:
            return nullptr;
        case StateID::Attack:
            return nullptr;
        case StateID::Defeat:
            return nullptr;
        }
    }
    else if (enemyType == "BossA")
    {
        switch (stateID)
        {
        case StateID::Idle:
            return std::make_unique<BossIdle>(owner);
        case StateID::Stun:
            return std::make_unique<Stun>(owner);
        case StateID::Chase:
            return std::make_unique<BossChase>(owner);
        case StateID::Attack:
            return std::make_unique<BossAttack>(owner, JsonDataManager<BossAttackData>::GetInstance().GetDataList(owner->GetData()->name));
        case StateID::Defeat:
            return std::make_unique<BossDefeated>(owner);
        case StateID::Notice:
            return std::make_unique<BossNotice>(owner);
        }
    }

    return nullptr;
}

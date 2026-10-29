#include "pch.h"
#include "WeaponManager.h"

#include "Common/Draw/Draw.h"
#include "Common/Manager/StorageManager.h"
#include "MeleeWeapon.h"
#include "RangedWeapon.h"
#include "Message/EventBus.h"
#include "Game/GamePlay/Data/WeaponData.h"
#include "Game/GamePlay/Actor/Actor.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Lib/MathLib.h"
#include "Lib/Camera.h"
#include "Tools/TextConverter.h"
#include "Message/AnnouncementManager.h"

using namespace DirectX;
using json = nlohmann::json;

WeaponManager::WeaponManager(Actor* owner)
	: m_currentWeapon{nullptr}
	, m_owner{owner}
	, m_requestName{"none"}
{

	auto weaponEvolved = [this](void* messanger)
		{
			if (dynamic_cast<Player*>(m_owner) == nullptr)
				return;

			std::string name = *static_cast<std::string*>(messanger);
            if (!m_isCreated[name])
            {
                m_isCreated[name] = true;
                AnnouncementManager::GetInstance()->Announce(
                    {
                        AnnouncementManager::AnnouncementType::ItemGet,
                        L"êVÇµÇ≠" + ToWString(name) + L"ÇçÏÇ¡ÇΩ!",
                        2.0f
                    }
                );
            }
			Request(name);
		};

	m_evolveID = EventBus::GetInstance()->Subscribe(
		Message::EventID::WEAPON_EVOLVED,
		weaponEvolved
	);

}
	
WeaponManager::~WeaponManager()
{
	EventBus::GetInstance()->UnSubscribe(Message::EventID::WEAPON_EVOLVED, m_evolveID);
}

void WeaponManager::Initialize(const GameContext& gameContext)
{
    
    // ïêäÌÇÃçÏê¨èÓïÒÇèâä˙âª
    for (auto& data : JsonDataManager<WeaponData>::GetInstance().GetDatas())
    {
        m_isCreated[data.first] = false;
    }
	ChangeWeapon(INIT_WEAPON.data(), gameContext);
}

void WeaponManager::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	if (m_currentWeapon)
	{
		if (!m_currentWeapon->IsAttackOn() && m_requestName != "none")
		{
			ChangeWeapon(m_requestName, gameContext);
		}
		m_currentWeapon->Update(elapsedTime, gameContext, camera);
	}
}

void WeaponManager::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
}

void WeaponManager::RenderText()
{
	
}

void WeaponManager::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{

	if (m_currentWeapon) m_currentWeapon;
}

void WeaponManager::Finalize()
{
}

void WeaponManager::ChangeWeapon(std::string name, const GameContext& gameContext)
{
    auto* data = JsonDataManager<WeaponData>::GetInstance().GetData(name);
    if (data == nullptr)
    {
        OutputDebugStringA(("Weapon data not found: " + name + "\n").c_str());
        m_requestName = "none";
        return;
    }

    std::unique_ptr<IWeapon> newWeapon;
    switch (data->attackType)
    {
    case AttackType::Melee:
        newWeapon = std::make_unique<MeleeWeapon>(data);
        break;
    case AttackType::Ranged:
        newWeapon = std::make_unique<RangedWeapon>(data);
        break;
    default:
        OutputDebugStringA("Unsupported weapon attack type.\n");
        m_requestName = "none";
        return;
    }

    newWeapon->SetOwner(m_owner);
    newWeapon->Initialize(gameContext);
    newWeapon->AttackOff();
	auto state = m_owner->GetStateMachine().GetCurrentState();
    newWeapon->SetOffsetOfState((state ? state->GetStateName() : "Idle"));

    if (m_currentWeapon)
        m_currentWeapon->Finalize();

    auto* currentWeapon = newWeapon.get();
    m_owner->SetCurrentWeapon(std::move(newWeapon));
    m_currentWeapon = currentWeapon;
    m_requestName = "none";
}
void WeaponManager::PostUpdate()
{
}





#include "pch.h"
#include "InventryMenu.h"
#include "Common/UI/Image.h"
#include <Message/EventBus.h>

InventryMenu::InventryMenu(Player& player, InputGroup* group)
    :InputGroup(group)
    , m_isOpen{}
    , m_slots{}
    , m_player{player}
{
    // 先にメモリを確保
    m_slots.reserve(InventryData::MAX_SLOT);
    
    int i = 0;
    float j = 0;

    // 各スロットの作成
    for (auto& slot : player.GetSlots())
    {
        // ボタンの配置場所を決める(縦２個、横５個)
        SimpleMath::Vector3 pos = {(SLOT_SIZE.x + SLOT_MARGIN) * i + SLOT_MARGIN, SLOT_SIZE.y + j, 0};

        auto* pSlot = &slot;

        m_slots.push_back(std::make_unique<SlotImage>(
            new FunctionButton{ [&, pSlot]()
                {
                    if (--pSlot->num <= 0)
                        pSlot->Reset();

                    EventBus::GetInstance()->Publish(Message::EventID::PICKUP_ITEM, &m_player.GetSlots());
                },
                pos,
                L"",
                this,
                SLOT_SIZE,
                true,
            },
            new FunctionButton{ [&, pSlot]()
                {
                    pSlot->Reset();
                    EventBus::GetInstance()->Publish(Message::EventID::PICKUP_ITEM, &m_player.GetSlots());
                },
                pos + DELETE_POS,
                L"全消去",
                this,
                DELETE_SIZE,
                true,
            },
            &slot
        ));
        i++;
        if (i > 4) i = 0, j = SLOT_SIZE.y + SLOT_MARGIN;
    }

}

InventryMenu::~InventryMenu()
{
}

void InventryMenu::Initialize(const GameContext& gameContext)
{
    for (auto& slot : m_slots)
    {
        slot->button->Initialize(gameContext);
        slot->deleteButton->Initialize(gameContext);
    }

    m_inventryCanvas.CreateUI("Back", new Image(SimpleMath::Vector2::Zero, SimpleMath::Color{ 0,0,0,0.7f }, SimpleMath::Vector2(DataManager::GetInstance()->ScreenX, 720)));
    m_inventryCanvas.CreateUI("WeaponImage", new Image(SimpleMath::Vector2(50, 50), SimpleMath::Color{1,1,1,0.7f}, SimpleMath::Vector2(100, 100)));
    m_inventryCanvas.Initialize(gameContext);
}

void InventryMenu::Update(float elapsedTime, const GameContext& gameContext)
{
	if (gameContext.keyboardTracker.pressed.I)
    {
        m_isOpen = !m_isOpen; // 閉じてたら開く、開いてたら閉じる
    }
    for (auto& slot : m_slots)
    {
        slot->button->Update(gameContext, elapsedTime);
        slot->deleteButton->Update(gameContext, elapsedTime);
        if (slot->slot->num > 0)
        {
            slot->button->GetText().SetText(ItemFlag::GetNameToType(slot->slot->type) + L":" + std::to_wstring(slot->slot->num));
        }
        else
        {
            slot->button->GetText().SetText(ItemFlag::GetNameToType(slot->slot->type));
        }
    }
}

void InventryMenu::Render()
{
    m_inventryCanvas.RenderText();
    for (auto& slot : m_slots)
    {
        slot->button->RenderText();
        slot->deleteButton->RenderText();
    }
}


void InventryMenu::Finalize()
{
    for (auto& slot : m_slots)
    {
        slot->button->Finalize();
        slot->deleteButton->Finalize();
    }
}

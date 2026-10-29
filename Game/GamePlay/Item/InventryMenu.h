//====================================================//
// ファイル名   : InventryMenu.h
// 作成者       : 高安瑞貴
// 作成日時     : 2026/06/15 14:08:33
//
// 概要 : 武器やアイテムを管理するインベントリのクラス
//
// 更新履歴 :
// 2026/06/15 14:08:33 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Tools/GameContext.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Button/FunctionButton.h"
#include "Game/GamePlay/Item/InventryData.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Message/InputGroup.h"


//====================================================//
// クラス宣言
//====================================================//
class InventryMenu : public InputGroup
{
public:

    // スロット専用の構造体
    struct SlotImage
    {
        std::unique_ptr<FunctionButton> button;
        std::unique_ptr<FunctionButton> deleteButton;
        InventryData::Slot* slot;

        SlotImage(FunctionButton* f, FunctionButton* dF, InventryData::Slot* slot)
            : button{std::move(f)}
            , deleteButton{dF}
            , slot{slot}
        {
            deleteButton->SetLayer(1);
        }
    };

public:

    // ---- Get ---- //
    bool IsOpen() { return m_isOpen; };

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    InventryMenu(Player& player, InputGroup* group);
    ~InventryMenu();

    // ---- 基本関数 ---- //
    void Initialize(const GameContext& gameContext);
    void Update(float elapsedTime, const GameContext& gameContext);
    void Render();
    void Finalize();

public:

    // スロットの描画サイズ
    static constexpr SimpleMath::Vector3 SLOT_SIZE = {200, 200, 1};
    // 全消去ボタンの位置、大きさ
    static constexpr SimpleMath::Vector3 DELETE_POS = { 160, 160, 1 };
    static constexpr SimpleMath::Vector3 DELETE_SIZE = { 40, 40, 1 };
    // スロットのマージン
    static constexpr float SLOT_MARGIN = 45.0f;

private:

    // 開いているか
    bool m_isOpen;
    // スロットのボタン
    std::vector<std::unique_ptr<SlotImage>> m_slots;
    // キャンバス
    Canvas m_inventryCanvas;
    // プレイヤーへのアドレス
    Player& m_player;


};
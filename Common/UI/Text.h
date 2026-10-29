//====================================================//
// ファイル名   : Text.h
// 作成者       : 高安瑞貴
//
// 概要 : UIの文字表示クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Common/UI/Common/UIObject.h"
#include "Common/Manager/DataManager.h"

//====================================================//
// 宣言
//====================================================//

class Text : public UIObject
{

public:

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="text">表示するテキスト</param>
    /// <param name="pos">表示位置</param>
    /// <param name="scale">拡大率</param>
    Text(const std::wstring& text, SimpleMath::Vector2 pos, float scale = 1.0f);

    // デストラクタ
    ~Text() override;

    // ---- 基本関数 ---- //
    void Initialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext, const float& elapsedTime) override;
    void Render(const GameContext& gameContext, const Camera& camera) override;
    void RenderText() override;

    // テキストの設定
    void SetText(const std::wstring& text) { m_text = text; };
    std::wstring GetText() { return m_text.c_str(); }

    // 文字列の幅（ピクセル単位）
    float GetWidth() { return DataManager::GetInstance()->Font_Size * m_text.length(); }

    // 初期位置取得
    SimpleMath::Vector3 GetInitPos() { return m_initPos; }
    // 初期スケール取得
    SimpleMath::Vector3 GetInitScale() { return m_initScale; }
    // 描画色設定
    void SetColor(SimpleMath::Color color) { m_color = color; }
    // 指定の始点と終点の中心に移動
    void SetCenter(SimpleMath::Vector2 center);

    void SetInitPosition(SimpleMath::Vector3 pos) { m_initPos = pos; }

private:

    std::wstring m_text;                // 表示するテキスト
    SimpleMath::Vector3 m_initPos;      // 初期位置
    SimpleMath::Vector3 m_initScale;    // 初期スケール
    SimpleMath::Color m_color;          // 描画色

};
//====================================================//
// ファイル名   : Image.h
// 作成者       : 高安瑞貴
//
// 概要 : UIの画像
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Common/UI/Common/UIObject.h"

//====================================================//
// 宣言
//====================================================//

class Image : public UIObject {

public:

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="pos">表示位置</param>
    /// <param name="color">色</param>
    /// <param name="image">表示する画像のパス</param>
    /// <param name="scale">拡大率</param>
    Image(
        SimpleMath::Vector2 pos = SimpleMath::Vector2::Zero, 
        SimpleMath::Color color = SimpleMath::Color(1.0f,1.0f,1.0f,1.0f), 
        SimpleMath::Vector2 scale = SimpleMath::Vector2::One,
        const std::wstring& image = L"Resources/Textures/white.png",
        bool active = true
    );

    // デストラクタ
    ~Image() override;

    // ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext,const float& elapsedTime) override;
	void Render(const GameContext& gameContext, const Camera& camera) override;
	void RenderText() override;


    // 画像の設定
    void SetImage(std::wstring_view& image) { m_image = image; };
    std::wstring& Get() { return m_image; }
    // 色の設定
    void SetColor(FXMVECTOR color) { m_color = color; }

    // デフォルト画像
    inline static std::wstring DEFAULT_IMAGE = L"Resources/Textures/white.png";

private:

    // 画像
    std::wstring m_image;

    // 色
    SimpleMath::Color m_color;
};
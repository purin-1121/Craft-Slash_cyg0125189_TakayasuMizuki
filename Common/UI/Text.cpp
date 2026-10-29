#include "pch.h"
#include "Text.h"
#include "Common/Draw/Draw.h"

Text::Text(const std::wstring& text, SimpleMath::Vector2 pos, float scale)
    : UIObject(SimpleMath::Vector3(pos),SimpleMath::Vector3{scale, scale, 1})
    , m_text{text}
    , m_initPos{m_worldPosition}
    , m_initScale{scale}
    , m_color{Colors::White}
{
}

Text::~Text()
{
}

void Text::Initialize(const GameContext& gameContext)
{
}

void Text::Update(const GameContext& gameContext,const float& elapsedTime)
{
}

void Text::Render(const GameContext& gameContext, const Camera& camera)
{
}

void Text::RenderText()
{
    // テキストの描画
    Draw::GetInstance().DrawText(m_text, m_worldPosition + m_posOffset, Colors::White, m_scale);
}

void Text::SetCenter(SimpleMath::Vector2 center)
{
	// 文字列のサイズを計測して中心を計算
	XMVECTOR sizeVec = Draw::GetInstance().GetRenderingContext().spriteFont.MeasureString(m_text.c_str());
	SimpleMath::Vector2 textSize(XMVectorGetX(sizeVec), XMVectorGetY(sizeVec));

	// スケールの適応
	auto currentScale = GetScale();
	textSize.x *= currentScale.x;
	textSize.y *= currentScale.y;

	// テキストの描画開始位置
	SimpleMath::Vector2 textPos = center - (textSize / 2.0f);

	// 計算した位置にテキストをセット
	SetOffset(m_posOffset);
	SetPosition(SimpleMath::Vector3(textPos.x, textPos.y, 0));
	SetInitPosition(GetPosition());
}

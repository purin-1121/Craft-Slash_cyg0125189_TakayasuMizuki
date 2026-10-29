#include "pch.h"
#include "Button.h"
#include "Common/Collider/NarrowPhase.h"
#include <WICTextureLoader.h>

Button::Button(SimpleMath::Vector3  pos)
	: Button(pos, L"")
{
}

Button::Button(SimpleMath::Vector3  pos, SimpleMath::Vector3  size)
	:Button(pos, L"", size)
{
}

Button::Button(SimpleMath::Vector3  pos, std::wstring text, SimpleMath::Vector3  size, XMVECTORF32 color)
	: UIObject(pos, size)
	, m_text{text.c_str(), SimpleMath::Vector2(pos) * 1.1f}
	, m_center{ m_worldPosition + size / 2 }
	, m_isOn{false}
	, m_click{ false }
	, m_boxColor{ color }
	, m_boxCurrentColor{m_boxColor}
	, m_textColor{Colors::Black}
	, m_onSize{size * ON_SCALE}
	, m_isScaled{false}
	, m_initScale{size}
	, m_initPos{pos}
	, m_comparision{ m_text.GetWidth() / size.x }
	, m_currentEase{0.0f}
	, m_ease{true}
	, m_textFitScale{1.0f}
{
}
Button::~Button()
{
}
void Button::Initialize(const GameContext& gameContext)
{
	m_ghHundle = ResourceManager::GetInstance().GetTexture(L"Resources/Textures/white.png");

	SimpleMath::Vector2 pos = { m_worldPosition.x, m_worldPosition.y };
	m_text.SetCenter(pos + m_scale / 2);
}

void Button::Update(const GameContext& gameContext, const float& elapsedTime)
{
	CheckClicked(gameContext);

	// 関数がセットされていたら実行
	if (m_ease && m_easing)
	{
		m_easing(elapsedTime);
	}

}

void Button::Render(const GameContext& gameContext, const Camera& camera)
{
	gameContext;
 
}

void Button::RenderText()
{
	// ボタンの描画
	SimpleMath::Vector2 pos = { m_worldPosition.x + m_posOffset.x, m_worldPosition.y + m_posOffset.y };
	Draw::GetInstance().DrawSpriteBox(m_ghHundle, pos, { m_scale.x, m_scale.y }, m_boxCurrentColor);

}

void Button::FitTextToButton()
{
	// 元のスケールでの幅を取得したいので、現在のFit倍率をリセットしてから計測
	m_text.SetScale(m_text.GetInitScale());

	auto length = m_text.GetWidth();
	const float padding = m_scale.x * 0.2f;

	// ボタンの枠に対して収める比率を計算
	m_textFitScale = 1.0f;
	if (length > (m_scale.x - padding) && length > 0.0f)
	{
		m_textFitScale = (m_scale.x - padding) / length;
	}

	// 「初期スケール × Fit倍率」を適用
	m_text.SetScale(m_text.GetInitScale() * m_textFitScale);

	// ボタンの中心に配置
	SimpleMath::Vector2 pos = { m_worldPosition.x, m_worldPosition.y };
	m_text.SetCenter(pos + m_scale / 2);
}

bool Button::CheckClicked(const GameContext& gameContext)
{
    auto mouse = Mouse::Get().GetState(); 
    
    // 当たり判定
	m_isOn = NarrowPhase::IsColliding(SimpleMath::Vector2{ static_cast<float>(mouse.x), static_cast<float>(mouse.y) },
											PlaneCollider{ m_worldPosition + m_posOffset, m_scale, SimpleMath::Quaternion()}).is;

	// 前のフレームと状態が変わっていたら
	if (m_isOn != m_isScaled)
	{
		m_isScaled = m_isOn;
		m_endSize = (m_isScaled ? m_onSize : m_initScale);

		// 関数セット
		m_easing = [&](float elapsedTime) {
			// 大きさ変更
			auto scale = Easing::EaseOut(m_currentEase, m_scale, m_endSize - m_scale, EASING_TIME);
			m_scale = scale;
			// 時間計測
			m_currentEase += elapsedTime;
			// 大きくなった差分を計算
			auto dif = m_scale - m_initScale;
			// 補正用に半分
			dif /= 2;
			// 位置補正
			m_worldPosition = SimpleMath::Vector3(m_initPos.x - dif.x, m_initPos.y - dif.y, m_worldPosition.z);

			// 文字の大きさも変更
			auto mult = (m_isScaled ? ON_SCALE : 1.0f);

			m_text.SetScale(m_text.GetInitScale() * m_textFitScale * (m_scale / m_endSize) * mult);

			m_text.SetPosition(m_text.GetInitPos() - dif * m_comparision);

			if (m_currentEase > EASING_TIME)
			{
				m_easing = nullptr;
				m_scale = m_endSize;
			}
		};
		m_currentEase = 0;
	}
	
	return false;
}

#include "pch.h"
#include "Bar.h"

template<typename T>
Bar<T>::Bar(T& value, SimpleMath::Vector3 position, float width, float height,  XMVECTORF32 color, T max, T min, T current)
	: UIObject(position, {width, height, 1})
	, m_barPosition{ position +SimpleMath::Vector2{width * 0.05f, height * 0.1f} }
	, m_barScale{SimpleMath::Vector2{width * 0.9f, height * 0.8f} }
	, m_barColor{color}
	, m_primerColor{ Colors::Gray}
	, m_raito{1.0f}
	, m_rendRaito{ 0 }
	, m_isInit{}
	, m_max{ max }
	, m_min{min}
	, m_initValue{value}
	, m_currentValue{value}
{
	m_currentValue = m_initValue;
}

template<typename T>
Bar<T>::~Bar()
{
}

template<typename T>
void Bar<T>::Initialize(const GameContext& gameContext)
{
	m_raito = 1.0f;
	m_rendRaito = 0;
	m_isInit = false;

	m_ghHundle = ResourceManager::GetInstance().GetTexture(L"Resources/Textures/white.png");
 
}

template<typename T>
void Bar<T>::Update(const GameContext& gameContext,const float& elapsedTime)
{
	gameContext;
	// 初期化前だったら
	if (!m_isInit)
	{
		// ゲージの表示割合を増やしていく
		m_rendRaito += 1.5f * elapsedTime;
	}
	// 表示割合が実際の割合を超えたら
	if (m_rendRaito >= m_raito)
	{
		// 初期化済み
		m_isInit = true;
		// 毎フレーム割合を減らしていく
		m_rendRaito -= 1.0f * elapsedTime;
	}
	// 0以下阻止
	if (m_rendRaito < 0) m_rendRaito = 0;

	// 割合更新
	m_raito = static_cast<float>(m_currentValue) / m_initValue;
	// 上限超えないように
	if (m_raito > 1) m_raito = 1;
}

template<typename T>
void Bar<T>::Render(const GameContext& gameContext, const Camera& camera)
{
	camera;
	gameContext;
 
}

template<typename T>
void Bar<T>::RenderText()
{
	// 下地の部分の描画
	Draw::GetInstance().DrawSpriteBox(m_ghHundle, { m_worldPosition.x, m_worldPosition.y }, { m_scale.x, m_scale.y}, m_primerColor);

	// バーの部分の描画
	auto scale = SimpleMath::Vector2(m_barScale.x * m_rendRaito, m_barScale.y);
	Draw::GetInstance().DrawSpriteBox(m_ghHundle, m_barPosition, scale, m_barColor);

	// ---- デバッグ用 ---- //
}

template<typename T>
void Bar<T>::SetRaito(float raito)
{
	m_raito = raito;
}

template class Bar<int>;
template class Bar<float>;
template class Bar<double>;
#include "pch.h"
#include "Slider.h"

template<typename T>
Slider<T>::Slider(T& target,SimpleMath::Vector3 position, float width, float height, XMVECTORF32 color, T max, T min, T current, bool isCross)
	: Bar<T>(target, position, width, height, color, max, min, current)
	, m_thumb{ SimpleMath::Vector3{position.x, position.y, 1}, L"", SimpleMath::Vector3{HUNDLE_WIDTH, height, 0}, color }
	, m_catched{}
	, m_catchX{}
	, m_catchY{}
	, m_isCross{isCross}
{
	this->m_initValue = (current >= this->m_min ? current : this->m_min);
	if (!m_isCross)
		m_thumb.SetScale({ width, 10.0f, 0.0f });
}

template<typename T>
Slider<T>::~Slider<T>()
{
}

template<typename T>
void Slider<T>::Initialize(const GameContext& gameContext)
{
	using ui = UIObject;

	this->m_isInit = false;
	// 画像読みこみ
	this->m_ghHundle = ResourceManager::GetInstance().GetTexture(L"Resources/Textures/white.png");

	// つまみの初期化
	m_thumb.Initialize(gameContext);
	
	// 初期値をもとに割合変更
	this->m_raito = static_cast<float>((float)(this->m_initValue - this->m_min) / (this->m_max - this->m_min));
	// 初期値から現在の位置を計算
	this->m_currentValue = (this->m_max - this->m_min) * this->m_raito + this->m_min;
	// つまみの位置を設定
	m_thumb.SetPosition(SimpleMath::Vector3{ ui::GetScale().x * this->m_raito + ui::GetPosition().x + ui::GetOffset().x,
		m_thumb.GetPosition().y + ui::GetOffset().y,
		0 });
	m_thumb.SetEase(false);
	m_thumb.SetOffset(ui::GetOffset());
}


template<typename T>
void Slider<T>::Update(const GameContext& gameContext,const float& elapsedTime)
{
	using ui = UIObject;

	// マウスの状態
	auto mouse = Mouse::Get().GetState();
	m_thumb.Update(gameContext, elapsedTime);
	m_thumb.SetOffset(ui::GetOffset());

	// 押されたら
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED && m_thumb.MouseOn())
	{
		// つかみ状態オン
		m_catched = true;
		// つかんだ位置記録
		m_catchX = { mouse.x };
		m_catchY = { mouse.y };
	}
	// 離したら
	else if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		// つかみ状態オフ
		m_catched = false;
	}

	// つかみ状態中
	if (m_catched)
	{
		if (m_isCross) ClampX(mouse);
		else ClampY(mouse);
		// 最大値と最小値の差と割合で値を計算して最小値を足す
		this->m_currentValue = (this->m_max - this->m_min) * this->m_raito + this->m_min;
	}

}

template<typename T>
void Slider<T>::Render(const GameContext& gameContext, const Camera& camera)
{
}

template<typename T>
void Slider<T>::RenderText()
{
	using ui = UIObject;

	// バーの部分の描画
	Draw::GetInstance().DrawSpriteBox(this->m_ghHundle, { ui::GetPosition().x + ui::GetOffset().x, ui::GetPosition().y + ui::GetOffset().y }, {ui::GetScale().x, ui::GetScale().y}, this->m_primerColor);

	// 値の描画
	Draw::GetInstance().DrawText(std::to_wstring(this->m_currentValue), { ui::GetPosition().x + ui::GetOffset().x + ui::GetScale().x + 20.0f, ui::GetPosition().y + ui::GetOffset().y, 0});

	m_thumb.RenderText();
}

template<typename T>
void Slider<T>::SetRaito(float raito)
{
	this->m_raito = raito;
}

template<typename T>
void Slider<T>::ClampX(Mouse::State mouse)
{
	using ui = UIObject;

	// マウスの位置と前フレームのマウスの位置の差
	float delta = static_cast<float>(mouse.x - m_catchX);

	SimpleMath::Vector3 vec = {
		(float)mouse.x - ui::GetOffset().x,
		m_thumb.GetPosition().y,
		0
	};

	// 位置を背景の布地内にクランプ
	vec.x = MathLib::Clamp(vec.x, ui::GetPosition().x, ui::GetPosition().x + ui::GetScale().x);

	// つかみを移動
	m_thumb.SetPosition(vec);
	// このフレームのマウス位置記録
	m_catchX = mouse.x;

	// 左端からつまみまでの距離をスケールで割って割合を計算
	this->m_raito = ((float)m_thumb.GetPosition().x - ui::GetPosition().x) / ui::GetScale().x;
}

template<typename T>
void Slider<T>::ClampY(Mouse::State mouse)
{
	using ui = UIObject;

	// マウスの位置と前フレームのマウスの位置の差
	float delta = static_cast<float>(mouse.y - m_catchY);

	SimpleMath::Vector3 vec = {
		m_thumb.GetPosition().x,
		(float)mouse.y - ui::GetOffset().y,
		0
	};

	// 位置を背景の布地内にクランプ
	vec.y = MathLib::Clamp(vec.y, ui::GetPosition().y, ui::GetPosition().y + ui::GetScale().y);

	// つかみを移動
	m_thumb.SetPosition(vec);
	// このフレームのマウス位置記録
	m_catchY = mouse.y;

	// 左端からつまみまでの距離をスケールで割って割合を計算
	this->m_raito = ((float)m_thumb.GetPosition().y - ui::GetPosition().y) / ui::GetScale().y;
}


template class Slider<int>;
template class Slider<float>;
template class Slider<double>;
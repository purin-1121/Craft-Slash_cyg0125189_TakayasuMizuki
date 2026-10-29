#include "pch.h"
#include "InputField.h"
#include "Common/Manager/InputManager.h"

InputField::InputField(Type type,SimpleMath::Vector3 pos, std::wstring text,  SimpleMath::Vector3 size, XMVECTORF32 color, bool is)
	: Button(pos, text, size, color)
	, m_isSelected{}
	, m_inputText{L""}
	, m_inputType{type}
{
}
void InputField::Initialize(const GameContext& gameContext)
{
	Button::Initialize(gameContext);
	auto pos = m_worldPosition;
	pos.y -= 50.0f;
	m_text.SetPosition(pos);
	m_textColor = Colors::Black;
}
void InputField::Update(const GameContext& gameContext, const float& elapsedTime)
{
	// 入力確認
	CheckClicked(gameContext);

	// 左クリックでこのフィールドを選択
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		// のっかっていたら
		if (m_isOn)
		{
			// モード反転
			m_isSelected = true;
			m_boxCurrentColor = m_selectedColor;
			m_click = true;
			EventBus::GetInstance()->Publish(Message::EventID::INPUT_START, this);
			return;
		}
		SelectOff();
	}

	// 選択外だったら何もしない
	if (!m_isSelected)
		return;

	// 文字入力
	auto text = InputManager::GetInstance()->GetInputChar(gameContext);
	// 数字タイプだったら数字以外の中身を破棄する
	if (m_inputType == Type::num && text != L"")
	{
		if (text != L"BS" && text != L"-" && text != L"." && (text > L"9" || text < L"0"))
		{
			text = L"";
		}
	}

	if (text == L"BS")
	{
		if (!m_inputText.empty())
		{
			m_inputText.pop_back();
		}
	}
	else if(text != L"")
	{
		m_inputText += text;
	}

	//m_text.SetOffset(m_posOffset);
}
void InputField::Render(const GameContext& gameContext, const Camera& camera)
{
}
void InputField::RenderText()
{
	Button::RenderText();
	m_text.RenderText();
	Draw::GetInstance().DrawText(m_inputText, m_worldPosition, Colors::Black);
}


float InputField::GetNum()
{
	// 入力が何もなかったらやらない
	if (m_inputText.empty())
		return 0;

	bool isNegative = false;
	float integerPart = 0;
	float fractionPart = 0;
	float fractionDivisor = 1;
	bool afterDecimal = false;

	for (wchar_t ch : m_inputText)
	{
		if (ch == L'-')
		{
			isNegative = true;
		}
		else if (ch == L'.')
		{
			afterDecimal = true;
		}
		else if (ch >= L'0' && ch <= L'9')
		{
			int digit = ch - L'0';
			if (!afterDecimal)
			{
				integerPart = integerPart * 10.0f + digit;
			}
			else
			{
				fractionDivisor *= 10.0f;
				fractionPart += digit / fractionDivisor;
			}
		}
	}

	float result = integerPart + fractionPart;
	if (isNegative)
		result *= -1.0f;

	return result;
}
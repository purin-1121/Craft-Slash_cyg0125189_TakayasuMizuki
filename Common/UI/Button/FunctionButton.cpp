#include "pch.h"
#include "FunctionButton.h"
#include <Common/Manager/InputManager.h>


FunctionButton::FunctionButton(std::function<void()> f, SimpleMath::Vector3 pos, std::wstring text, InputGroup* group, SimpleMath::Vector3 size, bool is)
	: Button(pos, text, size)
	, m_clickFunction(f)
	, m_isOnText(is) 
	, IInputObserver(group)
{

}
FunctionButton::FunctionButton(std::function<void()> f, SimpleMath::Vector3 pos, InputGroup* group, SimpleMath::Vector3 size)
	: Button(pos, L"", size)
	, m_clickFunction{f}
	, m_isOnText{false}
	, IInputObserver(group)
{
}
void FunctionButton::Initialize(const GameContext& gameContext)
{
	Button::Initialize(gameContext);
	FitTextToButton();
}
void FunctionButton::Update(const GameContext& gameContext, const float& elapsedTime)
{
	Button::Update(gameContext, elapsedTime);

	m_text.SetOffset(m_posOffset);
	
}
void FunctionButton::Render(const GameContext& gameContext, const Camera& camera)
{
}
void FunctionButton::RenderText()
{
	Button::RenderText();
	if (m_isOnText)
	{
		m_text.RenderText();
	}
}

void FunctionButton::Finalize()
{
}

bool FunctionButton::OnInputAccept(Message::InputEvent input)
{
	if (input == Message::InputEvent::BUTTONINVOKE)
	{
		// ‚Ì‚Á‚©‚Á‚Ä‚¢‚½‚ç
		if (m_isOn)
		{
			m_clickFunction();
			m_click = true;
			ResourceManager::GetInstance().GetSoundManager()->PlayOneShot(L"Click");
			return true;
		}
	}
	return false;
}

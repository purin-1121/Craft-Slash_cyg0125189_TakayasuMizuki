#include "pch.h" 
#include "InputManager.h"
#include "Message/EventBus.h"
#include "Tools/TextConverter.h"

InputManager* InputManager::s_InputManager = nullptr;

std::wstring InputManager::GetInputChar(const GameContext& gameContext)
{
	auto state = Keyboard::Get().GetState();
	auto kb = gameContext.keyboardTracker.pressed;
	auto lastkb = gameContext.keyboardTracker.GetLastState();
	bool isShift = false;

	if (state.LeftShift) 
		isShift = true;
	if (kb.A) return IsBig(L"a", isShift);
	if (kb.B) 
		return IsBig(L"b", isShift);
	if (kb.C) return IsBig(L"c", isShift);
	if (kb.D) return IsBig(L"d", isShift);
	if (kb.E) return IsBig(L"e", isShift);
	if (kb.F) return IsBig(L"f", isShift);
	if (kb.G) return IsBig(L"g", isShift);
	if (kb.H) return IsBig(L"h", isShift);
	if (kb.I) return IsBig(L"i", isShift);
	if (kb.J) return IsBig(L"j", isShift);
	if (kb.K) return IsBig(L"k", isShift);
	if (kb.L) return IsBig(L"l", isShift);
	if (kb.N) return IsBig(L"n", isShift);
	if (kb.M) return IsBig(L"m", isShift);
	if (kb.O) return IsBig(L"o", isShift);
	if (kb.P) return IsBig(L"p", isShift);
	if (kb.Q) return IsBig(L"q", isShift);
	if (kb.R) return IsBig(L"r", isShift);
	if (kb.S) return IsBig(L"s", isShift);
	if (kb.T) return IsBig(L"t", isShift);
	if (kb.U) return IsBig(L"u", isShift);
	if (kb.V) return IsBig(L"v", isShift);
	if (kb.W) return IsBig(L"w", isShift);
	if (kb.X) return IsBig(L"x", isShift);
	if (kb.Y) return IsBig(L"y", isShift);
	if (kb.Z) return IsBig(L"z", isShift);
	if (kb.D1) return L"1";
	if (kb.D2) return L"2";
	if (kb.D3) return L"3";
	if (kb.D4) return L"4";
	if (kb.D5) return L"5";
	if (kb.D6) return L"6";
	if (kb.D7) return L"7";
	if (kb.D8) return L"8";
	if (kb.D9) return L"9";
	if (kb.D0) return L"0";
	if (kb.Back) return L"BS";
	if (kb.IsKeyDown(Keyboard::Keys::OemMinus)) return L"-";
	if (kb.IsKeyDown(Keyboard::Keys::OemPeriod)) return L".";

	return L"";

}

void InputManager::Update(const Keyboard::KeyboardStateTracker& buttonTracker, const Mouse::ButtonStateTracker& mouseTracker)
{
	auto state = Keyboard::Get().GetState();
	auto mouse = Mouse::Get().GetState();

	using Event = Message::InputEvent;
	using Tracker = Mouse::ButtonStateTracker;

	// キー入力を定義する
	std::vector<KeyBinding> keyBindings =
	{
		{state.W,										Event::MOVEUP},			// 前進
		{state.A,										Event::MOVELEFT},		// 左移動
		{state.S,										Event::MOVEDOWN},		// 後退
		{state.D,										Event::MOVERIGHT},		// 右移動
		{state.Space,									Event::JUMP},			// ジャンプ
		{buttonTracker.pressed.Escape,					Event::PAUSE},			// ポーズメニュー
		{buttonTracker.pressed.E,						Event::EDIT},			// エディットモード
		{buttonTracker.pressed.I,						Event::INVENTRY},		// インベントリメニュー
		{buttonTracker.pressed.K,						Event::ATTACK},			// 攻撃
		{buttonTracker.pressed.L,						Event::OPEN},			// 攻撃
		{buttonTracker.pressed.F2,						Event::RENDMODE},		// 描画モード
		{buttonTracker.pressed.LeftAlt,					Event::CAMERADASH},		// カメラの加速
		{buttonTracker.pressed.Tab,						Event::RELOAD},			// データ再読み込み
		{mouseTracker.leftButton == Tracker::PRESSED,	Event::BUTTONINVOKE},	// ボタン実行
		{mouseTracker.leftButton == Tracker::PRESSED,	Event::ATTACK},			// 攻撃
		{mouseTracker.rightButton == Tracker::PRESSED,	Event::OPEN},			// 宝箱開閉
		{mouse.scrollWheelValue != m_lastMouseTracker.GetLastState().scrollWheelValue,Event::CAMERAVVERTICAL},  // カメラ上昇		
	};

	// 入力されたキーだけ送信する
	for (auto& binding : keyBindings)
	{
		if (binding.isDown)
		{
			// 送信先がシステムか判別
			if (IsSystemInput(binding.message))
			{
				SystemInputNotify(binding.message);
			}
			else if (binding.message == Event::RELOAD)
			{
				EventBus::GetInstance()->Publish(Message::EventID::RELOAD, nullptr);
			}
			else
			{
				Notify(binding.message);
			}
		}
	}

	m_lastButtonTracker = buttonTracker;
	m_lastMouseTracker = mouseTracker;

}

void InputManager::Notify(Message::InputEvent input)
{
	OnInputAccept(input);
}

bool InputManager::IsSystemInput(Message::InputEvent input)
{
	// システム判定
	return (input == Message::InputEvent::PAUSE) || 
		(input == Message::InputEvent::EDIT) ||
		(input == Message::InputEvent::INVENTRY) ||
		(input == Message::InputEvent::RENDMODE);
}

void InputManager::SystemInputNotify(Message::InputEvent input)
{
	// システム役に送信
	switch (input)
	{
	case Message::InputEvent::PAUSE:
		EventBus::GetInstance()->Publish(Message::EventID::PAUSE, 0);
		break;
	case Message::InputEvent::EDIT:
		EventBus::GetInstance()->Publish(Message::EventID::EDIT, this);
		break;
	case Message::InputEvent::INVENTRY:
		EventBus::GetInstance()->Publish(Message::EventID::INVENTRY, 0);
		break;
	case Message::InputEvent::RENDMODE:
		EventBus::GetInstance()->Publish(Message::EventID::RENDMODE, 0);
		break;
	default:
		break;
	}
}

std::wstring InputManager::IsBig(const std::wstring& str, bool is)
{
	return (is ? ToBig(str) : str);
}

InputManager* InputManager::GetInstance()
{
	// インスタンスが生成されていない場合
	if (s_InputManager == nullptr)
	{
		// InputManagerクラスのインスタンスを生成し、インスタンスをm_InputManagerに格納する
		s_InputManager = new InputManager();
	}
	// InputManagerクラスのインスタンスを返す
	return s_InputManager;
}

InputManager& InputManager::GetRefInstance()
{
	// インスタンスが生成されていない場合
	if (s_InputManager == nullptr)
	{
		// InputManagerクラスのインスタンスを生成し、インスタンスをs_InputManagerに格納する
		s_InputManager = new InputManager();
	}
	// InputManagerクラスのインスタンスを返す
	return *s_InputManager;
}

void InputManager::Dispose()
{
	// インスタンスが生成されている場合
	if (s_InputManager != nullptr)
	{
		// InputManagerクラスのインスタンスを破棄する
		delete s_InputManager;
		// InputManagerクラスのインスタンス変数を初期化する
		s_InputManager = nullptr;
	}
}

//====================================================//
// ファイル名   : InputManager.h
// 作成者       : 高安瑞貴
//
// 概要 : 入力を検知し管理するクラス
//====================================================//

#pragma once
#include <KeyBoard.h>
#include "Tools/GameContext.h"
#include "InterFace/IInputObserver.h"
#include "Message/InputGroup.h"

//====================================================//
// インクルードファイル
//====================================================//

//====================================================//
// 宣言
//====================================================//

class IInputObserver;

// キー入力対応表の要素（押下判定 / 部品オフセット / メッセージ）
struct KeyBinding
{
	// フレームで押されているか
	bool isDown;
	// 通知するメッセージ
	Message::InputEvent message;
};

// すべてシーン間で扱うデータを管理するクラス
class InputManager final : public InputGroup
{
public:


	// このフレームで入力された文字を返す(とりあえず半角英字と数字だけ)
	static std::wstring GetInputChar(const GameContext& gameContext);

	// 入力に応じて命令を流す(プレイ中)
	void Update(const Keyboard::KeyboardStateTracker& buttonTracker, const Mouse::ButtonStateTracker& mouseTracker);
	// 入力に応じて命令を流す(エディット中)
	//void EditUpdate(const Keyboard::KeyboardStateTracker& buttonTracker, const Mouse::ButtonStateTracker& mouseTracker);

public:
	// InputManagerクラスのインスタンスを取得する
	static InputManager* GetInstance();

	// InputManagerクラスのインスタンスへの参照を取得する
	static InputManager& GetRefInstance();
	

	// InputManagerクラスのインスタンスを破棄する
	static void Dispose();

	
private:
	// InputManagerクラスのインスタンスへのポインタ
	static InputManager* s_InputManager;

	// 前回の状態
	DirectX::Keyboard::KeyboardStateTracker m_lastButtonTracker;
	DirectX::Mouse::ButtonStateTracker    m_lastMouseTracker;

private:
	// コンストラクタ
	InputManager() : InputGroup(nullptr){};
	// デストラクタ
	~InputManager() {};

	// 送信
	void Notify(Message::InputEvent input);
	// システム入力かどうか
	bool IsSystemInput(Message::InputEvent input);
	// システム入力の実行
	void SystemInputNotify(Message::InputEvent input);
	// 大文字か小文字か
	static std::wstring IsBig(const std::wstring& str, bool is);


};


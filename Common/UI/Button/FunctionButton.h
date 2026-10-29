//====================================================//
// ファイル名   : FunctionButton.h
// 作成者       : 高安瑞貴
//
// 概要 : 機能を実装したボタンクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include <functional>
#include "Button.h"
#include "InterFace/IInputObserver.h"

//====================================================//
// 宣言
//====================================================//


class FunctionButton : public Button, public IInputObserver
{

public:


	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="f">クリック時に実行する関数</param>
	/// <param name="pos">ボタンの位置</param>
	/// <param name="text">ボタンのテキスト</param>
	/// <param name="size">ボタンのサイズ</param>
	FunctionButton(std::function<void()> f, 
				   SimpleMath::Vector3 pos, 
				   std::wstring text, 
				   InputGroup* group,
				   SimpleMath::Vector3 size = {40, 40, 1},
				   bool is = true);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="f">クリック時に実行する関数</param>
	/// <param name="pos">ボタンの位置</param>
	/// <param name="size">ボタンのサイズ</param>
	FunctionButton(std::function<void()> f,
				   SimpleMath::Vector3 pos,
				   InputGroup* group,
				   SimpleMath::Vector3 size = { 40, 40, 1 });

	// ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext,const float& elapsedTime) override;
	void Render(const GameContext& gameContext, const Camera& camera) override;
	void RenderText() override;
	void Finalize() override;

	bool OnInputAccept(Message::InputEvent input) override;
	void InVoke() { m_clickFunction(); }

private:

	std::function<void()> m_clickFunction;
	bool m_isOnText;
	


};


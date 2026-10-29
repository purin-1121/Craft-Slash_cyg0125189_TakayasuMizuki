//====================================================//
// ファイル名   : InputField.h
// 作成者       : 高安瑞貴
//
// 概要 : 数字入力専用クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include <functional>
#include "Button.h"
#include "Message/EventBus.h"

//====================================================//
// 宣言
//====================================================//


class InputField : public Button
{

public:

	enum class Type
	{
		num,
		text,
	};

	float GetNum();
	std::wstring GetText() { return m_inputText; }

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">位置</param>
	/// <param name="text">内容のテキスト</param>
	/// <param name="size">サイズ</param>
	InputField(
		Type type,
		SimpleMath::Vector3 pos, 
		std::wstring text, 
		SimpleMath::Vector3 size = {40, 40, 1},
		XMVECTORF32 color = m_defaultColor,
		bool is = true
	);

	// ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext,const float& elapsedTime) override;
	void Render(const GameContext& gameContext, const Camera& camera) override;
	void RenderText() override;

	void Set(std::wstring text) { 
		auto t = m_inputText;
		m_inputText = text; 
	}

	// 選択矯正解除
	void SelectOff() { 
		EventBus::GetInstance()->Publish(Message::EventID::INPUT_START, nullptr);
		m_isSelected = false;
		m_boxCurrentColor = m_defaultColor;
	}

private:

	// このフィールドが選択中か
	bool m_isSelected;

	// 入力された文字
	std::wstring m_inputText;

	// 入力最大文字数
	static constexpr int MAX_CHARS = 20;

	// このフィールドの入力タイプ
	Type m_inputType;

	// デフォルトカラー
	static constexpr XMVECTORF32 m_defaultColor = { 0.7, 0.7, 0.7, 0.5 };
	// 選択カラー
	static constexpr XMVECTORF32 m_selectedColor = { 0.9, 0.9, 0.9, 0.5 };

};


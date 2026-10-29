//====================================================//
// ファイル名   : Bar.h
// 作成者       : 高安瑞貴
//
// 概要 : UIのボタンクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include "Common/UI/Common/UIObject.h"
#include "Common/UI/Text.h"

//====================================================//
// 宣言
//====================================================//



using namespace DirectX;

class Button : public UIObject
{

public:

	// ---- コンストラクタ ---- //
	Button(SimpleMath::Vector3 pos);
	Button(SimpleMath::Vector3 pos, SimpleMath::Vector3 size);
	Button(SimpleMath::Vector3 pos, std::wstring text, SimpleMath::Vector3 size = { 30, 30, 1 }, XMVECTORF32 color = Colors::Gray);
	
	// デストラクタ
	~Button() override;

	// 基本関数
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext, const float& elapsedTime) override;
	virtual void Render(const GameContext& gameContext, const Camera& camera) override;
	virtual void RenderText() override;

	// 色設定
	void SetColor(XMVECTORF32 box, XMVECTORF32 text) { m_boxColor = box, m_textColor = text; };
	// 文字取得
	Text& GetText() { return m_text; }
	// マウスが上にあるか
	bool MouseOn() override { return m_isOn; }
	// イージングするか設定
	void SetEase(bool is) { m_ease = is; }
	// 初期位置設定
	void SetInitPos(SimpleMath::Vector3 init) { 
		m_initPos = init;
		m_text.SetPosition(init * 1.1f);
	}
	// ボタンの中にテキストをおさまるようにする
	void FitTextToButton();

public:

	static constexpr float ON_SCALE = 1.1f;	// カーソルが乗っているときの大きさ拡大率
	static constexpr float EASING_TIME = 0.35f;	// イージングにかかる時間

protected:

	bool m_isOn;	// カーソルが乗っているかどうか
	bool m_click;	// このフレームでクリックされたか

	SimpleMath::Vector3 m_center;	// ボタンの中心位置
	SimpleMath::Vector3 m_textPos;	// テキストの表示位置
	SimpleMath::Vector3 m_onSize;	// カーソルが乗っているときのサイズ

	Text m_text;	// ボタンの中に表示させる文字列

	XMVECTORF32 m_boxColor;	// ボックスの色
	XMVECTORF32 m_boxCurrentColor;	// ボックスの色
	XMVECTORF32 m_textColor;	// 文字色

	float m_comparision;	// ボックスの大きさとテキストの位置の比率
	float m_textFitScale;

	// ---- イージング関連 ---- //
	bool m_isScaled;	//	ボタンの大きさが変更中かどうか
	SimpleMath::Vector3 m_endSize;	// 終着点
	bool m_ease;			// イージングするか

	std::function<void(float)> m_easing;// 呼び出すイージング関数
	float m_currentEase;	// 現在のイージングにかかった時間
	SimpleMath::Vector3 m_initScale;	// 初期の大きさ
	SimpleMath::Vector3 m_initPos;	// 初期の位置

protected:

	// クリックの判定
	bool CheckClicked(const GameContext& gameContext);

	// ----- デバッグ用 ----- //
public:
};


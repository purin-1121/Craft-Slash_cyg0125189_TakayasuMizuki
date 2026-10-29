//====================================================//
// ファイル名   : Slider.h
// 作成者       : 高安瑞貴
//
// 概要 : スライダークラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Common/UI/Common/UIObject.h"
#include "Bar.h"
#include "Common/UI/Button/Button.h"

//====================================================//
// 宣言
//====================================================//

template<typename T>
class Slider : public Bar<T>
{

public:

	Button* GetButton() { return &m_thumb; }


	// メンバ関数--------------
public:

	//コンストラクタ
	Slider(T& target, SimpleMath::Vector3 position, float width, float height, XMVECTORF32 color, T max = 1, T min = 0, T current = -1, bool isCross = true);
	~Slider() override;

	// 基本関数
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext,const float& elapsedTime) override;
	void Render(const GameContext& gameContext, const Camera& camera) override;// バーの割合を設定する関数(0.0f ~ 1.0f)
	void RenderText() override;void SetRaito(float raito);


	// メンバ変数--------------
private:

	// つまみ部分
	Button m_thumb;
	// つまみを選択しているかどうか
	bool m_catched;
	// つかみ始めた位置
	int m_catchX;
	int m_catchY;
	// 楯向きか横向きか
	bool m_isCross;


	static constexpr float HUNDLE_WIDTH = 5.0f;

	void ClampX(Mouse::State);
	void ClampY(Mouse::State);

};


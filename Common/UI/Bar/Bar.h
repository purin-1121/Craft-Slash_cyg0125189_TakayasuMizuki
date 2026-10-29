//====================================================//
// ファイル名   : Bar.h
// 作成者       : 高安瑞貴
//
// 概要 : UIのバークラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Common/UI/Common/UIObject.h"

//====================================================//
// 宣言
//====================================================//

template<typename T>
class Bar : public UIObject
{
	
	// メンバ関数--------------
public:

	//コンストラクタ
	Bar(T& value,  SimpleMath::Vector3 position, float width, float height,  XMVECTORF32 color, T max = 1, T min = 0, T current = -1);
	~Bar() override;

	// 基本クラス
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext,const float& elapsedTime) override;
	void Render(const GameContext& gameContext, const Camera& camera) override;
	void RenderText() override;
	// バーの割合を設定する関数(0.0f ~ 1.0f)
	void SetRaito(float raito);

	// 割合を取得
	float GetRaito() { return m_rendRaito; }
	// 最大値を設定
	void SetMax(T max) { m_max = max; }


	// メンバ変数--------------
protected:

	//　バー部分の位置
	 SimpleMath::Vector2 m_barPosition;
	 SimpleMath::Vector2 m_barScale;

	 XMVECTORF32 m_barColor;	// バーの色
	 XMVECTORF32 m_primerColor;	// バーの下地の色

	//　割合(0.0f, 1.0f)
	float m_raito;

	// 表示している割合
	float m_rendRaito;

	// 初期状態にあるか(0->1になったら初期状態解除)
	bool m_isInit;

	// 初期値
	T m_initValue;
	// 値
	T& m_currentValue;
	// 最大値
	T m_max;
	// 最小値
	T m_min;


};


//====================================================//
// ファイル名   : GameDataType.h
// 作成者       : 高安瑞貴
//
// 概要 : ゲームで扱う構造体を管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

//====================================================//
// 宣言
//====================================================//

// データ
namespace Datas
{
	extern const int FONT_SIZE;
};

// 方向
enum class Direction
{
	top,
	down,
	left,
	right,
	front, 
	back,
	null,
};

// 渡された方向の逆を返す関数
inline Direction GetReverseDirection(const Direction& d)
{
	switch (d)
	{
	case Direction::top:
		return Direction::down;
	case Direction::down:
		return Direction::top;
	case Direction::left:
		return Direction::right;
	case Direction::right:
		return Direction::left;
	case Direction::front:
		return Direction::back;
	case Direction::back:
		return Direction::front;
	default:
		return Direction::null;
	}
}

// 地面にいるかどうか
enum class GroundState
{
	Ground,
	Fly,
};

// 描画状態
enum class RendState : unsigned int
{
    None      = 0b0000'0000,
    Object    = 0b0000'0001,
    Collision = 0b0000'0010,
    All       = 0b0000'0011,
};

// 前置インクリメント (++state) のオーバーロード
inline RendState& operator++(RendState& a) {
    // 一度数値に戻してインクリメントし、0?3 (0b00?0b11) の範囲で巡回させる
    unsigned int val = static_cast<unsigned int>(a);
    val = (val + 1) % static_cast<unsigned int>(RendState::All) + 1; 
    a = static_cast<RendState>(val);
    return a;
}

// 判定用の & 演算子
inline bool operator&(RendState a, RendState b) {
    return (static_cast<unsigned int>(a) & static_cast<unsigned int>(b)) != 0;
}


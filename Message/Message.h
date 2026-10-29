//--------------------------------------------------------------------------------------
// File: Message.h
//
// オブジェクトに送るメッセージの種類管理
//
// Date: 2026.7.15
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once

class Message
{

public:

	// イベント一覧
	enum class EventID
	{
		PLAYER_TAKE_DAMAGE,		// プレイヤー
		ENEMY_TAKE_DAMAGE,		// 敵
		PICKUP_ITEM,			// アイテム取得
		WEAPON_EVOLVED,			// 武器進化
		DESTROY,				// 破壊可能オブジェクト
		OPEN,					// インベントリ、チェスト、ドア
		CLOSE,					// インベントリ、ドア
		ON,						// ポーズ、エディット
		OFF,					// ポーズ、エディット	
		RESTART,				// ゲームプレイ
		PAUSE,					// ポーズ
		EDIT,					// エディット
		INVENTRY,				// インベントリ
		RENDMODE,				// 描画モード	
		BOSSDEFEATED,			// ボス撃破
		FAIL,					// 失敗
		INPUT_START,			// 入力開始
		RELOAD,					// データ再読み込み
		SET_SPAWN_AREA,			// 敵の出現エリア設置
		SAVE_SPAWN_AREA,		// 敵の出現エリア設置
	};

	// 入力イベント
	enum class InputEvent
	{
		CONFIRM,				// 決定
		CANCEL,					// 取り消し
		PAUSE,					// ポーズ
		EDIT,					// エディット
		INVENTRY,				// インベントリ
		RENDMODE,				// 描画
		MOVEUP,					// 前進
		MOVEDOWN,				// 交代
		MOVELEFT,				// 右移動
		MOVERIGHT,				// 左移動
		JUMP,					// ジャンプ
		ATTACK,					// 攻撃
		OPEN,					// 開く
		BUTTONINVOKE,			// ボタン実行
		CAMERADASH,				// カメラの速度アップ（エディット中）
		CAMERAVVERTICAL,		// カメラ上下
		RELOAD,					// reload
	};
};


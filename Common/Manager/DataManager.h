//====================================================//
// ファイル名   : DataManager.h
// 作成者       : 高安瑞貴
//
// 概要 : このゲーム全体で扱うデータを管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

//====================================================//
// 宣言
//====================================================//

// すべてシーン間で扱うデータを管理するクラス
class DataManager final
{
private:
	// コンストラクタ
	DataManager() = default;
	// デストラクタ
	~DataManager() = default;

public:
	// DataManagerクラスのインスタンスを取得する
	static DataManager* GetInstance()
	{
		// インスタンスが生成されていない場合
		if (s_dataManager == nullptr)
		{
			// DataManagerクラスのインスタンスを生成し、インスタンスをm_DataManagerに格納する
			s_dataManager = new DataManager();
		}
		// DataManagerクラスのインスタンスを返す
		return s_dataManager;
	}

	// DataManagerクラスのインスタンスへの参照を取得する
	static DataManager& GetRefInstance()
	{
		// インスタンスが生成されていない場合
		if (s_dataManager == nullptr)
		{
			// DataManagerクラスのインスタンスを生成し、インスタンスをs_dataManagerに格納する
			s_dataManager = new DataManager();
		}
		// DataManagerクラスのインスタンスを返す
		return *s_dataManager;
	}

	// DataManagerクラスのインスタンスを破棄する
	static void Dispose()
	{
		// インスタンスが生成されている場合
		if (s_dataManager != nullptr)
		{
			// DataManagerクラスのインスタンスを破棄する
			delete s_dataManager;
			// DataManagerクラスのインスタンス変数を初期化する
			s_dataManager = nullptr;
		}
	}

	// ---- フォントサイズ ---- //
	const float Font_Size = 24.0f;

	// ---- 画像サイズ ---- //
	const float Image_Size = 1.0f;

	// ---- スクリーン ---- //
	const float ScreenX = 1280.0f;
	const float ScreenY = 720.0f;
	const float CenterX = ScreenX / 2.0f;
	const float CenterY = ScreenY / 2.0f;
	const SimpleMath::Vector2 Center = { CenterX, CenterY };
	

	// ---- データ ---- //
	// 現在の記録
	int score = 0;
	float time = 0;
	bool isClear = false;
	// 最高記録
	int bestScore = 0;
	float bestTime = 0;

	// ---- 関数 ---- //

	// ハイスコアの確認
	bool CheckAndSaveHighscore()
	{
		if (score >= bestScore)
		{
			bestScore = score;
			return true;
		}
		return false;
	}
	// ベストタイムの確認
	bool CheckAndSaveBesttime()
	{
		if (time <= bestTime || bestTime <= 0.0f)
		{
			bestTime = time;
			return true;
		}
		return false;
	}

	// 数値のリセット
	void Reset()
	{
		score = 0;
		time = 0;
	}


private:
	// DataManagerクラスのインスタンスへのポインタ
	static DataManager* s_dataManager;

	
};


//====================================================//
// ファイル名   : StorageManager.h
// 作成者       : 高安瑞貴
//
// 概要 : ファイルの読み込みを管理するシングルトンクラス
//====================================================//

#pragma once
#include <string>
#include <fstream>

//====================================================//
// インクルードファイル
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class StorageManager
{

public:

    // ファイルの中身を渡す関数（出力）
    static std::string LoadTextFile(std::string_view filename)
    {
        std::ifstream ifs (filename.data());
        try
        {
            std::stringstream buffer;
            buffer << ifs.rdbuf();
			ifs.close();
            return buffer.str();
        }
        catch (const std::exception& e)
        {
        }

		ifs.close();

        return std::string();
    }

	// ファイルの中身を渡す関数（入力）
	static std::ofstream LoadInputTextFile(std::string_view filename)
	{
		std::ofstream ofs(filename.data());

		if (!ofs.is_open())
		{
			std::cerr << "Failed to open " << filename << '\n';
		}

		return ofs;
	}

public:

    // storageManagerクラスのインスタンスを取得する
	static StorageManager* GetInstance()
	{
		// インスタンスが生成されていない場合
		if (s_storageManager == nullptr)
		{
			// storageManagerクラスのインスタンスを生成し、インスタンスをm_storageManagerに格納する
			s_storageManager = new StorageManager();
		}
		// storageManagerクラスのインスタンスを返す
		return s_storageManager;
	}

	// storageManagerクラスのインスタンスへの参照を取得する
	static StorageManager& GetRefInstance()
	{
		// インスタンスが生成されていない場合
		if (s_storageManager == nullptr)
		{
			// storageManagerクラスのインスタンスを生成し、インスタンスをs_storageManagerに格納する
			s_storageManager = new StorageManager();
		}
		// storageManagerクラスのインスタンスを返す
		return *s_storageManager;
	}

	// storageManagerクラスのインスタンスを破棄する
	static void Dispose()
	{
		// インスタンスが生成されている場合
		if (s_storageManager != nullptr)
		{
			// storageManagerクラスのインスタンスを破棄する
			delete s_storageManager;
			// storageManagerクラスのインスタンス変数を初期化する
			s_storageManager = nullptr;
		}
	}

private:
    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
	StorageManager() {};
	~StorageManager() {};

    static StorageManager* s_storageManager;
};
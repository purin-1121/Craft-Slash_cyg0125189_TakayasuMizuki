
#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Message/EventBus.h"
#include "DataPath.h"


template<typename T>
class JsonDataManager
{
public:

	using DataList = std::vector<std::unique_ptr<T>>;
	using DataMap = std::unordered_map<std::string, DataList>;

    // オブジェクトデータの取得
    T* GetData(std::string_view key)
    {
        auto it = m_datas.find(std::string(key));

        if (it == m_datas.end() || it->second.empty())
            return nullptr;

        return it->second.front().get();
    }

	// オブジェクトのデータ取得
    std::vector<T> GetDataList(std::string_view key) const
    {
        std::vector<T> result;

        auto it = m_datas.find(std::string(key));

        if (it == m_datas.end())
            return result;

        result.reserve(it->second.size());

        for (const auto& data : it->second)
        {
            if (data)
                result.push_back(*data);
        }

        return result;
    }
    // 全データの取得
    DataMap& GetDatas() { return m_datas; }

    
	~JsonDataManager() {};

	static JsonDataManager& GetInstance()
	{
		static JsonDataManager instance(
			JsonDataPath<T>::value
		);

		return instance;
	}

private:

	JsonDataManager(const std::string& path)
		:m_datas{}
		, m_path{path}
	{
		RoadData(path);
		EventBus::GetInstance()->Subscribe(Message::EventID::RELOAD, [&](void*) {
            RoadData(m_path); 
            });
	}

    DataMap m_datas;
	std::string m_path;

    void RoadData(const std::string& path)
    {
        // JSONファイルを読み込む
        const std::string str = StorageManager::LoadTextFile(path);

        try
        {
            const nlohmann::json j = nlohmann::json::parse(str);

            // ルートが配列でなければ終了
            if (!j.is_array())
                return;

            // JSON配列の各要素を処理
            for (const auto& item : j)
            {
                // オブジェクト以外とコメント用データは無視
                if (!item.is_object() || item.contains("Comment"))
                    continue;

                // =====================================================
                // ① nameを持つ単体データを処理
                // =====================================================
                if (item.contains("name") && item["name"].is_string())
                {
                    const std::string name =
                        item["name"].get<std::string>();

                    // JSON全体からデータ構造体へ変換
                    T data = item.get<T>();

                    // 名前に対応するデータリスト
                    auto& list = m_datas[name];

                    if (list.empty())
                    {
                        // 未登録なら追加
                        list.push_back(
                            std::make_unique<T>(std::move(data))
                        );
                    }
                    else
                    {
                        // 登録済みなら既存オブジェクトを更新
                        // unique_ptr自体を交換しないため、
                        // 既存オブジェクトへのポインタを維持できる
                        *list.front() = std::move(data);
                    }

                    // 単体データなのでグループ処理には進まない
                    continue;
                }

                // =====================================================
                // ② BossAなどのグループ形式を処理
                // =====================================================
                for (const auto& [groupName, groupData] : item.items())
                {
                    // 配列ではない項目は対象外
                    if (!groupData.is_array())
                        continue;

                    // グループ名に対応するデータリスト
                    auto& list = m_datas[groupName];

                    std::size_t index = 0;

                    for (const auto& element : groupData)
                    {
                        // 配列の要素がオブジェクトでなければ無視
                        if (!element.is_object() ||
                            element.contains("Comment"))
                        {
                            continue;
                        }

                        // JSONからデータ構造体へ変換
                        T data = element.get<T>();

                        if (index < list.size())
                        {
                            // 既存データを更新
                            *list[index] = std::move(data);
                        }
                        else
                        {
                            // データが足りなければ追加
                            list.push_back(
                                std::make_unique<T>(std::move(data))
                            );
                        }

                        ++index;
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "JSONデータの読み込みに失敗: "
                << path << '\n'
                << e.what() << std::endl;
        }
    }
};




#pragma once
#include "Lib/Random.h"
#include "EnemyStateFactory.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/GameObject/BreakableObject.h"
#include "Game/GamePlay/Blocks/Goal.h"
#include "Game/GamePlay/Factory/DropFactory.h"
#include "Game/GamePlay/Blocks/Chest.h"
#include "Common/Manager/InputManager.h"
#include "Common/Manager/Resources/JsonDataManager.h"
#include "Common/Manager/Resources/ObjectDataType.h"

#include <type_traits>

using namespace DirectX;

// ゲームオブジェクトのファクトリークラス
class ObjectFactory
{
public:

	ObjectFactory()
		: m_gridFilled{}
		, m_grid{}
		, m_mathSize{}
	{
        
    };

	// 空いているランダムな座標を取得する内部関数
    std::pair<int, int> GetEmptyGrid() {
        while (true) {
            int x = Random::Range(0, m_grid - 1);
            int z = Random::Range(0, m_grid - 1);
            
            // まだ使われていなければ確定
            if (m_gridFilled.find({x, z}) == m_gridFilled.end()) {
                m_gridFilled.insert({x, z});
                return {x, z};
            }
            // 埋まっていたらループして再抽選（空きが十分ある前提）
        }
    }

    template<class T, class... Args>
    std::unique_ptr<T> CreateForJson(
        const std::string& name,
        Args&&... args)
    {
        // オブジェクトに対応するデータ型を取得
        using DataType = typename ObjectDataType<T>::Type;

        // JSONデータを取得
        DataType* data =
            JsonDataManager<DataType>::GetInstance().GetData(name);

        // データが見つからなければ生成しない
        if (data == nullptr)
            return nullptr;

        // コンストラクタがポインタを受け取る場合
        if constexpr (
            std::is_constructible_v<T, Args&&..., DataType*>)
        {
            return std::make_unique<T>(
                std::forward<Args>(args)...,
                data
            );
        }
        // コンストラクタが参照または値を受け取る場合
        else if constexpr (
            std::is_constructible_v<T, Args&&..., const DataType&>)
        {
            return std::make_unique<T>(
                std::forward<Args>(args)...,
                *data
            );
        }
        else
        {
            static_assert(
                std::is_constructible_v<T, Args&&..., const DataType&>,
                "Create: コンストラクタの引数型とデータ型が一致しません。名前:"
                );
        }
    }

    template<class T, class... Args>
    std::unique_ptr<T> Create(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

private:

	// グリッドの数
	int m_grid;
	// すでにオブジェクトがいるかどうか
	std::set<std::pair<int, int>> m_gridFilled;
	// 一マスの大きさ
	float m_mathSize;

};


#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Item.h"
#include "InventryData.h"

class Canvas;
class Camera;

// アイテム生成情報を補完
struct ItemData
{
	SimpleMath::Vector3 pos;
	uint8_t type = ItemFlag::WOOD;
	int index = -1;
};

// アイテムの基本情報
struct ItemSetting {
	uint8_t type = ItemFlag::WOOD;
	std::string name = "wood"; // アイテムの名前
	std::string modelName = "Meat";	// モデル参照用の名前
};

class ItemManager
{

public:

	ItemManager(const int& itemCount);
	~ItemManager() = default;

	void Initialize(Canvas&, const GameContext& gameContext);
	void Update(float elapsedTime, SimpleMath::Vector3 pos, const GameContext& gameContext, Camera& camera)  ;
	void Render(const GameContext& gameContext, SimpleMath::Matrix view)  ;
	void RenderText()  ;
	void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)  ;
	void Finalize()  ;

	// ------ Get ------ //
	std::vector<std::unique_ptr<Item>>& GetItems() { return m_items; }
	uint8_t GetItemType(int index) { return m_items[index]->GetType(); }
	int MaxItems() { return m_items.size(); }

	// ------ Set ------ //
	void ClearItem(bool flag, int index) { m_items[index]->SetActive(flag); }
	
	// ------ Add ------ //

	// ------ Other ------ //
	// void CreateItem(Item item) { item = item; }
	// アイテム初期化
	void InitItem(SimpleMath::Vector3 pos, uint8_t type, SimpleMath::Vector3 vec = SimpleMath::Vector3::Zero, int index = -1);
	void PostUpdate();


	// ---- メンバ変数 ---- //
private:

	// ---- 定数 ---- //
	// 最大速度
	static constexpr float MAX_SPEED = 15.0f;
	// 最大操舵力
	static constexpr float MAX_FORCE = 0.4f;

	// ---- 基本のパラメータ ---- //
	// アイテム
	std::vector<std::unique_ptr<Item>> m_items;	// アイテムの数
	// 追加リクエストのあったアイテムの情報
	std::vector<ItemData> m_requestItem;
	// アイテムの情報プール
	std::unordered_map<std::string, ItemSetting> m_itemPool;
	// アイテムの数
	int m_itemCount;


	// ---- デバッグ用 ---- //
	// 今activeなアイテムの数
	int m_num;

	// ---- メンバ関数 ---- //
private:

	void LoadItemData();

};


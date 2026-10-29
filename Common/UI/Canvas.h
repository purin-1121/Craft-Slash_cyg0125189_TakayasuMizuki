//====================================================//
// ファイル名   : Canavs.h
// 作成者       : 高安瑞貴
//
// 概要 : UIを管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include "Common/UI/Common/UIObject.h"
#include "Message/InputGroup.h"

//====================================================//
// 宣言
//====================================================//

using namespace DirectX;

// UIの情報
struct UI
{
	std::string name;
	UIObject* object;
};

class Canvas : public InputGroup
{


public:

	// コンストラクタ
	Canvas(bool active = true, InputGroup* group = nullptr);

	// ---- 基本関数 ---- //
	void Initialize(const GameContext& gameContext);
	bool Update(const GameContext& gameContext,const float& elapsedTime);
	void Render(const GameContext& gameContext, const Camera& camera);
	void RenderText();
	void Finalize();

	// マウスがUIに触れているか
	bool IsOnUIs();
	// UIの追加
	UIObject* CreateUI(const std::string& name, UIObject* UI) { m_UI.push_back({ name, UI }); SortLayer<int>(); return UI; };
	// UIの呼び出し
	UIObject* SammonUI(const std::string& name);
	template<typename T>
	T* SammonUI(const std::string& name);	// 事前にキャストしてから送る
	// UIの表示順並び替え
	template<typename T>
	void SortLayer(std::function<bool(T, T)> f = [&](T a, T b) { return a > b;});
	// 関数の追加
	void AddFunc(const std::string& key, std::function<void()> f) { m_func[key] = f; };
	// 関数の削除
	void RemoveFunc(const std::string& key) { m_func.erase(key); };
	// 位置の変更
	void SetPosition(SimpleMath::Vector3 pos) { m_position = pos; }
	void AddPosition(SimpleMath::Vector3 pos) { m_position += pos; }
	// 位置取得
	SimpleMath::Vector3& GetPosition() { return m_position; }
	// active状態の設定
	void SetActive(bool active) { m_isActive = active; }
	// active状態の取得
	// フレーム毎秒の取得
	// フレーム毎秒の設定

private:

	SimpleMath::Vector3 m_position;
	SimpleMath::Vector3 m_scale;
	bool m_isActive;
	std::vector<UI> m_UI;	// 所持しているUI
	std::unordered_map<std::string, std::function<void()>> m_func;	// キャンバスの関数
	std::function<void()>* m_threadFunc;	// 実行し続ける関数

};

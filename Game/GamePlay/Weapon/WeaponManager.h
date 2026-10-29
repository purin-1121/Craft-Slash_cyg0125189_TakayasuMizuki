#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "InterFace/IWeapon.h"
#include "Message/EventBus.h"

class Actor;
class WeaponManager
{

public:

	WeaponManager(Actor* owner);
	~WeaponManager();

	void Initialize(const GameContext& gameContext);
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera);
	void Render(const GameContext& gameContext, SimpleMath::Matrix view);
	void RenderText();
	void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view);
	void Finalize();

	// ------ Get ------ //
	IWeapon* GetCurrentWeapon() { return m_currentWeapon; }

	// ------ Set ------ //

	// ------ Add ------ //

	// ------ Other ------ //
	// void CreateWeapon(Weapon Weapon) { Weapon = Weapon; }
	void Request(std::string name) { 
		m_requestName = name; 
	};
	// アイテム初期化
	void PostUpdate();
	void ChangeWeapon(std::string name, const GameContext& gameContext);


	// ---- メンバ変数 ---- //
private:

	// ---- 定数 ---- //
	// 最大速度
	static constexpr float MAX_SPEED = 15.0f;
	// 最大操舵力
	static constexpr float MAX_FORCE = 0.4f;

	// ---- 基本のパラメータ ---- //

	// 使ってる武器の情報
	IWeapon* m_currentWeapon;
	// この武器マネージャを管理する親オブジェクト
	Actor* m_owner;
	// リクエストされた武器の名前
	std::string m_requestName;
	// 武器進化イベントID
	SubsprictionID m_evolveID;
	// 一度作成されたことがある武器か
	std::unordered_map<std::string, bool> m_isCreated;

	// ---- デバッグ用 ---- //

	// ---- メンバ関数 ---- //
	WeaponData* FindWEaponData(std::string name);

};


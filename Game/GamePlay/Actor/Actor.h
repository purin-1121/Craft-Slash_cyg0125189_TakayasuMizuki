//====================================================//
// ファイル名   : Actor.h
// 作成者       : 高安瑞貴
//
// 概要 : 自立志向を持った動的オブジェクトが継承するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Game/GamePlay/GameObject/GameObject.h"
#include "Game/GamePlay/Blocks/Drop.h"
#include "Game/GamePlay/State/StateMichine.h"
#include "Common/Model/Animator.h"
#include "Common/Model/CpuSkinnedRenderer.h"
#include "Game/GamePlay/Weapon/WeaponManager.h"
#include "Game/GamePlay/Actor/BuffManager.h"


//====================================================//
// 宣言
//====================================================//

class Actor : public GameObject
{

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="hp">初期体力</param>
	/// <param name="pos">初期位置</param>
	/// <param name="vec">速度</param>
	/// <param name="collider">衝突判定</param>
	Actor(
		int hp,
		 SimpleMath::Vector3 pos, 
		 SimpleMath::Vector3 vec,
		float mass
	);

	// デストラクタ
	virtual ~Actor() override;

	// ---- 基本関数 ---- //
	virtual void Initialize(const GameContext& gameContext) override = 0;
	virtual void Update(float elapsedTime, const GameContext& gameContext, Camera& camera)override;
	virtual void Render(const GameContext& gameContext,  SimpleMath::Matrix view) override = 0;
	virtual void RenderText() override = 0;
	virtual void RenderBounding(const GameContext& gameContext,  SimpleMath::Matrix view) override;
	virtual void Finalize()  override = 0;

	// ------ Get ------ //
	virtual bool IsAttacking() { return false; }
	virtual bool IsAlive() { return m_hp > 0; };
	virtual bool IsActive() override { return IsAlive(); };
	virtual int& GetHP() { return m_hp; };	// 現在の体力のの凝り具合を取得
	virtual  SimpleMath::Vector3& GetForword() { return m_forword; }
	virtual bool IsInvisible() { return m_invisibleTime > 0; }
	void InvisibleOn(float time) { m_invisibleTime = time; }
	StateMachine& GetStateMachine() { return m_stateMachine; }
	virtual IGameObjectState* GetIdleState() = 0;
	template<typename TState>
	DirectX::Model* GetModel() { return m_modelMap[typeid(TState)]; }
	StateID GetAfterID() { return m_idAfterStun; }

	// ------ Set ------ //
	void SetDamageCallback(std::function<void()> func) { m_damageCallback = func; }
	Actor* SetDieCallback(std::function<void(GameObject*, const Drop*, int, int)> func) { m_dieCallback = func; return this; }
	void SetRendVector(SimpleMath::Vector3 r) { m_rendVec = r; SetQuaternion_Y(std::atan2(m_rendVec.x, m_rendVec.z)); }
	void AddRendVector(SimpleMath::Vector3 r) { m_rendVec += r; SetQuaternion_Y(std::atan2(m_rendVec.x, m_rendVec.z));}
	void SetStop(bool stop) { m_isStop = stop; }	// 生死の設定
	void SetAfterID(StateID id) { m_idAfterStun = id; }	// スタン後移行するステート

	// ---- アニメーション関連 ---- //
	Animator& GetAnimator() { return m_animator; }
	// アニメーションの再生
	void PlayAnimation(std::string_view name, bool isLoop = true) {
		const auto& idleName = GetIdleAnimationName();
        const std::string suffix = "Idle";
        const std::string clipName = name == idleName
            ? idleName
            : idleName.substr(0, idleName.size() - suffix.size()) + std::string(name);
        const auto resource = ObjectLib::m_animatedModels.find(clipName);
        if (resource == ObjectLib::m_animatedModels.end() || !resource->second)
        {
            OutputDebugStringA(("Animation not found: " + clipName + "\n").c_str());
            return;
        }
        m_animator.SetModel(resource->second);
		m_animator.Play(0, isLoop);
	}
	virtual const std::string& GetIdleAnimationName() const = 0;

	// ---- 武器関連 ---- //
	IWeapon& GetWeapon() { return *m_currentWeapon; }
	WeaponManager& GetWeaponManager() { return *m_weapons; }
	int GetPower() { return GetWeapon().GetDamage(); }	// 今の武器のダメージを取得
	const  std::string& GetBoneName() { return m_boneNameForWeapon; }		// 現在武器が追従しているボーン名
	void SetBoneName(std::string str) { m_boneNameForWeapon = str; }
	void SetCurrentWeapon(std::unique_ptr<IWeapon> weapon) { m_currentWeapon = std::move(weapon); }

protected:

	// ---- 基本のパラメータ ---- //
	// HP
	int m_hp;
	// ダメージコールバック関数
	std::function<void()> m_damageCallback;
	// 死亡コールバック関数
	std::function<void(GameObject*, const Drop*, int, int)> m_dieCallback;
	// どこ向いてるか
	SimpleMath::Vector3 m_forword;
	Direction m_d;
	// 無敵時間
	float m_invisibleTime;
	// 描画角度用速度
	SimpleMath::Vector3 m_rendVec;
	// 処理をしないか
	bool m_isStop;
	// バフ管理
	BuffManager m_buffManager;


	// 武器マネージャ
	std::unique_ptr<WeaponManager> m_weapons;
	// 現在使用している武器
	std::unique_ptr<IWeapon> m_currentWeapon;

	// 描画モデルマップ
	std::unordered_map<std::type_index, DirectX::Model*> m_modelMap;

	// ---- ステート管理 ---- //
	StateMachine m_stateMachine;
	// スタン後に移動するID
	StateID m_idAfterStun;


	// ---- アニメーション ---- //
	Animator m_animator;					// アニメーションの再生
	CpuSkinnedRenderer m_skinnedRenderer;	// 描画
	std::string m_boneNameForWeapon;		// 武器を追従させるボーン

	// ---- 大きさ ---- //
	static constexpr SimpleMath::Vector3 HUMAN_SIZE = { 0.4f, 1.0f, 0.4f };
	// 持ち手の位置
	static constexpr SimpleMath::Vector3 HOLD_POSITION = SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	// 背負う位置
	static constexpr SimpleMath::Vector3 BACK_POSITION = { HUMAN_SIZE.x / 2 , HUMAN_SIZE.y / 2,  HUMAN_SIZE.z / 2 };
	// 持ち手の位置行列
	static inline SimpleMath::Matrix HOLD_MATRIX = SimpleMath::Matrix::CreateTranslation(HOLD_POSITION);
	// 背負う位置行列
	static inline SimpleMath::Matrix BACK_MATRIX = SimpleMath::Matrix::CreateTranslation(BACK_POSITION);


	// 地面判定を取る足
	std::vector<Collider*> m_legs;

	std::unordered_map<GameObject*, int> m_hitCount;

	// 「このダメージ判定は今回処理していい1回目か」を判定する共通関数
	bool ShouldProcessHit(GameObject* otherObj)
	{
		int& count = m_hitCount[otherObj];
		count++;
		return count == 1; // 1回目だけtrue
	}

	void ClearHit(GameObject* otherObj)
	{
		auto it = m_hitCount.find(otherObj);
		if (it != m_hitCount.end())
		{
			if (--(it->second) <= 0)
				m_hitCount.erase(it);
		}
	}
};


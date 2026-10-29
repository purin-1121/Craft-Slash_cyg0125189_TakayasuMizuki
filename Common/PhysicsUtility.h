//====================================================//
// ファイル名   : PhysicsUtility.h
// 作成者       : 高安瑞貴
//
// 概要 : 物理挙動を管理する名前空間
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

//====================================================//
// 宣言
//====================================================//
using namespace DirectX;
class GameObject;

class PhysicsUtility
{

public:

	/// <summary>
	/// オブジェクトの移動などを処理する
	/// </summary>
	/// <param name="objects">対象のオブジェクト</param>
	void PhysicsUpdate();

	/// <summary>
	/// 各オブジェクトごとに重力を設定する
	/// </summary>
	/// <param name="object">　対象オブジェクト　</param>
	void ApplyGravity(GameObject* object, float elapsedTime);

	/// <summary>
	/// 発射方向のベクトルを取得
	/// </summary>
	/// <param name="pitch">ピッチ</param>
	/// <param name="yaw">ヨー</param>
	/// <param name="init_forward">初期前方ベクトル</param>
	/// <returns>発射方向ベクトル</returns>
	XMVECTOR GetVec(float pitch, float yaw, XMVECTOR init_forward);

	/// <summary>
	/// 任意の原点における回転させた座標
	/// </summary>
	/// <param name="pitch">ピッチ</param>
	/// <param name="yaw">ヨー</param>
	/// <param name="init_forward">初期前方ベクトル</param>
	/// <param name="origin">原点</param>
	/// <param name="d_to_o">オフセット</param>
	/// <returns>回転させた座標</returns>
	XMVECTOR GetPos(float pitch, float yaw, XMVECTOR init_forward, SimpleMath::Vector3 origin, SimpleMath::Vector3 d_to_o);

	// オブジェクトの登録
	void RegisterObject(GameObject* object) { m_objects.push_back(object); };
	// オブジェクトの解除
	void UnRegisterObject(GameObject* object);
	// オブジェクトの登録
	std::vector<GameObject*>& GetObjects() { return m_objects; }

	// ------ 基本要素 ------ //
	static constexpr float GRAVITY = -9.8f * 0.2f;							// 重力加速度
	static constexpr SimpleMath::Vector3 TERMINAL_VELOCITY = 
						{ GRAVITY * 3.5f, GRAVITY * 2, GRAVITY * 3.5f };	// 重力加速度の終着速度
	static constexpr float FRICTION = 0.95f;						// 摩擦力

	// ------ ゲーム用物理 ------ //
	static constexpr const float JUMPCUT_GRAVITY_SCALE = 2.5f;		// ジャンプ後ボタンを離した際の重力
	static constexpr const float FALL_GRAVITY_SCALE = 1.5f;		// 落下中の重力速度
	static constexpr const float FIXED_TIME = 1 / 60.0f;				// 物理計算用時間

	~PhysicsUtility() = default;

	static PhysicsUtility* GetInstance()
	{
		if (s_physicsUtility == nullptr)
		{
			s_physicsUtility.reset(new PhysicsUtility);
		}

		return s_physicsUtility.get();
	}

	static PhysicsUtility& GetRefarence()
	{
		if (s_physicsUtility == nullptr)
		{
			s_physicsUtility.reset(new PhysicsUtility);
		}

		return *s_physicsUtility.get();
	}

	static void Dispose()
	{
		s_physicsUtility.reset();
	}


private:

	std::vector<GameObject*> m_objects;	// 実行するオブジェクト

	static std::unique_ptr<PhysicsUtility> s_physicsUtility;

	PhysicsUtility() :m_objects{} {};
	
	
};
//--------------------------------------------------------------------------------------
// File: GameObject.h
//
// オブジェクトにつける基底クラス
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include <vector>
#include <utility>
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/GameDataTypes.h"
class Camera;

class GameObject
{

public:

	// ------ コンストラクタ ------ //
	GameObject(
		SimpleMath::Vector3 pos,
		SimpleMath::Vector3 vec, 
		float mass = 100.0f,
		bool kinematic = true
	);

	virtual ~GameObject();

	// ------ 基本関数 ------ //
	virtual void Initialize(const GameContext& gameContext) = 0;
	virtual void Update(float elapsedTime, const GameContext& gameContext, Camera& camera)  = 0;
	virtual void Render(const GameContext& gameContext, SimpleMath::Matrix view)  = 0;
	virtual void RenderText()  = 0;
	virtual void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view);
	virtual void Finalize()  = 0;

	// ------ Get ------ //
	inline GameObject* GetParent() { return m_parent; };
	inline std::vector<GameObject*> GetChildren() { return m_children; }
	inline GameObject* GetChild(int index) { return m_children[index]; }
	inline GroundState GetGround() { return m_groundState; };
	inline SimpleMath::Vector3& GetPosition() { return m_position; }
	inline SimpleMath::Vector3& GetScale() { return m_scale; }
	inline SimpleMath::Vector3 GetCenter() { return m_position; }
	inline SimpleMath::Vector3 GetPosOffset() { return m_posOffset; }
	inline SimpleMath::Quaternion& GetQuaternion() { return m_quaternion; }
	inline SimpleMath::Matrix GetRotateMatrix() { return m_rotateMatrix; }
	inline SimpleMath::Vector3 GetVelocity() { return m_velocity; }
	inline SimpleMath::Matrix GetWorldMatrix() { return m_worldMatrix; }
	inline std::vector<Collider*>& GetBounding(){ return m_colliders; }
	inline SimpleMath::Vector3 GetColliderOffset() { return m_colliderOffset; }
	inline bool IsKinematic() { return m_isKinematic; };
	inline float GetMass() { return m_mass; }
	inline float GetElapsedTime() { return m_elapsedTime; }
	inline float GetHeight() { return m_height; }
	inline SimpleMath::Vector3 GetForward()
	{
		// Facing yaw uses atan2(x, z), so the local front is +Z.
		SimpleMath::Vector3 dir = SimpleMath::Vector3::Transform(
			SimpleMath::Vector3::UnitZ, 
			m_quaternion
		);
		dir.Normalize();
		return dir;
	}
	virtual bool IsActive() = 0;

	// ------ Set ------ //
	inline void SetParent(GameObject* parent) { m_parent = parent; };
	inline void AddChild(GameObject* child) { child->SetParent(this); m_children.push_back(child); }
	inline void Flying(GroundState state) { m_groundState = state; };
	inline void SetPosition(SimpleMath::Vector3 pos) { m_position = pos; }
	inline void SetScale(SimpleMath::Vector3 scale) {  m_scale = scale;}
	inline void SetPosOffset(SimpleMath::Vector3 offset) { m_posOffset = offset; }
	inline void SetQuaternion(SimpleMath::Quaternion q) { m_quaternion = q; }
	inline void SetQuaternion_Y(float qy) { m_quaternion = SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, qy); }
	inline void SetRotateMatrix(SimpleMath::Matrix rotate) { m_rotateMatrix = rotate; }
	inline void SetVelocity(SimpleMath::Vector3 vec) { m_velocity = vec; }
	inline void SetVelocity_X(float x) { m_velocity.x = x; }
	inline void SetVelocity_Y(float y) { m_velocity.y = y; }
	inline void SetVelocity_Z(float z) { m_velocity.z = z; }
	inline void SetWorldMatrix(SimpleMath::Matrix world) { m_worldMatrix = world; }
	inline void SetColliderOffset(SimpleMath::Vector3 offset) { m_colliderOffset = offset; }
	inline void SetElapsedTime(float elapsed) { m_elapsedTime = elapsed; }
	inline void SetHeight(float height) { m_height = height; }

	// ------ Add ------ //
	inline void AddPosition(SimpleMath::Vector3 pos) { m_position += pos; }
	inline void AddVelocity(SimpleMath::Vector3 vec) { m_velocity += vec; }

	virtual void UpdateCollider()
	{
		// 衝突判定の更新
		for (auto& b : GetBounding())
		{
			b->SetPos(GetPosition() + b->GetPosOffset());
			b->SetRotatetion(GetQuaternion().ToEuler());
		}
	}
	
	// コライダーの追加
	template<class T, class... Args>
	inline T* AddCollider(Args&&... args)
	{
		T* collider = new T(std::forward<Args>(args)...);

		collider->SetObj(this);

		m_colliders.push_back(collider);

		CollisionManager::GetInstance()->AddCollider(collider);

		return collider;
	}

	// ------ 処理 ------ //
	// 攻撃判定と接触したときの処理
	virtual void OnCollisionEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  {};
	virtual void OnCollision(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  {};
	virtual void OnCollisionExit(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  {};
	virtual void OnTriggerEnter(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  {};
	virtual void OnTrigger(Collider* mine, Collider* other, GameObject* otherObj, const CollisionInfo& info)  {};
	virtual void OnTriggerExit(Collider* mine, Collider* other, GameObject* otherObj)   {};

private:

	// 親オブジェクト
	GameObject* m_parent;
	// 子オブジェクト
	std::vector<GameObject*> m_children;

	// 地面にいるか
	GroundState m_groundState;

	// 速度、角度
	SimpleMath::Vector3 m_velocity;
	SimpleMath::Vector3 m_oldPos;
	SimpleMath::Quaternion m_quaternion;
	// スケール
	SimpleMath::Vector3 m_scale;
	// 親の位置オフセット
	SimpleMath::Vector3 m_posOffset;
	// 回転行列
	SimpleMath::Matrix m_rotateMatrix;
	// このオブジェクトのワールド行列
	SimpleMath::Matrix m_worldMatrix;
	// オブジェクトの位置
	SimpleMath::Vector3 m_position;
	// 物理演算を行うかどうか
	bool m_isKinematic;
	// 自分が今いる座標の高さ
	float m_height;

	// 質量
	float m_mass;
	// フレーム毎秒
	float m_elapsedTime;

	// 当たり判定
	std::vector<Collider*> m_colliders;
	SimpleMath::Vector3 m_colliderOffset;

	// あいう
	BoxCollider* body;
};


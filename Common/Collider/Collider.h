//====================================================//
// ファイル名   : Collider.h
// 作成者       : 高安瑞貴
//
// 概要 : 衝突判定の定義クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Common/GameDataTypes.h"
#include "Tools/GameContext.h"
#include "Tools/JsonConverter.h"
#include "Common/Draw/Draw.h"
#include "Lib/MathLib.h"
#include "SimpleMath.h"

//====================================================//
// 宣言
//====================================================//

using namespace DirectX;
class OctreeNode;
//class GameObject;

enum class Type
{
	Line,
	Triangle,
	Plane,
	Box,
	OBB,
	Sphere,
	Capsule,
	Cylinder,
};

// 識別用コライダーID
enum class ColliderTag
{
	Default = 0,
	Ground,
	Item,
	Player,
	PlayerAttack,
	Enemy,
	EnemyAttack,
	EnemySearch,
	Leg,
};

// 識別用コライダーレイヤー
enum class ColliderLayer
{
	Default = 0,
	Player,
	Enemy,
	EnemySearch,
	Leg,
	Chest,
	ChestBody,
	PlayreAttack,
	Breakble,
	All,
};

struct CollisionInfo
{
	bool is;
	SimpleMath::Vector3 normal;
	//float penetration;
	SimpleMath::Vector3 push;
};

// 衝突判定の基底クラス
class Collider
{

public:

	Collider(const SimpleMath::Vector3& pos, const SimpleMath::Vector3& size, const SimpleMath::Vector3& vec, const Type& type, bool isTrigger)
		: position{pos}
		, size{size}
		, vec{vec}
		, m_type{type}
		, m_isTrigger{isTrigger}
		, m_offset{}
		, m_isDirty{true}
		, m_octreeIndex{100000000}
		, isStatic{}
	{};

	virtual ~Collider() {};

	// 初期化関数
	virtual void Clear() = 0;

	// 位置取得
	virtual SimpleMath::Vector3 GetPos() const { return position; }
	// 中心取得
	virtual  SimpleMath::Vector3 GetCenter() const { return position + m_offset + size / 2; } 
	// 大きさ取得
	virtual SimpleMath::Vector3 GetSize() const{ return size; }
	// 回転量取得
	virtual SimpleMath::Vector3 GetVector() const { return vec; }
	// 位置更新
	virtual void AddPos( SimpleMath::Vector3 pos) { 
		if (pos != SimpleMath::Vector3::Zero)
		{
			position += pos;
			m_isDirty = true;
		}
	}
	virtual void SetPos( SimpleMath::Vector3 pos) { 
		if (position != pos)
		{
			position = pos; 
			m_isDirty = true;
		}
	}
	// 大きさ更新
	virtual void SetSize( SimpleMath::Vector3 s) {
		if (size != s)
		{
			size = s;
			m_isDirty = true;
		}
	}
	// 回転量更新
	virtual void SetRotatetion(SimpleMath::Vector3 v) { 
		if (vec != v)
		{
			vec = v; 
			m_isDirty = true;	
		}
	}
	// 当たり判定の描画
	virtual void Draw(XMVECTORF32 color = Colors::Yellow);
	// 種類取得
	Type GetType() const { return m_type; };
	// タグ取得
	ColliderTag GetTag() const { return m_tag; }
	// タグ設定
	Collider* SetTag(ColliderTag tag) { m_tag = tag; return this; }
	// タグが一致しているか
	bool IsTag(ColliderTag tag) const { return m_tag == tag; }
	// コライダーの親オブジェクト取得
	GameObject* GetObj() { return m_owner; }
	// 親オブジェクト設定
	void SetObj(GameObject* obj) { m_owner = obj; }
	// 参照可能か決める
	void SetEnable(bool e) { m_enable = e; }
	// 参照可能かどうか取得
	bool GetEnable() const { 
		return m_enable; 
	}
	// オフセット設定
	void SetPosOffset(SimpleMath::Vector3 offset) { m_offset = offset; }
	// オフセット取得
	SimpleMath::Vector3 GetPosOffset() const { return m_offset; }
	// レイヤー設定
	Collider* SetLayer(ColliderLayer layer) { m_layer = layer; return this; }
	// レイヤー取得
	ColliderLayer GetLayer() const { return m_layer; }
	// 静的かどうか
	bool IsStatic() { return isStatic; }
	// 静的にする
	void SetStatic(bool is) { isStatic = is; }
	// ある点が子のコライダー内にあるかどうか
	virtual bool IsPointInside(const SimpleMath::Vector3& point) const { return false; }
	// トリガーかどうか
	bool IsTrigger() { return m_isTrigger; }
	// 変更されたかどうか
	bool IsDirty() { return m_isDirty; }
	void DirtyOff() { m_isDirty = false; }
	// ノードの設定
	void SetNode(uint16_t index) { m_octreeIndex = index; }
	// ノードの取得
	uint32_t& GetNode() { return m_octreeIndex; }

protected:


	// 位置と大きさ
	SimpleMath::Vector3 position;
	SimpleMath::Vector3 size;
	// 方向ベクトル
	SimpleMath::Vector3 vec;
	// 当たり判定のタイプ
	Type m_type;
	// コライダーの主
	GameObject* m_owner = nullptr;
	// 所属するノード番号
	uint32_t m_octreeIndex;
	// タグ
	ColliderTag m_tag = ColliderTag::Default;
	// 動的かどうか
	bool isStatic;

	// オブジェクトレイヤー
	//uint32_t m_layer;

	// 貫通するか
	bool m_isTrigger = false;

	// 実行できるか
	bool m_enable = true;

	// オフセット
	SimpleMath::Vector3 m_offset;

	// この判定のレイヤー
	ColliderLayer m_layer = ColliderLayer::Default;

	// 更新したかのフラグ
	bool m_isDirty;

	friend class CollisionManager;
	friend class GameObject;
};

// 線分の基本定義
class LineCollider : public Collider
{
	public:

		// ---- コンストラクタ ---- //
		LineCollider();
		LineCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size);
		// 情報をすべてリセット
		void Clear();

		// 座標取得
		SimpleMath::Vector3 GetStart() const { return GetPos(); }
		SimpleMath::Vector3 GetEnd() const { return GetPos() + GetSize(); }
		
		// 交点の取得
		SimpleMath::Vector3 GetCrossPoint() const { return crossPoint; }
		// 正規化したベクトルの取得
		SimpleMath::Vector3 GetNormailzed() const { return normalized; }
		// 交点の設定
		void SetCrossPoint(SimpleMath::Vector3 cross) { crossPoint = cross; }

		// ---- 定数 ---- //
		static const LineCollider Zero;
		static const LineCollider One;

	private:

		SimpleMath::Vector3 crossPoint;
		SimpleMath::Vector3 distance;
		SimpleMath::Vector3 normalized;

		friend class CollisionManager;

};

// 三角形の基本定義
class TriangleCollider : public Collider
{
	public:

    // コンストラクタ
	TriangleCollider(SimpleMath::Vector3 p0, SimpleMath::Vector3 p1, SimpleMath::Vector3 p2, bool isTrigger = true);

    // 距離判定などで「三角形の物理的な中心」が必要な時だけ、重心を計算して返す
	SimpleMath::Vector3 GetCenter() const override;

	// 法線取得
	SimpleMath::Vector3 GetCross() const { return normal; };

	// 各頂点の取得
	SimpleMath::Vector3 GetVertex0() const { return v0; }
	SimpleMath::Vector3 GetVertex1() const { return v1; }
	SimpleMath::Vector3 GetVertex2() const { return v2; }

    void Clear() override {}

private :

	SimpleMath::Vector3 v0, v1, v2;
    SimpleMath::Vector3 normal; // 面の傾き（法線)

	friend class CollisionManager;

};

// 平面の基本定義
class PlaneCollider : public Collider
	{
	
	public:

		// ---- コンストラクタ ---- //
		PlaneCollider();
		PlaneCollider(float f);
		PlaneCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, SimpleMath::Quaternion rotato, bool isTrigger = true);
		// 情報をすべてリセット
		void Clear();	



		// ---- 定数 ---- //
		static const PlaneCollider Zero;
		static const PlaneCollider One;

	private:

		SimpleMath::Vector3 vecRight;
		SimpleMath::Vector3 vecForward;

		friend class CollisionManager;

	};

// 矩形の基本定義
class BoxCollider : public Collider
	{


	public:

		// ---- コンストラクタ ---- //
		BoxCollider();
		BoxCollider(float f);
		/// <summary>
		/// 立方体コンストラクタ
		/// </summary>
		/// <param name="f">辺の長さ</param>
		/// <param name="center">中心座標かどうか</param>
		BoxCollider(float f, bool center);
		/// <summary>
		/// 直方体コンストラクタ
		/// </summary>
		/// <param name="pos">位置</param>
		/// <param name="size">大きさ</param>
		/// <param name="center">中心座標かどうか</param>
		/// <param name="isTrigger">すり抜けるか</param>
		BoxCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, bool center = true, bool isTrigger = false);
		/// <summary>
		/// 直方体コンストラクタ
		/// </summary>
		/// <param name="pos">初期座標</param>
		/// <param name="size">初期サイズ</param>
		/// <param name="vec">回転量</param>
		BoxCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, SimpleMath::Vector3 vec, bool isTrigger = false);

		virtual bool IsPointInside(const SimpleMath::Vector3& point) const override;

		// 各サイズを軸ごとに計算して一つの長さにする
		float* GetAxisFromSize(SimpleMath::Vector3 vec) {
			float f = (vec.x * GetSize().x / 2 +
				vec.y * GetSize().y / 2 +
				vec.z * GetSize().z / 2);
			return &f;
		}

		// 情報をすべてリセットに
		void Clear();

		// ---- 定数 ---- //
		static const BoxCollider Zero;
		static const BoxCollider One;

	private:
		
		friend class CollisionManager;

	};

class OBBCollider : public BoxCollider
{
public:

public:
	OBBCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, SimpleMath::Vector3 rotation, bool isTrigger = false)
		: BoxCollider(pos, size, rotation, isTrigger)
	{
		m_type = Type::OBB;
	}

	SimpleMath::Vector3 GetCenter() const override { return position; } ;
	SimpleMath::Vector3 GetRotation() const { return vec; } // 意味を明示するラッパー
	SimpleMath::Vector3 GetHalfExtents() const { return size * 0.5f; } // 半径も導出でOK

	SimpleMath::Vector3 GetAxisX() const { return SimpleMath::Vector3::UnitX; };
	SimpleMath::Vector3 GetAxisY() const { return SimpleMath::Vector3::UnitY; };
	SimpleMath::Vector3 GetAxisZ() const { return SimpleMath::Vector3::UnitZ; };

	SimpleMath::Matrix CreateRotMatrix() { return SimpleMath::Matrix::CreateRotationX(vec.x) * SimpleMath::Matrix::CreateRotationY(vec.y) * SimpleMath::Matrix::CreateRotationZ(vec.z); }

	bool IsPointInside(const SimpleMath::Vector3& point) const override { return false; };

	void Draw(XMVECTORF32 color = Colors::Yellow) override;

};

// 球の基本定義
class SphereCollider : public Collider
{
public :

	// ---- コンストラクタ ---- //
	SphereCollider();
	SphereCollider(float f);
	SphereCollider(float r, SimpleMath::Vector3 pos,  SimpleMath::Vector3 vec, bool isTrigger = false);

	// 情報をすべてゼロに
	void Clear();

	bool IsPointInside(const SimpleMath::Vector3& point) const override;

	// 位置取得
	SimpleMath::Vector3 GetPos() const override { return GetCenter(); };
	// 直径取得
	float GetDiametor() const { return r * 2; }
	// 半径取得
	float GetRadius() const { return r; }
	// 中心を取得
	SimpleMath::Vector3 GetCenter() const override { return position; }

	// 球の線分描画
	void Draw(XMVECTORF32 color = Colors::Yellow) override;

	// ---- 定数 ---- //
	static const SphereCollider Zero;
	static const SphereCollider One;

private:

	// 半径
	float r;

	friend class CollisionManager;
};

// カプセルコライダー
class CapsuleCollider : public Collider
{
public:  // ← ここが今の実装だと抜けている(後述)

	CapsuleCollider();
	CapsuleCollider(float f);
	CapsuleCollider(float r, float height, SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, bool isTrigger = false);

	void Clear() override;

	// ---- 位置・形状取得 ---- //
	SimpleMath::Vector3 GetCenter() const override { return position; }
	// 線分の始点(下側の半球の中心)
	SimpleMath::Vector3 GetSegmentStart() const { return position - axis * (halfHeight); }
	// 線分の終点(上側の半球の中心)
	SimpleMath::Vector3 GetSegmentEnd()   const { return position + axis * (halfHeight); }
	// 正規化済みの軸方向
	SimpleMath::Vector3 GetAxis() const { return axis; }

	bool IsPointInside(const SimpleMath::Vector3& point) const override;

	float GetRadius() const { return r; }
	float GetHeight() const { return height; }       // 円柱部分の長さ(半球を除いた長さ)
	float GetHalfHeight() const { return halfHeight; }

	void Draw(XMVECTORF32 color = Colors::Yellow) override;

	static const CapsuleCollider Zero;
	static const CapsuleCollider One;

private:

	float r;            // 半径
	float height;        // 円柱部分の長さ(全長ではなく、半球を除いた芯の長さ)
	float halfHeight;    // height / 2 をキャッシュ(毎回割り算しないため)
	SimpleMath::Vector3 axis; // 正規化済みの軸方向ベクトル(キャッシュ)

	friend class CollisionManager;
};

// 円柱コライダー
class CylinderCollider : public Collider
{
public:

	// ---- コンストラクタ ---- //
	CylinderCollider();
	CylinderCollider(float f);
	CylinderCollider(float r, float height, SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, bool isTrigger = false);

	// 情報をすべてリセット
	void Clear() override;

	// ---- 位置・形状取得 ---- //
	SimpleMath::Vector3 GetPos() const override { return position; }
	SimpleMath::Vector3 GetCenter() const override { return position; }

	// 下端の中心(平らな蓋の中心)
	SimpleMath::Vector3 GetBottom() const { return position - axis * halfHeight; }
	// 上端の中心
	SimpleMath::Vector3 GetTop() const { return position + axis * halfHeight; }
	// 正規化済みの軸方向
	SimpleMath::Vector3 GetAxis() const { return axis; }
	// シリンダー内にいるか
	bool IsPointInside(const SimpleMath::Vector3& point) const override;

	float GetRadius() const { return r; }
	float GetHeight() const { return height; }
	float GetHalfHeight() const { return halfHeight; }

	// 円柱の描画
	void Draw(XMVECTORF32 color = Colors::Yellow) override;

	// ---- 定数 ---- //
	static const CylinderCollider Zero;
	static const CylinderCollider One;

private:

	float r;              // 半径
	float height;          // 円柱の高さ(全長)
	float halfHeight;      // height / 2 のキャッシュ
	SimpleMath::Vector3 axis; // 正規化済みの軸方向

	friend class CollisionManager;
};

// 接触コライダーのペア
struct CollisionPair
{
	Collider* a;
	Collider* b;

	CollisionPair(Collider* first, Collider* second)
		: a{ first }
		, b{ second }
	{}

	bool operator==(const CollisionPair& other) const
	{
		return (a == other.a && b == other.b) ||
			(a == other.b && b == other.a);
	}
};

// コライダーのハッシュ
struct CollisionPairHash
{
	size_t operator()(const CollisionPair& p) const
	{
		auto h1 = std::hash<Collider*>{}(p.a);
		auto h2 = std::hash<Collider*>{}(p.b);

		return h1 ^ h2;
	}
};

// JSONから読み込むとき用のデータ
struct ColliderData
{
	std::string type = "Box";
	ColliderTag tag = ColliderTag::Default;

	SimpleMath::Vector3 position = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 size = SimpleMath::Vector3::One;
	SimpleMath::Vector3 vec = SimpleMath::Vector3::Zero;
};

// タグを自動で変換
inline void from_json(const nlohmann::json& j, ColliderTag& tag)
{
	std::string str = j.get<std::string>();

	if (str == "Default")
		tag = ColliderTag::Default;
	else if (str == "Ground")
		tag = ColliderTag::Ground;
	else if (str == "Item")
		tag = ColliderTag::Item;
	else if (str == "Player")
		tag = ColliderTag::Player;
	else if (str == "PlayerAttack")
		tag = ColliderTag::PlayerAttack;
	else if (str == "EnemyAttack")
		tag = ColliderTag::EnemyAttack;
	else if (str == "Leg")
		tag = ColliderTag::Leg;
	else
		tag = ColliderTag::Default;
}

// コライダー情報をまとめて入れれるように
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
	ColliderData, type, tag, position, size, vec
)


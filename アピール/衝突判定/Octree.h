#pragma once
#include <unordered_set>
class Collider;


// 各ノードに保存するこらーだーデータ
struct OctreeObject
{
	Collider* col = nullptr;	// 対象のコライダー
	bool isStatic = false;		// 動くか

	bool operator==(const OctreeObject& a)
	{
		return a.col == this->col;
	}
};

// 各ノード
struct OctreeNode
{
	std::vector<OctreeObject> objects{};						// ノード内にいるオブジェクト
};


// レベル level の先頭インデックス = (8^level - 1) / 7
static constexpr uint32_t LevelOffset(int level)
{
	return ((1u << (3 * level)) - 1) / 7;
}

class Octree
{	
public:

	Octree();
	~Octree();

	void Initialize();
	void Update(Collider* collider);
	void Finalize();

	// 指定位置のノードを取得
	OctreeNode* GetOctree(uint32_t morton);

	// 親ルートを取得
	OctreeNode* GetRoot() { return m_root[0].get(); }

	static constexpr int   MAX_LEVEL = 4;					// 葉のレベル（ルートが0）
	static constexpr int   GRID_SIZE = 1 << MAX_LEVEL;		// 1辺のセル数
	static constexpr int   OFFSET = GRID_SIZE / 2;			// 原点をグリッドの中央に
	static constexpr float WORLD_SIZE = 800.0f;             // ワールド1辺の長さ（仮の値）
	static constexpr float NODE_LENGTH = WORLD_SIZE / GRID_SIZE;

	static constexpr uint32_t NODE_COUNT = LevelOffset(MAX_LEVEL + 1);   // 全ノード数

	static constexpr uint32_t Morton(uint32_t x, uint32_t y, uint32_t z)
	{
		uint32_t m = 0;
		for (int b = 0; b < MAX_LEVEL; ++b)
		{
			m |= ((x >> b) & 1u) << (3 * b + 0);
			m |= ((y >> b) & 1u) << (3 * b + 1);
			m |= ((z >> b) & 1u) << (3 * b + 2);
		}
		return m;
	}

	// ノード全体を取得
	std::array<std::unique_ptr<OctreeNode>, NODE_COUNT>& GetTree() { return m_root; }


private:

	// ルートノード
	std::array<std::unique_ptr<OctreeNode>, NODE_COUNT> m_root;
	// オブジェクト数
	std::vector<Collider*> m_cols;

};


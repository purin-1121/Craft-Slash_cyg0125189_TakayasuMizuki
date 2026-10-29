#include "pch.h"
#include "OCtree.h"
#include "Collider.h"

Octree::Octree()
	: m_root{}
{
}

Octree::~Octree()
{
}

void Octree::Initialize()
{
}


void Octree::Update(Collider* collider)
{
	// コライダーの変更情報を初期化する
	collider->DirtyOff();
	
	// コライダーの始点と終点
	auto start = collider->GetCenter() - collider->GetSize() / 2;
	auto end = collider->GetCenter() + collider->GetSize() / 2;

	// ---- 開始位置と終了位置に接するノードを取得 ---- //

	auto ToCell = [&](float v)
		{
			int c = static_cast<int>(std::floor(v / NODE_LENGTH)) + OFFSET;
			return static_cast<uint32_t>(std::clamp(c, 0, GRID_SIZE - 1));   // 7 ではなく GRID_SIZE - 1
		};

	// 出した位置をモートン番号に直してノードを取得
	// 各値からそれぞれのビットを取り出し挟みあいをする
	// 開始地点
	uint32_t startNode = Morton(ToCell(start.x), ToCell(start.y), ToCell(start.z));
	uint32_t endNode = Morton(ToCell(end.x), ToCell(end.y), ToCell(end.z));

	// -----------------------------------------
	// 共通祖先を探す
	// -----------------------------------------
	
	// 異なるビットの割り出し
	uint16_t diff = startNode ^ endNode;

	// 何ビットシフトするか計算
	int level = MAX_LEVEL;
	if (diff != 0)
	{
		int highBit = std::bit_width(diff) - 1;
		level = MAX_LEVEL - 1 - highBit / 3;
	}

	uint32_t node = LevelOffset(level) + (startNode >> (3 * (MAX_LEVEL - level)));
	if (node == collider->GetNode())
	{
		return;
	}

	// 以前格納されていたノード
	OctreeNode* oldNode = GetOctree(collider->GetNode());

	// 古いノードから削除
	if (oldNode != nullptr)
	{
		auto& objects = oldNode->objects;

		if (!objects.empty())
		{
			objects.erase(
				std::remove_if(
					objects.begin(),
					objects.end(),
					[&](const OctreeObject& object)
					{
						return object.col == collider;
					}),
				objects.end()
			);
		}
	}

	// -----------------------------------------
	// 新しいノードに登録
	// -----------------------------------------
	
	// 新しいノード
	OctreeNode* newNode = GetOctree(node);

	newNode->objects.push_back(
		{
			collider,
			collider->IsStatic()
		}
	);

	// ノードのイテレータ取得
	auto it = std::find_if(m_root.begin(), m_root.end(), [&](const auto& node) { return node.get() == newNode; });

	// イテレータの位置を取得
	int d = 0;
	if (it != m_root.end())
	{
		d = std::distance(m_root.begin(), it);
	}

	// 現在所属しているノードを記録
	collider->SetNode(d);

	if (collider->GetLayer() == ColliderLayer::Chest)
	{
		char buf[128];
		sprintf_s(buf, "Chest: level=%d node=%d old=%d\n", level, (int)node, (int)collider->GetNode());
		OutputDebugStringA(buf);
	}
}

OctreeNode* Octree::GetOctree(uint32_t  morton)
{
	if (morton >= m_root.size())
		return nullptr;

	if (m_root[morton] == nullptr)
	{
		m_root[morton] = std::make_unique<OctreeNode>();
	}
	return m_root[morton].get();
}

void Octree::Finalize()
{
	for (auto& node : m_root)
	{
		node.reset();
	}

	m_cols.clear();
}




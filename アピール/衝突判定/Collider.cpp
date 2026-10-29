#pragma once

#include "pch.h"
#include "Collider.h"
#include "Game/GamePlay/GameObject/GameObject.h"

const BoxCollider BoxCollider::Zero(SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f }, SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f }, SimpleMath::Vector3::Zero);
const BoxCollider BoxCollider::One(SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f }, SimpleMath::Vector3{ 1.0f, 1.0f, 1.0f }, SimpleMath::Vector3::Zero);

const SphereCollider SphereCollider::Zero(0.0f, SimpleMath::Vector3{ 0.0f, 0.0f,0.0f }, SimpleMath::Vector3::Zero);
const SphereCollider SphereCollider::One(1.0f, SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f }, SimpleMath::Vector3::Zero);

const PlaneCollider PlaneCollider::Zero(SimpleMath::Vector3{ 0.0f,0.0f, 0.0f }, SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f }, SimpleMath::Quaternion());
const PlaneCollider PlaneCollider::One(SimpleMath::Vector3{ 0.0f,0.0f, 0.0f }, SimpleMath::Vector3{ 1.0f, 1.0f, 1.0f }, SimpleMath::Quaternion());

const CylinderCollider CylinderCollider::Zero(0.0f, 0.0f, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Up);
const CylinderCollider CylinderCollider::One(1.0f, 1.0f, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Up);


// ---- Collider ---- //
void Collider::Draw(
		 XMVECTORF32 color
		)
{
	Draw::GetInstance().DrawBox(
		position + m_offset,
		position + m_offset + size,
		color
	);
};

// ---- LineCollider ---- //
LineCollider::LineCollider()
	: LineCollider(
		SimpleMath::Vector3::Zero,
		SimpleMath::Vector3::One
	)
{
}

LineCollider::LineCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size)
	: Collider(
		pos,
		size,
		SimpleMath::Vector3::Zero,
		Type::Line,
        false
	)
	, crossPoint{}
	, distance{size}
	, normalized{XMVector3Normalize(size)}
{
}

void LineCollider::Clear()
{
}

// ---- TriangleCollider ---- //

TriangleCollider::TriangleCollider(SimpleMath::Vector3 p0, SimpleMath::Vector3 p1, SimpleMath::Vector3 p2, bool isTrigger)
    : Collider(
        p0, 
        SimpleMath::Vector3::Zero, 
        SimpleMath::Vector3::Zero, 
        Type::Triangle, 
        isTrigger
    )
    , v0(p0), v1(p1), v2(p2)
{
    // 作成された瞬間に「法線」を計算して記憶しておく！
    SimpleMath::Vector3 edge1 = v1 - v0;
    SimpleMath::Vector3 edge2 = v2 - v0;
    
    normal = edge1.Cross(edge2);
    normal.Normalize();
}

SimpleMath::Vector3 TriangleCollider::GetCenter() const
{
        return (v0 + v1 + v2) / 3.0f;
}

// ---- PlaneCollider ---- //

PlaneCollider::PlaneCollider()
 : PlaneCollider(
     SimpleMath::Vector3::Zero, 
     SimpleMath::Vector3{ 1.0f,1.0f, 1.0f },
      SimpleMath::Quaternion()
 )
{
    Clear();
}
PlaneCollider::PlaneCollider(float f)
 : PlaneCollider(
     SimpleMath::Vector3::Zero, 
     SimpleMath::Vector3{ f,f,f },
      SimpleMath::Quaternion()
 )
{

}

PlaneCollider::PlaneCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, SimpleMath::Quaternion rotato, bool isTrigger)
	: Collider(
		pos,
		size,
		SimpleMath::Vector3::Zero,
		Type::Plane,
        isTrigger
	)
{
	SimpleMath::Matrix mat = SimpleMath::Matrix::CreateFromQuaternion(rotato);

	this->vecRight = mat.Right();
	this->vecForward = mat.Forward();
	this->vec = mat.Up();
}

void PlaneCollider::Clear()
{
}

// ---- BoxCollider ---- //
BoxCollider::BoxCollider()
    : BoxCollider(
        {0,0,0}, 
        {1,1,1}, 
        {0,0,0}
    )
{
    Clear();
}

BoxCollider::BoxCollider(float f)
    : BoxCollider(
        { 0,0,0 }, 
        {f,f,f}, 
         SimpleMath::Vector3::Zero
    )
{
}

BoxCollider::BoxCollider(float f, bool center)
 : BoxCollider(
     { center ? SimpleMath::Vector3(- (f / 2), 0, -(f / 2)) : SimpleMath::Vector3::Zero },
     {f,f,f},
      SimpleMath::Vector3::Zero
 )
{
}

BoxCollider::BoxCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size, bool center, bool isTrigger)
        :BoxCollider(
            pos  ,
            size,
            SimpleMath::Vector3::Zero
        )
{
    m_offset = SimpleMath::Vector3{
                center ?
                    SimpleMath::Vector3(-(size.x / 4), 0, -(size.z / 4)) :
                    SimpleMath::Vector3::Zero };
}

BoxCollider::BoxCollider(SimpleMath::Vector3 pos, SimpleMath::Vector3 size,  SimpleMath::Vector3 vec, bool isTrigger)
    : Collider(
        pos, 
        size, 
        vec,
        Type::Box,
        isTrigger
    )
{
}

void BoxCollider::Clear()
{
}

bool BoxCollider::IsPointInside(const SimpleMath::Vector3& point) const
{
    SimpleMath::Vector3 minP = position;
    SimpleMath::Vector3 maxP = position + size;

    return point.x >= minP.x && point.x <= maxP.x
        && point.y >= minP.y && point.y <= maxP.y
        && point.z >= minP.z && point.z <= maxP.z;
}


// ---- SphereCollider ---- //

SphereCollider::SphereCollider()
    : SphereCollider(
        1, {0,0,0},{1,1,1}
    )
{
}

SphereCollider::SphereCollider(float f)
    : SphereCollider(
        f, {0,0,0}, {f,f,f}
    )
{
}

SphereCollider::SphereCollider(float r, SimpleMath::Vector3 pos,  SimpleMath::Vector3 vec, bool isTrigger)
    : Collider(
        pos, 
        {r,r,r}, 
        vec, 
        Type::Sphere,
        isTrigger
    )
    , r{ r }
{
}

void SphereCollider::Clear()
{
}

void SphereCollider::Draw(XMVECTORF32 color)
{
    SimpleMath::Vector3 center = GetPos() + m_offset; // 球の中心座標を取得
    float radius = r;                      // 半径を取得
    
    const int segments = 16; // 円を何分割して線分で表すか
    Draw::GetInstance().DrawSphere(segments, radius, center, Colors::White, 3);
};

bool SphereCollider::IsPointInside(const SimpleMath::Vector3& point) const
{
    return (point - GetCenter()).LengthSquared() <= (r * r);
}



// ---- Capsule ---- //
CapsuleCollider::CapsuleCollider()
    : CapsuleCollider(1, 1, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, false)
{
}

CapsuleCollider::CapsuleCollider(float f)
    : CapsuleCollider(f, f, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, false)
{
}

CapsuleCollider::CapsuleCollider(float r, float height, SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, bool isTrigger)
    : Collider(pos, SimpleMath::Vector3{ r, height, r }, vec, Type::Capsule, isTrigger)
    , r{ r }
    , height{ height }
    , halfHeight{ height * 0.5f }
    , axis{ vec }  
{
    axis.Normalize();
    this;
}

void CapsuleCollider::Clear()
{
}

void CapsuleCollider::Draw(XMVECTORF32 color)
{
    SimpleMath::Vector3 start = GetSegmentStart() + m_offset;
    SimpleMath::Vector3 end = GetSegmentEnd() + m_offset;

    const int segments = 16;
    Draw::GetInstance().DrawCapsule(segments, r, start, end, Colors::White);
};

bool CapsuleCollider::IsPointInside(const SimpleMath::Vector3& point) const
{
    SimpleMath::Vector3 start = GetSegmentStart();
    SimpleMath::Vector3 end = GetSegmentEnd();
    SimpleMath::Vector3 d = end - start;
    float lenSq = d.Dot(d);

    float t = 0.0f;
    if (lenSq > 0.0001f)
    {
        t = (point - start).Dot(d) / lenSq;
        t = MathLib::Clamp(t, 0.0f, 1.0f);
    }

    SimpleMath::Vector3 closest = start + t * d;
    return (point - closest).LengthSquared() <= (r * r);
}

// ---- CylinderCollider ---- //
CylinderCollider::CylinderCollider()
    : CylinderCollider(1.0f, 1.0f, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Up)
{
}

CylinderCollider::CylinderCollider(float f)
    : CylinderCollider(f, f, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Up)
{
}

CylinderCollider::CylinderCollider(float r, float height, SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, bool isTrigger)
    : Collider(pos, SimpleMath::Vector3{ r, height, r }, vec, Type::Cylinder, isTrigger)
    , r{ r }
    , height{ height }
    , halfHeight{ height * 0.5f }
{
    // ゼロベクトル対策(カプセルの時と同じガード)
    if (vec.LengthSquared() < 0.0001f)
    {
        axis = SimpleMath::Vector3::Up;
    }
    else
    {
        vec.Normalize();
        axis = vec;
    }
}

void CylinderCollider::Clear()
{
}

void CylinderCollider::Draw(XMVECTORF32 color)
{
    SimpleMath::Vector3 bottom = GetBottom() + m_offset;
    SimpleMath::Vector3 top = GetTop() + m_offset;

    const int segments = 16;
    Draw::GetInstance().DrawCylinder(segments, r, bottom, top, Colors::White);
}

bool CylinderCollider::IsPointInside(const SimpleMath::Vector3& point) const
{
    SimpleMath::Vector3 w = point - GetBottom();

    // 軸方向にどれだけ進んだ位置か（0~heightの範囲外なら蓋の外）
    float axialDist = w.Dot(axis);
    if (axialDist < 0.0f || axialDist > height)
    {
        return false;
    }

    // 軸に対する垂直成分（中心軸からの水平距離）
    SimpleMath::Vector3 perp = w - axis * axialDist;
    float perpDistSq = perp.LengthSquared();

    return perpDistSq <= (r * r);
}


// ---- OBBCollider ---- //

void OBBCollider::Draw(XMVECTORF32 color)
{
    SimpleMath::Matrix rot = CreateRotMatrix();

    SimpleMath::Vector3 Ax =
        SimpleMath::Vector3::Transform(GetAxisX(), rot) * GetHalfExtents().x;

    SimpleMath::Vector3 Ay =
        SimpleMath::Vector3::Transform(GetAxisY(), rot) * GetHalfExtents().y;

    SimpleMath::Vector3 Az =
        SimpleMath::Vector3::Transform(GetAxisZ(), rot) * GetHalfExtents().z;

    Draw::GetInstance().DrawOBB(
        GetPos(),
        Ax,
        Ay,
        Az,
        Colors::Red
    );

}

#include "pch.h"
#include "RayCast.h"

//===================================================================
// 平面(Plane) と 線分(Line) の当たり判定
//===================================================================
bool RayCaster::RayCast(LineCollider& line, const PlaneCollider& plane)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    Vector3 rayOrigin = plane.GetPos();
    Vector3 rayDir = plane.GetSize() - plane.GetPos();
    float lineLength = rayDir.Length();

    if (lineLength > 0.0f) {
        rayDir.Normalize();
    }

    // PlaneCollider の基底メンバ（position:平面上の点, vec:法線ベクトル）を使って平面を作る
    DirectX::SimpleMath::Plane p(plane.GetPos(), plane.GetVector());

    float distance = 0.0f;
    Ray ray(rayOrigin, rayDir);

    if (ray.Intersects(p, distance))
    {
        if (distance >= 0.0f && distance <= lineLength)
        {
            line.SetCrossPoint(rayOrigin + rayDir * distance);
            return true;
        }
    }

    return false;
}

bool RayCaster::RayCast(LineCollider& ray, const BoxCollider& box)
{
    // Match the bounds used by Collider::Draw and box collision detection.
    const auto start = ray.GetStart();
    const auto direction = ray.GetSize();
    if (direction.LengthSquared() == 0.0f)
        return false;

    const auto corner = box.GetPos() + box.GetPosOffset();
    const auto opposite = corner + box.GetSize();
    float enter = 0.0f;
    float exit = 1.0f;

    const auto intersectAxis = [&](float origin, float delta, float a, float b)
    {
        const float lower = std::min(a, b);
        const float upper = std::max(a, b);
        // A parallel segment hits this slab only when it starts inside.
        if (delta == 0.0f)
            return origin >= lower && origin <= upper;

        float nearT = (lower - origin) / delta;
        float farT = (upper - origin) / delta;
        if (nearT > farT)
            std::swap(nearT, farT);
        enter = std::max(enter, nearT);
        exit = std::min(exit, farT);
        return enter <= exit;
    };

    if (!intersectAxis(start.x, direction.x, corner.x, opposite.x)
        || !intersectAxis(start.y, direction.y, corner.y, opposite.y)
        || !intersectAxis(start.z, direction.z, corner.z, opposite.z))
        return false;

    ray.SetCrossPoint(start + direction * enter);
    return true;
}
bool RayCaster::RayCast(LineCollider& ray, const SphereCollider& sphere)
{
    return false;
}

bool RayCaster::RayCast(LineCollider& line, const  TriangleCollider& triangle)
{
    using namespace DirectX::SimpleMath;

    // ★修正: 未初期化の可能性がある normalized には頼らず、
    // Camerline.cpp で代入された line.size (方向 × 1000.0f) から確実に計算する！
    Vector3 rayOrigin = line.GetStart(); // 始点 (line.position)
    Vector3 rayDir = line.GetSize();          // 終点へのベクトル
    float lineLength = rayDir.Length(); // 本当の長さ(1000.0f)を測る

    // レイの長さが0なら判定しない
    if (lineLength < 0.000001f) {
        return false;
    }
    rayDir.Normalize(); // ここで確実に正規化（長さを1に）する

    // 三角形の頂点（※お使いのTriangleColliderのメンバ変数名に合わせて v0,v1,v2 または p0,p1,p2 にしてください）
    Vector3 v0 = triangle.GetVertex0();
    Vector3 v1 = triangle.GetVertex1();
    Vector3 v2 = triangle.GetVertex2();

    // メラー・トランボアの交差判定演算法（純粋な数学的判定）
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    Vector3 h = rayDir.Cross(edge2);
    float a_det = edge1.Dot(h);

    // レイと三角形が平行な場合は当たらない
    const float EPSILON = 0.0000001f;
    if (a_det > -EPSILON && a_det < EPSILON)
    {
        return false;
    }

    float f = 1.0f / a_det;
    Vector3 s = rayOrigin - v0;
    float u = f * s.Dot(h);

    // 交点が三角形のポリゴン外（外側）ならハズレ
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vector3 q = s.Cross(edge1);
    float v = f * rayDir.Dot(q);

    // 同様にポリゴン外ならハズレ
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    // レイの始点からの距離 t を計算
    float t = f * edge2.Dot(q);

    // 距離が0以上、かつレイの長さ（1000.0f）以内ならついにヒット！
    if (t > EPSILON && t <= lineLength)
    {
        // friend 宣言されているので private な crossPoint に直接書き込める
        line.SetCrossPoint(rayOrigin + rayDir * t);
        return true;
    }

    return false;
}

bool RayCaster::RayCast(LineCollider& ray, Collider* target)
{
    if (target->GetType() == Type::Plane)    return RayCast(ray, *dynamic_cast<PlaneCollider*>(target));
    if (target->GetType() == Type::Box)      return RayCast(ray, *dynamic_cast<BoxCollider*>(target));
    if (target->GetType() == Type::Sphere)   return RayCast(ray, *dynamic_cast<SphereCollider*>(target));
    if (target->GetType() == Type::Triangle) return RayCast(ray, *dynamic_cast<TriangleCollider*>(target));
    return false;
}

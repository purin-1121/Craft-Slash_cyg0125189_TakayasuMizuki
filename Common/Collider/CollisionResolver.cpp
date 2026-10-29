#include "pch.h"
#include "CollisionResolver.h"
#include "Game/GamePlay/GameObject/GameObject.h"

Direction CollisionResolver::GetCollisionSide(BoxCollider& boxA, BoxCollider& boxB)
{
    auto aPos = boxA.GetPos();
    auto aOffset = boxA.GetPosOffset();
    auto aSize = boxA.GetSize();
    auto bPos = boxB.GetPos();
    auto bOffset = boxB.GetPosOffset();
    auto bSize = boxB.GetSize();

    float a_centerX = aPos.x + aOffset.x + aSize.x / 2;
    float a_centerY = aPos.y + aOffset.y + aSize.y / 2;
    float a_centerZ = aPos.z + aOffset.z + aSize.z / 2;
    float b_centerX = bPos.x + bOffset.x + bSize.x / 2;
    float b_centerY = bPos.y + bOffset.y + bSize.y / 2;
    float b_centerZ = bPos.z + bOffset.z + bSize.z / 2;

    float ox = (aSize.x + bSize.x) / 2 - std::abs(a_centerX - b_centerX);
    float oy = (aSize.y + bSize.y) / 2 - std::abs(a_centerY - b_centerY);
    float oz = (aSize.z + bSize.z) / 2 - std::abs(a_centerZ - b_centerZ);

    // 最も重なりが小さい軸（押し戻す軸）を判定する
    if (oy < ox && oy < oz)
    {
        // Y軸方向の衝突：aがbより上にあれば、bの「トップ（上面）」にのっている
        return (a_centerY > b_centerY) ? Direction::top : Direction::down;
    }
    else if (ox < oz)
    {
        // X軸方向の衝突
        return (a_centerX > b_centerX) ? Direction::left : Direction::right;
    }
    else
    {
        // Z軸方向の衝突
        return (a_centerZ > b_centerZ) ? Direction::front : Direction::back;
    }
}

Direction CollisionResolver::GetCollisionSide(Collider* a, Collider* b)
{
    if (a->GetType() != Type::Box || b->GetType() != Type::Box)
        return Direction::null;
    auto boxA = dynamic_cast<BoxCollider*>(a);
    auto boxB = dynamic_cast<BoxCollider*>(b);

    return GetCollisionSide(*boxA, *boxB);
}

void CollisionResolver::RestrictMovementOnContact(Direction d, GameObject& obj)
{
    switch (d)
    {
    case Direction::top:
        obj.Flying(GroundState::Ground);
        if (obj.GetVelocity().y < 0)
            obj.SetVelocity_Y(0);
        break;
    case Direction::down:
        obj.Flying(GroundState::Fly);
        if (obj.GetVelocity().y > 0)
            obj.SetVelocity_Y(0);
        break;
    case Direction::left:
        obj.Flying(GroundState::Fly);
        if (obj.GetVelocity().x > 0)
            obj.SetVelocity_X(0);
        break;
    case Direction::right:
        obj.Flying(GroundState::Fly);
        if (obj.GetVelocity().x < 0)
            obj.SetVelocity_X(0);
        break;
    case Direction::front:
        obj.Flying(GroundState::Fly);
        if (obj.GetVelocity().z < 0)
            obj.SetVelocity_Z(0);
        break;
    case Direction::back:
        obj.Flying(GroundState::Fly);
        if (obj.GetVelocity().z > 0)
            obj.SetVelocity_Z(0);
        break;
    default:
        break;
    }
}


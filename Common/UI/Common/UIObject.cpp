#include "pch.h"
#include "Common/UI/Common/UIObject.h"
#include "Common/Collider/NarrowPhase.h"

bool UIObject::MouseOn()
{
	auto mouse = Mouse::Get().GetState();
	SimpleMath::Vector2 pos = { static_cast<float>(mouse.x),static_cast<float>(mouse.y)};

	return NarrowPhase::IsColliding(pos, PlaneCollider{m_worldPosition, m_scale, SimpleMath::Quaternion::Identity}).is;
}

UIObject::UIObject()
	: UIObject(SimpleMath::Vector3 ::One)
{
}

UIObject::UIObject(SimpleMath::Vector3  pos, SimpleMath::Vector3  scale)
	: m_worldPosition{pos}
	, m_scale{scale}
	, m_orderLayer{}
	, m_parent{nullptr}
	, m_isActive{true}
{
}

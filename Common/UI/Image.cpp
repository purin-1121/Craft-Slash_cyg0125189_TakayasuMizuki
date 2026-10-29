#include "pch.h"
#include "Image.h"

Image::Image( SimpleMath::Vector2 pos, SimpleMath::Color color, SimpleMath::Vector2 scale, const std::wstring& image, bool active)
    : UIObject(SimpleMath::Vector3(pos),SimpleMath::Vector3{scale.x, scale.y, 1})
    , m_image{image}
	, m_color{color}
{
	SetActive(active);
}

Image::~Image()
{
}

void Image::Initialize(const GameContext& gameContext)
{
	m_ghHundle = ResourceManager::GetInstance().GetTexture(m_image);
}

void Image::Update(const GameContext& gameContext,const float& elapsedTime)
{
}

void Image::Render(const GameContext& gameContext, const Camera& camera)
{
}

void Image::RenderText()
{
    // ‰æ‘œ‚Ì•`‰æ
	SimpleMath::Vector2 pos = { m_worldPosition.x + m_posOffset.x, m_worldPosition.y + m_posOffset.y };
	Draw::GetInstance().DrawSpriteBox(m_ghHundle, pos, { m_scale.x, m_scale.y }, m_color);
}

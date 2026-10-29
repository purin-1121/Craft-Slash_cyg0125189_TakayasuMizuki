#include "pch.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::m_ResourceManager = nullptr;

ResourceManager::ResourceManager()
	: m_imageManager{}
	, m_soundManager{}
	, m_device{}
{

}


ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize(ID3D11Device* device)
{
	m_device = device;
	m_imageManager = std::make_unique<ImageManager>();
	m_soundManager = std::make_unique<SoundManager>();

	// ---- ‰æ‘œ“Ç‚Ýž‚Ý ---- //
	


	// ---- ‰¹º“Ç‚Ýž‚Ý ---- //

	m_soundManager->LoadSound(L"Resources/Sounds/Click.wav", L"Click");
	//m_soundManager->LoadSound(L"Resources/Sounds/TitleBGM.wav", L"TitleBGM");
}

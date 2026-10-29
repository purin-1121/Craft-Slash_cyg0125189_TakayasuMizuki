#pragma once

#include "pch.h"
#include "ObjectManager.h"
namespace ObjectLib
{
	std::unordered_map<std::string, std::unique_ptr<Model>> m_models;
	std::unordered_map<std::string, std::shared_ptr<ModelResource>> m_animatedModels;
};


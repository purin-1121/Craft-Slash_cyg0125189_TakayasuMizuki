#pragma once
#include <string>
#include <vector>

#include "Tools/nlohmann/json.hpp"


struct BulletData
{
	// Ž¯•Ê—p–¼‘O
	std::string name = "none";
	// ID
	int id = 0;
	// ƒ‚ƒfƒ‹‚ÌƒpƒX
	std::string modelPath = "none";
	// ”š”j”¼Œa
	float bombRadius = 0.0f;
	// ”š”jˆÐ—Í
	float power = 0.0f;
	// –C’e‚Ì”¼Œa
	float bulletRadius = 0.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
	BulletData, name, id, modelPath, bombRadius, power, bulletRadius
)
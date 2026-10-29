//====================================================//
// ファイル名   : ObjectManager.h
// 作成者       : 高安瑞貴
//
// 概要 : ゲームで扱う3Dモデルを管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <memory>
#include <fstream>
#include <unordered_map>
#include <Model.h>
#include <Effects.h>
#include <filesystem>
#include <iostream>
#include "Tools/nlohmann/json.hpp"
#include "Common/Manager/StorageManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelResource.h"

//====================================================//
// 宣言
//====================================================//
using json = nlohmann::json;

// ノード階層を再帰的に出力する
void PrintNodeHierarchy(const aiNode* node, int depth = 0)
{
	if (node == nullptr)
		return;

	// 階層の深さに応じてインデントする
	std::string indent(depth * 2, ' ');

	std::string message =
		indent +
		std::string(node->mName.C_Str()) +
		"\n";

	OutputDebugStringA(message.c_str());

	// 子ノードを順番に出力
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		PrintNodeHierarchy(node->mChildren[i], depth + 1);
	}
}



struct ModelData
{
	// モデル
	std::unique_ptr<DirectX::Model> model;
	// モデルインスタンス
	DirectX::Model* modelInstance;
	// SDKアニメーション
	std::unique_ptr<DX::AnimationSDKMESH> animation;
	// スケルトン
	std::unique_ptr<DirectX::ModelBone::TransformArray> pBone;
	// アニメーションスケルトン
	std::unique_ptr<DirectX::ModelBone::TransformArray> animeBone;
	// アニメーション時間
	float animTime;
};

inline void TestFBXLoad()
{
	Assimp::Importer importer;

	// FBXファイルを読み込む
	const aiScene* scene = importer.ReadFile(
		"Resources/Models/Neutral_Idle.fbx",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights
	);

	PrintNodeHierarchy(scene->mRootNode);

	// 読み込み失敗を確認
	if (scene == nullptr ||
		(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
		scene->mRootNode == nullptr)
	{
		std::cerr << "FBX読み込み失敗: "
			<< importer.GetErrorString()
			<< std::endl;
		return;
	}

	std::cout << "FBX読み込み成功\n";
	std::cout << "メッシュ数: " << scene->mNumMeshes << '\n';
	std::cout << "アニメーション数: "
		<< scene->mNumAnimations << '\n';

	std::cout << "ボーン数: ";

	unsigned int boneCount = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		boneCount += scene->mMeshes[i]->mNumBones;
	}

	std::cout << boneCount << '\n';

	// アニメーションの詳細
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		const aiAnimation* anim = scene->mAnimations[i];

		std::string name = anim->mName.C_Str();
		std::string message = "アニメーション名: " + name + '\n';
		OutputDebugStringA(message.c_str());

		name = std::to_string(anim->mNumChannels);
		message = "チャンネル数: " + name + '\n';
		OutputDebugStringA(message.c_str());

		name = std::to_string(anim->mTicksPerSecond);
		message = "mTicksPerSecond: " + name + '\n';
		OutputDebugStringA(message.c_str());

		name = std::to_string(anim->mDuration);
		message = "mDuration: " + name + '\n';
		OutputDebugStringA(message.c_str());

		// 各チャンネルの情報を表示
		for (unsigned int c = 0; c < anim->mNumChannels; ++c)
		{
			const aiNodeAnim* channel = anim->mChannels[c];

			std::string message =
				"Node: " + std::string(channel->mNodeName.C_Str()) +
				", PositionKeys: " +
				std::to_string(channel->mNumPositionKeys) +
				", RotationKeys: " +
				std::to_string(channel->mNumRotationKeys) +
				", ScalingKeys: " +
				std::to_string(channel->mNumScalingKeys) + "\n";

			OutputDebugStringA(message.c_str());
		}

	}
}


namespace ObjectLib
{
	// モデルマップ
	extern std::unordered_map<std::string, std::unique_ptr<Model>> m_models;
	// 追加：Assimpで読み込んだモデルリソース
	extern std::unordered_map<std::string,std::shared_ptr<ModelResource>> m_animatedModels;

	// モデルとアニメーションの読み込み（バインド処理も行う）
	inline void CreateResources(ID3D11Device1* device, DirectX::IEffectFactory& fx)
		{
		// ------ モデルをJsonから読み込み ------ //

		std::string str = StorageManager::LoadTextFile("Resources/Jsons/Model.json");
		const std::string MODEL_BASE_PATH = "Resources/Models/";

		json m_json;

		auto modelLoad = [](bool loaded, std::string modelPath, std::string name, std::shared_ptr<ModelResource> resource)
			{
				if (!loaded)
				{
					std::cerr << "FBX load failed: " << modelPath << std::endl;
					return;
				}

				m_animatedModels.emplace(name, std::move(resource));
			};

		try
		{
			m_json = json::parse(str); // jsonデータを読み込む

			// m_jsonが配列であることを確認
			if (m_json.is_array())
			{
				// 変更点 3: 配列の各要素をループ処理
				for (const auto& item : m_json)
				{
					// 名前とファイルのパスがあることを確認
					if (item.is_object() &&
						item.contains("name") &&
						item.contains("modelPath") 
						)
					{
						std::string name;	// モデルのキー
						std::string modelPath;	// モデルのファイルパス
						name = item["name"].get<std::string>();
						modelPath = item["modelPath"].get<std::string>();
						modelPath = MODEL_BASE_PATH + modelPath;

						// 拡張子取り出し
						std::string exe = modelPath.substr(modelPath.length() - 3);

						if (exe == "fbx")
                        {
                            bool loaded = false;

                            // A base FBX has no "baseModel" field. Animation-only FBXs
                            // refer to the base model entry by its JSON name.
                            if (item.contains("baseModel") && item["baseModel"].is_array())
                            {
                                std::vector<std::string> baseName =
                                    item["baseModel"].get<std::vector<std::string>>();

								auto varPath = modelPath;
								std::string deleteName = varPath.erase(varPath.length() - 4 , 4);

								for (auto& n : baseName)
								{

									const auto baseIt = m_animatedModels.find(n);
									if (baseIt == m_animatedModels.end())
									{
										std::cerr << "Base animated model must be listed before its clips: "
											<< n << std::endl;
										continue;
									}
									auto resource = std::make_shared<ModelResource>();
									loaded = resource->LoadAnimation(modelPath, baseIt->second);
									const std::string suffix = "Idle";

									if (n.size() < suffix.size() ||
										n.compare(n.size() - suffix.size(), suffix.size(), suffix) != 0)
									{
										std::cerr << "Base model name must end with 'Idle': "
											<< n << std::endl;
										continue;
									}

									const std::string resourceName =
										n.substr(0, n.size() - suffix.size()) + name;
									modelLoad(loaded, modelPath, resourceName, std::move(resource));
								}
                            }
                            else
                            {
								auto resource = std::make_shared<ModelResource>();
                                loaded = resource->Load(modelPath);
								modelLoad(loaded, modelPath, name, std::move(resource));
                            }

                            
                        }
						else
						{
							// 読み込みのための型変換
							std::wstring pathStr(modelPath.begin(), modelPath.end());
							try
							{
								std::cout << "Load Model : " << name << " / " << modelPath << std::endl;

								auto model = Model::CreateFromCMO(device, pathStr.c_str(), fx);

								m_models.emplace(name, std::move(model));
							}
							catch (const std::exception& e)
							{
								std::cerr << "  -> 読み込み失敗 : " << name << std::endl;
								std::cerr << "  -> Path : " << modelPath << std::endl;
								std::cerr << "  -> Error : " << e.what() << std::endl;
							}
						}
					}
				}
			}
		}
		catch (json::parse_error& e)
		{
			// jsonのパースに失敗した場合
			exit(1);
		}
	}
};


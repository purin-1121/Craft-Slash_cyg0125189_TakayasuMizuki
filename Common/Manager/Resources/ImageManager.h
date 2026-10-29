#pragma once
//====================================================//
// ファイル名   : ImageManager.h
// 作成者       : 高安瑞貴
//
// 概要 : このゲームの入力情報を管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include<memory>
#include "pch.h"
#include <string>
#include <tchar.h> 
#include <unordered_map>
#include <WICTextureLoader.h>

//====================================================//
// 宣言
//====================================================//

class ImageManager
{

private:

    // テクスチャマップ
	std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_images;

public:

    // コンストラクタ
    ImageManager() :m_images{} {};

    // 初期化
	void Initialize() {};

	/// <summary>
	/// 画像の追加
	/// </summary>
	/// <param name="filename"> ファイルのパス </param>
	void AddImage(ID3D11Device* device, const std::wstring& filename) {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        HRESULT hr = CreateWICTextureFromFile(device, filename.c_str(), nullptr, srv.GetAddressOf());
        
        if (SUCCEEDED(hr)) {
            m_images[filename] = srv;
        }
    };

	/// <summary>
	/// 画像の追加
	/// </summary>
	/// <param name="filename"> ファイルのパス </param>
	/// <param name="name">呼び出し名</param>
	void AddImage(const std::wstring& filename, const std::wstring name) {
        filename;
        name;
    };

	/// <summary>
	/// 指定の画像を渡す関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="name">画像のキー</param>
	/// <returns>画像 (ない場合は作成して返す)</returns>
	ID3D11ShaderResourceView* GetTexture(ID3D11Device* device, const std::wstring& name) {
        // すでに読み込み済みならそれを返す
        if (m_images.count(name) > 0) {
            return m_images[name].Get();
        }

        // 未読なら CreateWICTextureFromFile で読み込んで map に保存
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        HRESULT hr = CreateWICTextureFromFile(device, name.c_str(), nullptr, srv.GetAddressOf());
        
        if (SUCCEEDED(hr)) {
            m_images[name] = srv;
            return srv.Get();
        }
        return nullptr;
    }

    /// <summary>
	/// 指定の画像を渡す関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="device">画像のパス</param>
	/// <param name="name">画像のキー</param>
	/// <returns>画像 (ない場合は作成して返す)</returns>
	ID3D11ShaderResourceView* GetTexture(ID3D11Device* device, const std::wstring& path, const std::wstring& name) {
        // すでに読み込み済みならそれを返す
        if (m_images.count(name) > 0) {
            return m_images[name].Get();
        }

        // 未読なら CreateWICTextureFromFile で読み込んで map に保存
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        HRESULT hr = CreateWICTextureFromFile(device, path.c_str(), nullptr, srv.GetAddressOf());
        
        if (SUCCEEDED(hr)) {
            m_images[name] = srv;
            return srv.Get();
        }
        return nullptr;
    }
};


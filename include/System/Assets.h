#pragma once

#include <filesystem>
#include <vector>

#include "Model.h"
#include "Material.h"

class Assets
{
	struct Asset
	{
		std::filesystem::path path;
		void* asset;
	};

	inline static std::vector<Asset> _assets;

	template <typename T> static T* findAsset(const std::filesystem::path& path)
	{
		auto it = std::ranges::find_if(_assets, [&path](const auto& asset) { return asset.path == path; });
		return it != _assets.end() ? (T*)it->asset : nullptr;
	}

public:
	template <typename T> static T* load(const std::filesystem::path& path);

	template <typename T> static std::filesystem::path findAssetPath(const T* asset);
};

template <typename T> T* Assets::load(const std::filesystem::path& path)
{
	if (auto asset = findAsset<T>(path)) return asset;

	T* asset = new T(path);
	_assets.push_back({path, asset});
	return asset;
}

template <typename T> std::filesystem::path Assets::findAssetPath(const T* asset)
{
	auto it = std::ranges::find_if(_assets, [asset](const auto& existingAsset) { return existingAsset.asset == asset; });
	return it != _assets.end() ? it->path : "";
}

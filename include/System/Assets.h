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

	template <typename T>
	static T* findAsset(const std::filesystem::path& path)
	{
		auto it = std::ranges::find_if(_assets, [&path](const auto& asset) { return asset.path == path; });
		return it != _assets.end() ? (T*)it->asset : nullptr;
	}

public:
	template <typename T>
	static T* import(const std::filesystem::path& path);
};

template <typename T>
T* Assets::import(const std::filesystem::path& path)
{
	if (auto asset = findAsset<T>(path)) return asset;

	T* asset = new T(path);
	_assets.push_back({path, asset});
	return asset;
}

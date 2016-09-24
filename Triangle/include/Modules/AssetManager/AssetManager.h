#pragma once
#include "Modules/TemplateUtils/TypeIndex.h"

#include <tuple>
#include <unordered_map>

namespace AssetManager
{
	using AssetID = uint32_t;

	template<class... AssetTypes>
	class AssetManager
	{
	public:
		template<class AssetType>
		void addAsset(AssetType at)
		{
			constexpr int index = type_index<AssetType, AssetTypes...>();
			std::get<index>(assets).push_back(std::make_pair(generateID(), at));
		}

		template<class AssetType>
		AssetType& getAssetByID(AssetID id)
		{
			constexpr int index = type_index<AssetType, AssetTypes...>();
			return std::get<index>(assets).at(id);
		}

	private:
		std::tuple<std::unordered_map<AssetID, AssetTypes>...> assets;

		AssetID lastID = 0;
		AssetID generateID()
		{
			return (++lastID);
		}
	};
}

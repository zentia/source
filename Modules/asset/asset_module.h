#pragma once

#include <filesystem>
#include <fstream>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "spdlog/spdlog.h"

namespace source_runtime
{
	class asset_module
	{
	public:
		template <typename AssetType>
		static AssetType load_asset(const std::string& asset_url)
		{
			// read json file to string
			std::filesystem::path asset_path = get_full_path(asset_url);
			std::ifstream asset_json_file(asset_path);
			if (!asset_json_file)
			{
				SPDLOG_ERROR("open file: {} failed!", asset_path.generic_string());
				return AssetType();
			}

			std::stringstream buffer;
			buffer << asset_json_file.rdbuf();

			return rfl::json::read<AssetType, rfl::SnakeCaseToCamelCase>(buffer).value();
		}

		template <typename AssetType>
		static bool save_asset(const AssetType& asset, const std::string& asset_url)
		{
			std::ofstream asset_json_file(get_full_path(asset_url));
			if (!asset_json_file)
			{
				SPDLOG_ERROR("open file {} failed!", asset_url);
			}

			rfl::json::write<rfl::SnakeCaseToCamelCase>(asset, asset_json_file);

			return true;
		}

		static std::filesystem::path get_full_path(const std::string& relative_path);
	};
}

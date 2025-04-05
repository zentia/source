#pragma once

#include <filesystem>
#include <fstream>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "spdlog/spdlog.h"

namespace source_module::asset
{
	class asset_module
	{
	public:
		explicit asset_module(std::filesystem::path&& root_folder);
		template <typename AssetType>
		AssetType load_asset(const std::string& asset_url)
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
		bool save_asset(const AssetType& asset, const std::string& asset_url)
		{
			std::ofstream asset_json_file(get_full_path(asset_url));
			if (!asset_json_file)
			{
				SPDLOG_ERROR("open file {} failed!", asset_url);
			}

			rfl::json::write<rfl::SnakeCaseToCamelCase>(asset, asset_json_file);

			return true;
		}

		template <typename AssetType>
		static AssetType load_config(const std::string& asset_path)
		{
			std::ifstream asset_json_file(asset_path);
			if (!asset_json_file)
			{
				SPDLOG_ERROR("open file: {} failed!", asset_path);
				return AssetType();
			}

			std::stringstream buffer;
			buffer << asset_json_file.rdbuf();

			return rfl::json::read<AssetType, rfl::SnakeCaseToCamelCase>(buffer).value();
		}
	private:
		[[nodiscard]]std::filesystem::path get_full_path(const std::string& relative_path) const;
		std::filesystem::path m_root_folder_;
	};
}

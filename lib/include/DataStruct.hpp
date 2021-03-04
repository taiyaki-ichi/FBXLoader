#pragma once
#include<variant>
#include<vector>
#include<string>

namespace FBXL
{
	using DataVariant = std::variant <
		std::int16_t,
		bool,
		std::int32_t,
		float,
		double,
		std::int64_t,
		std::vector<float>,
		std::vector<double>,
		std::vector<std::int64_t>,
		std::vector<std::int32_t>,
		std::vector<bool>,
		std::string,
		std::vector<unsigned char>//バイナリデータ
	>;


	struct Node {
		std::string mName{};
		std::vector<DataVariant> mProperties{};
		std::vector<Node> mChildren{};
	};


	struct Data {
		std::string mFilePath{};
		std::uint32_t mVersion{};
		std::vector<Node> mNodes{};
	};

}
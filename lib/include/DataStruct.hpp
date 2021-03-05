#pragma once
#include<variant>
#include<vector>
#include<string>
#include<optional>

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



	//
	//以下、Objects内の構造体
	//どれも仮の姿
	//

	struct NodeAttributeOption
	{
		std::string mProp70Strings[3]{};
		double mProp70Double;
	};
	
	struct NodeAttribute
	{
		//prop
		std::int64_t mIndex{};
		std::string mMagic{};
		std::string mName{};


		//以下、children

		//prop70
		std::optional<NodeAttributeOption> mOption{};

		//TypeFlag
		std::string mTypeFlag{};
	};
	



}
#pragma once
#include<variant>
#include<vector>
#include<string>
#include<optional>
#include<type_traits>

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
		std::string name{};
		std::vector<DataVariant> properties{};
		std::vector<Node> children{};
	};


	struct Data {
		std::string filePath{};
		std::uint32_t version{};
		std::vector<Node> nodes{};
	};



	//
	//以下、Objects内の構造体
	//どれも仮の姿
	//

	//これ使わないかも
	struct NodeAttributeOption
	{
		std::string prop70Strings[3]{};
		double prop70Double;
	};
	
	struct NodeAttribute
	{
		//prop
		std::int64_t index{};
		std::string magic{};
		std::string name{};


		//以下、children

		//prop70
		std::optional<NodeAttributeOption> option{};

		//TypeFlag
		std::string typeFlag{};
	};
	


	template<
		typename VertexContainer,
		typename IndexContainer
	>
		struct GeometryMesh
	{

		std::string name{};

		VertexContainer vertices{};
		IndexContainer indeces{};

		//UVなどが追加される予定

	};






}
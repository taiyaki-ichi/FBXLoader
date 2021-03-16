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


	struct PrimitiveData {
		std::string filePath{};
		std::uint32_t version{};
		std::vector<Node> nodes{};
	};



	//
	//以下、Objects内の構造体
	//どれも仮の姿
	//

	template<
		typename VertexContainer,
		typename IndexContainer
	>
		struct GeometryMesh
	{

		static_assert(std::is_default_constructible_v<VertexContainer>);
		static_assert(std::is_default_constructible_v<IndexContainer>);

		std::string name;

		VertexContainer vertices{};
		IndexContainer indeces{};

		std::vector<std::int32_t> materialRange{};

		//UVなどが追加される予定

	};


	template<typename Vector3D>
	struct ModelMesh
	{
		//nameいらないかも
		std::string name;

		Vector3D localTranslation;
		Vector3D localRotation;
		Vector3D localScaling;
	};


	template<typename Vector3D>
	struct Material
	{
		//
		std::string name;

		Vector3D diffuseColor;
		double diffuseFactor;

		Vector3D ambientColor;
		double ambientFactor;

		Vector3D specularColor;
		double specularFactor;

		//Phongの反射モデル参照
		//影つけるやつ
		double shininess;
		double shininessFactor;

		//自身が放つ光
		Vector3D emissive;
		double emissiveFactor;

		//透過度
		//alpha成分のみ作用
		double transparent;
		double transparentFactor;

		Vector3D reflectionColor;
		double reflectionFactor;

		//BumpMap,NormapMapとか追加するかも
	};


	//最終的な目標
	template<typename Vector3D>
	struct Model3D
	{

		std::vector<Vector3D> vertices;
		std::vector<std::int32_t> indeces;

		std::vector<Material<Vector3D>> material;

		//マテリアルの適用範囲
		std::vector<std::int32_t> materialRange;


		//std::vector<Texture> textures

	};

}
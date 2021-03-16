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
		std::vector<unsigned char>//�o�C�i���f�[�^
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
	//�ȉ��AObjects���̍\����
	//�ǂ�����̎p
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

		//UV�Ȃǂ��ǉ������\��

	};


	template<typename Vector3D>
	struct ModelMesh
	{
		//name����Ȃ�����
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

		//Phong�̔��˃��f���Q��
		//�e������
		double shininess;
		double shininessFactor;

		//���g������
		Vector3D emissive;
		double emissiveFactor;

		//���ߓx
		//alpha�����̂ݍ�p
		double transparent;
		double transparentFactor;

		Vector3D reflectionColor;
		double reflectionFactor;

		//BumpMap,NormapMap�Ƃ��ǉ����邩��
	};


	//�ŏI�I�ȖڕW
	template<typename Vector3D>
	struct Model3D
	{

		std::vector<Vector3D> vertices;
		std::vector<std::int32_t> indeces;

		std::vector<Material<Vector3D>> material;

		//�}�e���A���̓K�p�͈�
		std::vector<std::int32_t> materialRange;


		//std::vector<Texture> textures

	};

}
#pragma once
#include"include/DataStruct.hpp"
#include"DescriptorHeap/DescriptorHeap.hpp"
#include"Resource/VertexBufferResource.hpp"
#include"Resource/ConstantBufferResource.hpp"
#include"CommandList.hpp"
#include"Resource/ColorTextureResource.hpp"
#include"Resource/TextureResource.hpp"
#include"Utility.hpp"
#include<vector>

#include<DirectXMath.h>

namespace DX12
{
	//とりあえず
	template<typename Vector3D>
	struct Material
	{
		Vector3D diffuseColor;
		float diffuseFactor;

		Vector3D ambientColor;
		float ambientFactor;

		Vector3D specularColor;
		float specularFactor;
	};

	//とりあえず　
	struct SceneData
	{
		DirectX::XMMATRIX view{};
		DirectX::XMMATRIX proj{};
		DirectX::XMFLOAT3 eye{};
	};


	class FBXModel
	{
		VertexBufferResource vertexBufferResource{};
		DescriptorHeap<DescriptorHeapType::CBV_SRV_UAV> descriptorHeap{};
		std::vector<ConstantBufferResource> materialConstantBufferResources{};
		ConstantBufferResource sceneDataConstantBufferResource{};

		std::vector<std::int32_t> materialRange{};

		std::vector<TextureResource> textureResources{};
		WhiteTextureResource whiteTextureResource{};

	public:
		template<typename Vector2D, typename Vector3D>
		void Initialize(Device*, CommandList*, FBXL::Model3D<Vector2D, Vector3D>&&);

		void Draw(CommandList*);

		void MapSceneData(SceneData&&);
	};

	//
	//以下、定義
	//

	template<typename Vector2D, typename Vector3D>
	inline void FBXModel::Initialize(Device* device, CommandList* cl, FBXL::Model3D<Vector2D, Vector3D>&& model)
	{
		vertexBufferResource.Initialize(device, sizeof(model.vertices[0]) * model.vertices.size(), sizeof(model.vertices[0]));
		vertexBufferResource.Map(std::move(model.vertices));

		whiteTextureResource.Initialize(device);

		//SceneDataとマテリアルの分
		descriptorHeap.Initialize(device, 1 + model.material.size() * 2);

		sceneDataConstantBufferResource.Initialize(device, sizeof(SceneData));
		descriptorHeap.PushBackView(device, &sceneDataConstantBufferResource);

		materialConstantBufferResources.resize(model.material.size());
		for (std::size_t i = 0; i < model.material.size(); i++)
		{
			Material<Vector3D> materialTmp[1]{};
			materialTmp[0].diffuseColor = model.material[i].diffuseColor;
			materialTmp[0].diffuseFactor = model.material[i].diffuseFactor;
			materialTmp[0].ambientColor = model.material[i].ambientColor;
			materialTmp[0].ambientFactor = model.material[i].ambientFactor;
			materialTmp[0].specularColor = model.material[i].specularColor;
			materialTmp[0].specularFactor = model.material[i].specularFactor;

			ConstantBufferResource constantBufferTmp{};
			constantBufferTmp.Initialize(device, sizeof(Material<Vector3D>));
			constantBufferTmp.Map(materialTmp);

			materialConstantBufferResources[i] = std::move(constantBufferTmp);
			descriptorHeap.PushBackView(device, &materialConstantBufferResources[i]);

			bool textureFlag = false;
			if (model.material[i].diffuseColorTexturePath)
			{
				auto result = GetTexture(model.material[i].diffuseColorTexturePath.value());
				if (result)
				{
					textureFlag = true;
					auto [metaData, scratchImage] = std::move(result.value());

					TextureResource textureResourceTmp{};
					textureResourceTmp.Initialize(device, cl, &metaData, &scratchImage);

					descriptorHeap.PushBackView(device, &textureResourceTmp);
					textureResources.emplace_back(std::move(textureResourceTmp));
				}
			}

			if (!textureFlag)
				descriptorHeap.PushBackView(device, &whiteTextureResource);
		}


		materialRange = std::move(model.materialRange);
	}


	void FBXModel::Draw(CommandList* cl)
	{
		unsigned int vertexOffset = 0;

		cl->Get()->IASetVertexBuffers(0, 1, &vertexBufferResource.GetView());

		cl->Get()->SetDescriptorHeaps(1, &descriptorHeap.Get());

		cl->Get()->SetGraphicsRootDescriptorTable(0, descriptorHeap.GetGPUHandle(0));

		for (std::size_t i = 0; i < materialRange.size(); i++)
		{
			cl->Get()->SetGraphicsRootDescriptorTable(1, descriptorHeap.GetGPUHandle(i * 2 + 1));
			cl->Get()->DrawInstanced(materialRange[i], materialRange[i] / 3, vertexOffset, vertexOffset / 3);

			vertexOffset += materialRange[i];
		}
	}

	inline void FBXModel::MapSceneData(SceneData&& sceneData)
	{
		SceneData* ptr = nullptr;
		sceneDataConstantBufferResource.Get()->Map(0, nullptr, (void**)&ptr);

		*ptr = std::move(sceneData);

		sceneDataConstantBufferResource.Get()->Unmap(0, nullptr);
	}
}
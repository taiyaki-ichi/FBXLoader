#pragma once
#include"include/DataStruct.hpp"
#include"DescriptorHeap/DescriptorHeap.hpp"
#include"Resource/VertexBufferResource.hpp"
#include"Resource/ConstantBufferResource.hpp"
#include"CommandList.hpp"
#include<vector>

#include<DirectXMath.h>

namespace DX12
{
	//とりあえず
	template<typename Vector3D>
	struct Material
	{
		Vector3D diffuseColor;
		double diffuseFactor;

		Vector3D ambientColor;
		double ambientFactor;

		Vector3D specularColor;
		double specularFactor;
	};

	//とりあえず　
	struct SceneData
	{
		DirectX::XMMATRIX view{};
		DirectX::XMMATRIX proj{};
	};
	

	class FBXModel
	{
		VertexBufferResource vertexBufferResource{};
		DescriptorHeap<DescriptorHeapType::CBV_SRV_UAV> descriptorHeap{};
		std::vector<ConstantBufferResource> materialConstantBufferResources{};
		ConstantBufferResource sceneDataConstantBufferResource{};

		std::vector<std::int32_t> materialRange{};

	public:
		template<typename Vector2D,typename Vector3D>
		void Initialize(Device*, FBXL::Model3D<Vector2D, Vector3D>&&);

		void Draw(CommandList*);

		void MapSceneData(SceneData&&);
	};

	//
	//以下、定義
	//

	template<typename Vector2D, typename Vector3D>
	inline void FBXModel::Initialize(Device* device, FBXL::Model3D<Vector2D, Vector3D>&& model)
	{
		vertexBufferResource.Initialize(device, sizeof(model.vertices[0]) * model.vertices.size(), sizeof(model.vertices[0]));
		vertexBufferResource.Map(std::move(model.vertices));

		//SceneDataとマテリアルの分
		descriptorHeap.Initialize(device, 1 + model.material.size());

		sceneDataConstantBufferResource.Initialize(device, sizeof(SceneData));

		materialConstantBufferResources.resize(model.material.size());
		for (std::size_t i = 0; i < model.material.size(); i++)
		{
			ConstantBufferResource tmp{};
			tmp.Initialize(device, sizeof(model.material[i]));
			FBXL::Material<Vector3D> hoge[] = { std::move(model.material[i]) };
			tmp.Map(hoge);

			materialConstantBufferResources[i] = std::move(tmp);
		}

		descriptorHeap.PushBackView(device, &sceneDataConstantBufferResource);
		for (auto& materialBuff : materialConstantBufferResources)
			descriptorHeap.PushBackView(device, &materialBuff);

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
			cl->Get()->SetGraphicsRootDescriptorTable(1, descriptorHeap.GetGPUHandle(i + 1));
			cl->Get()->DrawInstanced(materialRange[i], materialRange[i] / 3, vertexOffset, 0);

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
#pragma once
#include"include/DataStruct.hpp"
#include"DescriptorHeap/DescriptorHeap.hpp"
#include"Resource/VertexBufferResource.hpp"
#include"Resource/ConstantBufferResource.hpp"
#include"CommandList.hpp"
#include<vector>

namespace DX12
{
	//Ç∆ÇËÇ†Ç¶Ç∏
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



	class FBXModel
	{
		VertexBufferResource vertexBufferResource{};
		DescriptorHeap<DescriptorHeapType::CBV_SRV_UAV> descriptorHeap{};
		std::vector<ConstantBufferResource> constantBufferResources{};

		std::vector<std::int32_t> materialRange{};

	public:
		template<typename Vector2D,typename Vector3D>
		void Initialize(Device*, FBXL::Model3D<Vector2D, Vector3D>&&);

		void Draw(CommandList*);
	};

	//
	//à»â∫ÅAíËã`
	//

	template<typename Vector2D, typename Vector3D>
	inline void FBXModel::Initialize(Device* device, FBXL::Model3D<Vector2D, Vector3D>&& model)
	{
		vertexBufferResource.Initialize(device, sizeof(model.vertices), sizeof(typename decltype(model.vertices)::value_type));
		vertexBufferResource.Map(std::move(model.vertices));

		descriptorHeap.Initialize(device, model.material.size());

		constantBufferResources.resize(model.material.size());
		for (std::size_t i = 0; i < model.material.size(); i++)
		{
			ConstantBufferResource tmp{};
			tmp.Initialize(device, sizeof(model.material[i]));
			FBXL::Material<Vector3D> hoge[] = { std::move(model.material[i]) };
			tmp.Map(hoge);

			constantBufferResources[i] = std::move(tmp);
		}

		for (auto& materialBuff : constantBufferResources)
			descriptorHeap.PushBackView(device, &materialBuff);

		materialRange = std::move(model.materialRange);
	}


	void FBXModel::Draw(CommandList* cl)
	{
		unsigned int vertexOffset = 0;

		cl->Get()->IASetVertexBuffers(0, 1, &vertexBufferResource.GetView());

		cl->Get()->SetDescriptorHeaps(1, &descriptorHeap.Get());

		for (std::size_t i = 0; i < materialRange.size(); i++)
		{
			cl->Get()->SetGraphicsRootDescriptorTable(0, descriptorHeap.GetGPUHandle(i));
			//cl->Get()->DrawInstanced(materialRange[i], materialRange[i] / 3, vertexOffset, 3);

			vertexOffset += materialRange[i];
		}

	}
}
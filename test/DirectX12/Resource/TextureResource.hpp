#pragma once
#include"ResourceBase.hpp"
#include"../DescriptorHeap/DescriptorHeapViewType.hpp"
#include<DirectXTex.h>

namespace DX12
{
	class TextureResource :public ResourceBase
	{
	public:
		void Initialize(Device* device, CommandList* cl, const DirectX::TexMetadata* metaData, const DirectX::ScratchImage* scratchImage);
	};

	template<>
	struct ViewTypeTraits<TextureResource> {
		using ViewType = ViewType::Float4ShaderResource;
	};	
}
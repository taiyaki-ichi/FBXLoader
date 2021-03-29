#pragma once
#include"ResourceBase.hpp"
#include"../DescriptorHeap/DescriptorHeapViewType.hpp"

namespace DX12
{
	class DoubleBufferResource : public ResourceBase {};

	template<>
	struct ViewTypeTraits<DoubleBufferResource>
	{
		using ViewType = ViewType::Float4ShaderResource;
	};
}
#pragma once
#include"ResourceBase.hpp"
#include"../DescriptorHeap/DescriptorHeapViewType.hpp"

namespace DX12
{
	class DepthStencilBufferResource : public ResourceBase
	{
	public:
		void Initialize(Device*, std::size_t width, std::size_t height);
	};

	template<>
	struct ViewTypeTraits<DepthStencilBufferResource> {
		using ViewType = ViewType::DepthStencilBuffer;
	};


}
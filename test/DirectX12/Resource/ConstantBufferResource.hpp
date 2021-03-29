#pragma once
#include"UploadResource.hpp"
#include"../DescriptorHeap/DescriptorHeapViewType.hpp"

namespace DX12
{
	class ConstantBufferResource : public UploadResource
	{
	public:
		void Initialize(Device*, std::size_t size);
	};

	template<>
	struct ViewTypeTraits<ConstantBufferResource>
	{
		using ViewType = ViewType::ConstantBuffer;
	};
}
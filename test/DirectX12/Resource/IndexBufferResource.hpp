#pragma once
#include"UploadResource.hpp"

namespace DX12
{

	class IndexBufferResource : public UploadResource
	{
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	public:
		void Initialize(Device*, std::size_t size);

		const D3D12_INDEX_BUFFER_VIEW& GetView() const noexcept;
	};

}
#include"IndexBufferResource.hpp"

namespace DX12
{

	void IndexBufferResource::Initialize(Device* device, std::size_t size)
	{
		UploadResource::Initialize(device, size);

		indexBufferView.BufferLocation = Get()->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = size;
	}

	const D3D12_INDEX_BUFFER_VIEW& IndexBufferResource::GetView() const noexcept
	{
		return indexBufferView;
	}

}
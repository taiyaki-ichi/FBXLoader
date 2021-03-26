#include"VertexBufferResource.hpp"

namespace DX12
{

	void VertexBufferResource::Initialize(Device* device, std::size_t size, std::size_t stride)
	{
		UploadResource::Initialize(device, size);

		bufferView.BufferLocation = Get()->GetGPUVirtualAddress();
		bufferView.SizeInBytes = size;
		bufferView.StrideInBytes = stride;
	}

	const D3D12_VERTEX_BUFFER_VIEW& VertexBufferResource::GetView() const noexcept
	{
		return bufferView;
	}
}
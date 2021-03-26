#include"UploadResource.hpp"

namespace DX12
{
	void UploadResource::Initialize(Device* device, std::size_t size)
	{
		D3D12_HEAP_PROPERTIES heapprop{};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc{};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = size;
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ResourceBase::Initialize(device,
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr
		);
	}

	namespace {
		inline constexpr size_t AlignmentSize(size_t size, size_t alignment) {
			return size + alignment - size % alignment;
		}
	}

	void UploadResource::Map(uint8_t* data, std::size_t rowPitch, std::size_t height)
	{
		uint8_t* target = nullptr;
		if (FAILED(Get()->Map(0, nullptr, (void**)&target)))
			throw "UploadResource is failed\n";

		std::size_t targetRowPitch = AlignmentSize(rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		for (std::size_t i = 0; i < height; i++)
		{
			std::copy_n(data, rowPitch, target);
			data += rowPitch;
			target += targetRowPitch;
		}

		Get()->Unmap(0, nullptr);
	}
}
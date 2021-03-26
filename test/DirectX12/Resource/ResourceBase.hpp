#pragma once
#include<optional>
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{
	class Device;
	class CommandList;

	class ResourceBase
	{
		ID3D12Resource* resource = nullptr;

		D3D12_RESOURCE_STATES state{};

		std::optional<D3D12_CLEAR_VALUE> clearValue{};

	public:
		ResourceBase() = default;
		virtual ~ResourceBase();

		ResourceBase(const ResourceBase&) = delete;
		ResourceBase& operator=(const ResourceBase&) = delete;

		ResourceBase(ResourceBase&&) noexcept;
		ResourceBase& operator=(ResourceBase&&) noexcept;

		void Initialize(Device* device, const D3D12_HEAP_PROPERTIES* heapProp, D3D12_HEAP_FLAGS flag,
			const D3D12_RESOURCE_DESC* resoDesc, D3D12_RESOURCE_STATES s, const D3D12_CLEAR_VALUE* cv);

		//主にバックバッファとテクスチャのリソースを生成する際に使用
		void Initialize(ID3D12Resource* r);

		void Barrior(CommandList*, D3D12_RESOURCE_STATES);

		ID3D12Resource* Get() const noexcept;

		D3D12_CLEAR_VALUE* GetClearValue();
	};



}
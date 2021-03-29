#include"ResourceBase.hpp"
#include"../Device.hpp"
#include"../CommandList.hpp"

namespace DX12
{

	ResourceBase::~ResourceBase()
	{
		if (resource)
			resource->Release();
	}

	ResourceBase::ResourceBase(ResourceBase&& rhs) noexcept
	{
		resource = rhs.resource;
		state = rhs.state;
		clearValue = std::move(rhs.clearValue);

		rhs.resource = nullptr;
	}

	ResourceBase& ResourceBase::operator=(ResourceBase&& rhs) noexcept
	{
		resource = rhs.resource;
		state = rhs.state;
		clearValue = std::move(rhs.clearValue);

		rhs.resource = nullptr;

		return *this;
	}

	void ResourceBase::Initialize(Device* device, const D3D12_HEAP_PROPERTIES* heapProp, D3D12_HEAP_FLAGS flag, 
		const D3D12_RESOURCE_DESC* resoDesc, D3D12_RESOURCE_STATES s, const D3D12_CLEAR_VALUE* cv)
	{
		state = s;

		if (cv != nullptr)
			clearValue = *cv;

		if (FAILED(device->Get()->CreateCommittedResource(
			heapProp,
			flag,
			resoDesc,
			s,
			cv,
			IID_PPV_ARGS(&resource))))
		{
			throw  "resource initialize is failed\n";
		}
	}

	void ResourceBase::Initialize(ID3D12Resource* r)
	{
		if (resource == nullptr && r != nullptr)
			resource = r;
		else
			throw "";
	}

	void ResourceBase::Barrior(CommandList* cl, D3D12_RESOURCE_STATES rs)
	{
		if (state == rs)
			return;


		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = state;
		barrier.Transition.StateAfter = rs;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cl->Get()->ResourceBarrier(1, &barrier);

		state = rs;
	}

	ID3D12Resource* ResourceBase::Get() const noexcept
	{
		return resource;
	}

	D3D12_CLEAR_VALUE* ResourceBase::GetClearValue()
	{
		if (clearValue)
			return &clearValue.value();
		else
			return nullptr;
	}

}
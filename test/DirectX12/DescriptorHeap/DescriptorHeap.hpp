#pragma once
#include"CreateViewFunction.hpp"
#include"DescriptorHeapType.hpp"
#include<optional>
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


namespace DX12
{
	class Device;

	template<typename T>
	struct ViewTypeTraits;

	template<typename T>
	struct GetResourcePtrPolicy;



	template<typename DescriptorHeapType>
	class DescriptorHeap
	{
		ID3D12DescriptorHeap* descriptorHeap = nullptr;

		unsigned int size = 0;
		unsigned int offset = 0;
		unsigned int incrementSize = 0;

	public:
		DescriptorHeap() = default;
		~DescriptorHeap();

		DescriptorHeap(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&) = delete;

		DescriptorHeap(DescriptorHeap&&) noexcept;
		DescriptorHeap& operator=(DescriptorHeap&&) noexcept;

		void Initialize(Device* d, unsigned int size);

		//Viewを作り成功した場合はハンドルを返す
		//TはViewTypeTraitsとGetResourcePtrPolicyを特殊化している必要がある
		template<typename T>
		std::optional<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_CPU_DESCRIPTOR_HANDLE>>
			PushBackView(Device* device, T* resource);

		//offsetを0にする
		void Reset() noexcept;

		ID3D12DescriptorHeap*& Get() noexcept;

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(std::size_t index);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(std::size_t index);

	};


	//
	//以下、実装
	//

	template<typename DescriptorHeapType>
	inline DescriptorHeap<DescriptorHeapType>::~DescriptorHeap()
	{
		if (descriptorHeap)
			descriptorHeap->Release();
	}

	template<typename DescriptorHeapType>
	inline DescriptorHeap<DescriptorHeapType>::DescriptorHeap(DescriptorHeap&& rhs) noexcept
	{
		descriptorHeap = rhs.descriptorHeap;
		size = rhs.size;
		offset = rhs.offset;
		incrementSize = rhs.incrementSize;

		rhs.descriptorHeap = nullptr;
	}

	template<typename DescriptorHeapType>
	inline DescriptorHeap<DescriptorHeapType>& DescriptorHeap<DescriptorHeapType>::operator=(DescriptorHeap&& rhs) noexcept
	{
		descriptorHeap = rhs.descriptorHeap;
		size = rhs.size;
		offset = rhs.offset;
		incrementSize = rhs.incrementSize;

		rhs.descriptorHeap = nullptr;

		return *this;
	}

	template<typename DescriptorHeapType>
	inline void DescriptorHeap<DescriptorHeapType>::Initialize(Device* d, unsigned int size)
	{
		descriptorHeap = DescriptorHeapType::Initialize(d, size);
		if (!descriptorHeap) 
			throw "descriptor heap init is failed\n";

		incrementSize = DescriptorHeapType::GetIncrementSize(d);
		size = size;
	}

	template<typename DescriptorHeapType>
	template<typename T>
	inline std::optional<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_CPU_DESCRIPTOR_HANDLE>> 
		DescriptorHeap<DescriptorHeapType>::PushBackView(Device* device, T* resource)
	{
		//空いてるスペースがない場合
		if (offset >= size)
			return std::nullopt;

		//cpuハンドルの取得
		auto cpuHandle = GetCPUHandle(offset);

		//Viewを作製したいリソースのポインタを取得
		auto resourcePtr = GetResourcePtrPolicy<T>::GetResourcePtr(resource);

		//viewの生成
		if (!CreateView<DescriptorHeapType, typename ViewTypeTraits<T>::ViewType>(device, resourcePtr, cpuHandle))
			return std::nullopt;

		//戻り値用にgpuハンドルの取得
		auto gpuHandle = GetGPUHandle(offset);

		//オフセットの更新
		offset++;

		return std::make_pair(gpuHandle, cpuHandle);
	}

	template<typename DescriptorHeapType>
	inline void DescriptorHeap<DescriptorHeapType>::Reset() noexcept
	{
		offset = 0;
	}

	template<typename DescriptorHeapType>
	inline ID3D12DescriptorHeap*& DescriptorHeap<DescriptorHeapType>::Get() noexcept
	{
		return descriptorHeap;
	}

	template<typename DescriptorHeapType>
	inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap<DescriptorHeapType>::GetGPUHandle(std::size_t index)
	{
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += static_cast<UINT64>(incrementSize) * static_cast<UINT64>(index);
		return gpuHandle;
	}

	template<typename DescriptorHeapType>
	inline D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap<DescriptorHeapType>::GetCPUHandle(std::size_t index)
	{
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += static_cast<size_t>(incrementSize) * static_cast<size_t>(index);
		return cpuHandle;
	}
}
#pragma once
#include"DescriptorHeap/DescriptorHeap.hpp"
#include"Resource/DoubleBufferResource.hpp"
#include<array>
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{
	class DoubleBuffer
	{
		IDXGIFactory5* factory = nullptr;
		IDXGISwapChain4* swapChain = nullptr;
		DescriptorHeap<DescriptorHeapType::RTV> descriptorHeap{};

		std::array<DoubleBufferResource, 2> doubleBufferResources{};

	public:
		DoubleBuffer() = default;
		~DoubleBuffer();

		DoubleBuffer(const DoubleBuffer&) = delete;
		DoubleBuffer& operator=(const DoubleBuffer&) = delete;

		DoubleBuffer(DoubleBuffer&&) noexcept;
		DoubleBuffer& operator=(DoubleBuffer&&) noexcept;

		void Initialize(Device*, IDXGIFactory5*, IDXGISwapChain4*);

		D3D12_CPU_DESCRIPTOR_HANDLE GetBackbufferCpuHandle();

		void BarriorToBackbuffer(CommandList*, D3D12_RESOURCE_STATES);

		void ClearBackBuffer(CommandList*);

		void Flip();
	};
}
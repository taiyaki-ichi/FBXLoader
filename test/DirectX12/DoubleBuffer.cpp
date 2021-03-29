#include"DoubleBuffer.hpp"
#include"CommandList.hpp"

namespace DX12
{

	DoubleBuffer::~DoubleBuffer()
	{
		if (factory)
			factory->Release();
		if (swapChain)
			swapChain->Release();
	}

	DoubleBuffer::DoubleBuffer(DoubleBuffer&& rhs) noexcept
	{
		factory = rhs.factory;
		swapChain = rhs.swapChain;
		descriptorHeap = std::move(rhs.descriptorHeap);
		doubleBufferResources = std::move(rhs.doubleBufferResources);

		rhs.factory = nullptr;
		rhs.swapChain = nullptr;
	}

	DoubleBuffer& DoubleBuffer::operator=(DoubleBuffer&& rhs) noexcept
	{
		factory = rhs.factory;
		swapChain = rhs.swapChain;
		descriptorHeap = std::move(rhs.descriptorHeap);
		doubleBufferResources = std::move(rhs.doubleBufferResources);

		rhs.factory = nullptr;
		rhs.swapChain = nullptr;

		return *this;
	}

	void DoubleBuffer::Initialize(Device* device, IDXGIFactory5* f, IDXGISwapChain4* sc)
	{
		if (f == nullptr || sc == nullptr)
			throw "";

		factory = f;
		swapChain = sc;

		descriptorHeap.Initialize(device, 2);

		for (size_t i = 0; i < doubleBufferResources.size(); i++) {

			ID3D12Resource* resourcePtr;

			//失敗したとき
			if (FAILED(swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&resourcePtr)))) 
				throw "GetBuffer is failed\n";

			doubleBufferResources[i].Initialize(resourcePtr);
			auto result = descriptorHeap.PushBackView(device, &doubleBufferResources[i]);
		}

	}

	D3D12_CPU_DESCRIPTOR_HANDLE DoubleBuffer::GetBackbufferCpuHandle()
	{
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();
		return descriptorHeap.GetCPUHandle(bbIdx);
	}

	void DoubleBuffer::BarriorToBackbuffer(CommandList* cl, D3D12_RESOURCE_STATES state)
	{
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();
		doubleBufferResources[bbIdx].Barrior(cl, state);
	}

	void DoubleBuffer::ClearBackBuffer(CommandList* cl)
	{
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();
		auto rtvH = descriptorHeap.GetCPUHandle(bbIdx);
		//バックバッファのクリア
		float clearColor[] = { 0.5f,0.5f,0.5f,1.0f };
		cl->Get()->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	}

	void DoubleBuffer::Flip()
	{
		swapChain->Present(1, 0);
	}

}
#include"CommandList.hpp"
#include"Device.hpp"

namespace DX12
{

	CommandList::~CommandList()
	{
		if (allocator)
			allocator->Release();
		if (queue)
			queue->Release();
		if (list)
			list->Release();
		if (fence)
			fence->Release();
	}

	CommandList::CommandList(CommandList&& rhs) noexcept
	{
		allocator = rhs.allocator;
		queue = rhs.queue;
		list = rhs.list;
		fence = rhs.fence;
		fenceValue = rhs.fenceValue;
		rhs.allocator = nullptr;
		rhs.queue = nullptr; 
		rhs.list = nullptr;
		rhs.fence = nullptr;
	}

	CommandList& CommandList::operator=(CommandList&& rhs) noexcept
	{
		allocator = rhs.allocator;
		queue = rhs.queue;
		list = rhs.list;
		fence = rhs.fence;
		fenceValue = rhs.fenceValue;
		rhs.allocator = nullptr;
		rhs.queue = nullptr;
		rhs.list = nullptr;
		rhs.fence = nullptr;
		return *this;
	}

	void CommandList::Initialize(Device* device)
	{
		if (FAILED(device->Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator))))
			throw "CreateCommandAllocator is failed\n";

		if (FAILED(device->Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr, IID_PPV_ARGS(&list))))
			throw "CreateCommandList is failed\n";

		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//タイムアウトナシ
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	//プライオリティ特に指定なし
		cmdQueueDesc.Type = list->GetType();			//ここはコマンドリストと合わせる
		if (FAILED(device->Get()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&queue))))
			throw "CreateCommandQueue is failed\n";

		if (FAILED(device->Get()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
			throw "CreateFence is failed\n";

	}

	ID3D12GraphicsCommandList* CommandList::Get()
	{
		return list;
	}

	std::pair<IDXGIFactory5*, IDXGISwapChain4*> CommandList::CreateFactryAndSwapChain(HWND hwnd)
	{
		IDXGIFactory5* factory = nullptr;
		IDXGISwapChain4* swapChain = nullptr;

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) 
			throw "CreateDXGIFactory1 is failed\n";

		RECT windowRect{};
		GetWindowRect(hwnd, &windowRect);

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
		swapchainDesc.Width = windowRect.right - windowRect.left;
		swapchainDesc.Height = windowRect.bottom - windowRect.top;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = false;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (FAILED(factory->CreateSwapChainForHwnd(queue, hwnd, &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapChain)))
			throw "CreateSwapChainForHwnd is failed\n";

		return std::make_pair(factory, swapChain);
	}

	void CommandList::Execute()
	{
		queue->ExecuteCommandLists(1, (ID3D12CommandList**)(&list));
		queue->Signal(fence, ++fenceValue);

		//コマンドキューが終了していないことの確認
		if (fence->GetCompletedValue() < fenceValue)
		{
			HANDLE fenceEvent = nullptr;
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			// イベントが発火するまで待つ
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}

	void CommandList::Clear()
	{
		allocator->Reset();
		list->Reset(allocator, nullptr);
	}

	void CommandList::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle)
	{
		list->OMSetRenderTargets(1, &renderTargetHandle, false, nullptr);
	}

	void CommandList::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle)
	{
		list->OMSetRenderTargets(1, &renderTargetHandle, false, &depthStencilHandle);
	}

	void CommandList::SetRenderTarget(unsigned int renderTagetHandleNum, D3D12_CPU_DESCRIPTOR_HANDLE* renderTarget)
	{
		list->OMSetRenderTargets(renderTagetHandleNum, renderTarget, false, nullptr);
	}

	void CommandList::SetRenderTarget(unsigned int renderTagetHandleNum, D3D12_CPU_DESCRIPTOR_HANDLE* renderTarget, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle)
	{
		list->OMSetRenderTargets(renderTagetHandleNum, renderTarget, false, &depthStencilHandle);
	}

	void CommandList::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		list->RSSetViewports(1, &viewport);
	}

	void CommandList::SetViewport(unsigned int num, D3D12_VIEWPORT* viewportPtr)
	{
		list->RSSetViewports(num, viewportPtr);
	}

	void CommandList::SetScissorRect(const D3D12_RECT& rect)
	{
		list->RSSetScissorRects(1, &rect);
	}

	void CommandList::SetScissorRect(unsigned int num, D3D12_RECT* rectPtr)
	{
		list->RSSetScissorRects(num, rectPtr);
	}

	void CommandList::Close()
	{
		list->Close();
	}

}
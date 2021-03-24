#pragma once
#include<utility>
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{
	class Device;

	class CommandList
	{
		ID3D12CommandAllocator* allocator = nullptr;
		ID3D12CommandQueue* queue = nullptr;

		ID3D12GraphicsCommandList* list = nullptr;

		ID3D12Fence* fence = nullptr;
		HANDLE fenceEvent = nullptr;
		UINT64 fenceValue = 1;

	public:
		CommandList() = default;
		~CommandList();

		CommandList(const  CommandList&) = delete;
		CommandList& operator=(const CommandList&) = delete;

		CommandList(CommandList&&) noexcept;
		CommandList& operator=(CommandList&&) noexcept;

		void Initialize(Device*);

		ID3D12GraphicsCommandList* Get();

		std::pair<IDXGIFactory5*, IDXGISwapChain4*> CreateFactryAndSwapChain(HWND);

		//コマンドの実行
		void Execute();

		//コマンドのクリア
		//引数は初期設定したいパイプラインステート
		void Clear();

		//レンダーターゲットの設定
		//どのターゲットのViewもおなじディスクリプタヒープ連続して生成されていないとみなしている
		void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle);
		void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle);
		void SetRenderTarget(unsigned int renderTagetHandleNum, D3D12_CPU_DESCRIPTOR_HANDLE* renderTarget);
		void SetRenderTarget(unsigned int renderTagetHandleNum, D3D12_CPU_DESCRIPTOR_HANDLE* renderTarget, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle);

		//ビューポートの設定
		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetViewport(unsigned int num, D3D12_VIEWPORT* viewportPtr);

		//シザー矩形の設定
		void SetScissorRect(const D3D12_RECT& rect);
		void SetScissorRect(unsigned int num, D3D12_RECT* rectPtr);

		//Closeを呼び出す
		void Close();
	};


}
#include"include/Loader.hpp"
#include"Window.hpp"
#include"DirectX12/Device.hpp"
#include"DirectX12/CommandList.hpp"
#include"DirectX12/Shader.hpp"
#include"DirectX12/RootSignature.hpp"
#include"DirectX12/PipelineState.hpp"
#include"DirectX12/FBXModel.hpp"
#include"DirectX12/DoubleBuffer.hpp"
#include"DirectX12/Resource/DepthStencilBufferResource.hpp"

#include<DirectXMath.h>

#include<iostream>
#include<fstream>


struct Vector3
{
	float x{};
	float y{};
	float z{};

};

struct Vector2
{
	float x{};
	float y{};
};


int main()
{
	using namespace DirectX;

	constexpr auto windowWidth = 800;
	constexpr auto windowHeight = 600;
	
	auto hwnd = Window::CreateSimpleWindow(L"test", windowWidth, windowHeight);

	DX12::Device device{};
	device.Initialize();

	DX12::CommandList commandList{};
	commandList.Initialize(&device);

	auto [factory, swapChain] = commandList.CreateFactryAndSwapChain(hwnd);

	DX12::DoubleBuffer doubleBuffer{};
	doubleBuffer.Initialize(&device, factory, swapChain);

	DX12::Shader vertexShader{};
	vertexShader.Intialize(L"ShaderFile/VertexShader.hlsl", "main", "vs_5_0");

	DX12::Shader pixcelShader{};
	pixcelShader.Intialize(L"ShaderFile/PixelShader.hlsl", "main", "ps_5_0");


	DX12::RootSignature rootSignature{};
	rootSignature.Initialize<
		DX12::DescrriptorTableArray<
		DX12::DescriptorTable<DX12::DescriptorRange::CBV>,//シーンデータ
		DX12::DescriptorTable<DX12::DescriptorRange::CBV, DX12::DescriptorRange::SRV>//マテリアルとテクスチャ
	>, DX12::StaticSamplers<DX12::StaticSampler::Normal>
	>(&device);

	DX12::PipelineState pipelineState{};
	pipelineState.Initialize(&device, std::move(rootSignature), std::move(vertexShader), std::move(pixcelShader));

	auto model2 = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/fbx_loader_test_001.fbx");

	DX12::FBXModel2 fbxModel2{};
	fbxModel2.Initialize(&device, &commandList, std::move(model2.value()));

	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(windowWidth);//出力先の幅(ピクセル数)
	viewport.Height = static_cast<float>(windowHeight);//出力先の高さ(ピクセル数)
	viewport.TopLeftX = 0;//出力先の左上座標X
	viewport.TopLeftY = 0;//出力先の左上座標Y
	viewport.MaxDepth = 1.0f;//深度最大値
	viewport.MinDepth = 0.0f;//深度最小値

	D3D12_RECT scissorrect{};
	scissorrect.top = 0;//切り抜き上座標
	scissorrect.left = 0;//切り抜き左座標
	scissorrect.right = scissorrect.left + windowWidth;//切り抜き右座標
	scissorrect.bottom = scissorrect.top + windowHeight;//切り抜き下座標

	DirectX::XMFLOAT3 eye{ 0,7,-6 };
	DirectX::XMFLOAT3 target{ 0,0,0 };
	DirectX::XMFLOAT3 up{ 0,1,0 };
	auto view = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
	auto proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2,
		static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
		1.f,
		1000.f
	);


	DX12::DepthStencilBufferResource depthStencilBufferResource{};
	depthStencilBufferResource.Initialize(&device, windowWidth, windowHeight);

	DX12::DescriptorHeap<DX12::DescriptorHeapType::DSV> depthStencilDescriptorHeap{};
	depthStencilDescriptorHeap.Initialize(&device, 1);

	depthStencilDescriptorHeap.PushBackView(&device, &depthStencilBufferResource);

	auto cnt = 0;
	while (Window::UpdateWindow())
	{
		
		DirectX::XMFLOAT3 eye{ 400 * std::sin(cnt / 100.f),300,400 * std::cos(cnt / 100.f) };
		view = DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
		cnt++;

		fbxModel2.MapSceneData({ view,proj ,eye });

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetRenderTarget(doubleBuffer.GetBackbufferCpuHandle(), depthStencilDescriptorHeap.GetCPUHandle());

		doubleBuffer.ClearBackBuffer(&commandList);

		commandList.Get()->ClearDepthStencilView(depthStencilDescriptorHeap.GetCPUHandle(),
			D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

		commandList.SetViewport(viewport);
		commandList.SetScissorRect(scissorrect);

		pipelineState.PreparationForDrawing(&commandList);

		fbxModel2.Draw(&commandList);

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_PRESENT);

		commandList.Close();
		commandList.Execute();
		commandList.Clear();

		doubleBuffer.Flip();
	}
	
	


	return 0;
}



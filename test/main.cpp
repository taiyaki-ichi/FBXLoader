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
	/*
	auto hoge = FBXL::GetIndecedData(std::vector<double>{1.0,1.0,2.0,3.0,3.0});
	hoge = FBXL::UpdateIndeces(std::move(hoge), std::vector<std::int32_t>{0, 2, 2, 1, 3, 4, 0, 1});

	auto piyo = FBXL::DoubleTuple<2>{};

	auto a = FBXL::GetDoubleTuples<2>(std::vector<double>{ 1,2,3,4,5,6,7,8 });
	auto b = FBXL::GetDoubleTuples<3>(std::vector<double>{ 1,2,3,4,5,6,7,8,9 });
	*/

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
		DX12::DescriptorTable<DX12::DescriptorRange::CBV, DX12::DescriptorRange::CBV>,//シーンデータとtransform
		DX12::DescriptorTable<DX12::DescriptorRange::CBV, DX12::DescriptorRange::SRV>//マテリアルとテクスチャ
	>, DX12::StaticSamplers<DX12::StaticSampler::Normal>
	>(&device);

	DX12::PipelineState pipelineState{};
	pipelineState.Initialize(&device, std::move(rootSignature), std::move(vertexShader), std::move(pixcelShader));

	auto model = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/fbx_loader_test_001.fbx");
	//auto model = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/uv_cube.fbx");
	//auto model2 = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/smooth_uv_cube.fbx");
	auto model2 = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/cube.fbx");

	DX12::FBXModel2 fbxModel{};
	fbxModel.Initialize(&device, &commandList, std::move(model.value()));

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

	DirectX::XMMATRIX modelWorldMatrix = XMMatrixTranslation(150.f,0.f,0.f);
	DirectX::XMMATRIX modelWorldMatrix2 = XMMatrixTranslation(-150.f, 0.f, 0.f);

	auto cnt = 0;
	while (Window::UpdateWindow())
	{
		
		DirectX::XMFLOAT3 eye{ 400 * std::sin(cnt / 100.f),300,400 * std::cos(cnt / 100.f) };
		view = DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
		cnt++;

		fbxModel.MapSceneData({ view,proj ,eye });
		fbxModel.Update(modelWorldMatrix);

		fbxModel2.MapSceneData({ view,proj ,eye });
		fbxModel2.Update(modelWorldMatrix2);

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetRenderTarget(doubleBuffer.GetBackbufferCpuHandle(), depthStencilDescriptorHeap.GetCPUHandle());

		doubleBuffer.ClearBackBuffer(&commandList);

		commandList.Get()->ClearDepthStencilView(depthStencilDescriptorHeap.GetCPUHandle(),
			D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

		commandList.SetViewport(viewport);
		commandList.SetScissorRect(scissorrect);

		pipelineState.PreparationForDrawing(&commandList);

		fbxModel.Draw(&commandList);
		fbxModel2.Draw(&commandList);

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_PRESENT);

		commandList.Close();
		commandList.Execute();
		commandList.Clear();

		doubleBuffer.Flip();
	}
	
	


	return 0;
}



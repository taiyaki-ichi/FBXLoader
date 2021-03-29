#include"include/Loader.hpp"
#include"Window.hpp"
#include"DirectX12/Device.hpp"
#include"DirectX12/CommandList.hpp"
#include"DirectX12/Shader.hpp"
#include"DirectX12/RootSignature.hpp"
#include"DirectX12/PipelineState.hpp"
#include"DirectX12/FBXModel.hpp"
#include"DirectX12/DoubleBuffer.hpp"

#include<DirectXMath.h>

#include<iostream>
#include<fstream>


struct Vector3
{
	double x{};
	double y{};
	double z{};

};

struct Vector2
{
	double x{};
	double y{};


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
		DX12::DescriptorTable<DX12::DescriptorRange::CBV>
		>, DX12::StaticSamplers<DX12::StaticSampler::Normal>
	>(&device);

	DX12::PipelineState pipelineState{};
	pipelineState.Initialize(&device, std::move(rootSignature), std::move(vertexShader), std::move(pixcelShader));

	auto model = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/test_material.fbx");

	DX12::FBXModel fbxModel{};
	fbxModel.Initialize(&device, std::move(model.value()));

	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(windowWidth);//�o�͐�̕�(�s�N�Z����)
	viewport.Height = static_cast<float>(windowHeight);//�o�͐�̍���(�s�N�Z����)
	viewport.TopLeftX = 0;//�o�͐�̍�����WX
	viewport.TopLeftY = 0;//�o�͐�̍�����WY
	viewport.MaxDepth = 1.0f;//�[�x�ő�l
	viewport.MinDepth = 0.0f;//�[�x�ŏ��l

	D3D12_RECT scissorrect{};
	scissorrect.top = 0;//�؂蔲������W
	scissorrect.left = 0;//�؂蔲�������W
	scissorrect.right = scissorrect.left + windowWidth;//�؂蔲���E���W
	scissorrect.bottom = scissorrect.top + windowHeight;//�؂蔲�������W




	while (Window::UpdateWindow())
	{

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetRenderTarget(doubleBuffer.GetBackbufferCpuHandle());

		doubleBuffer.ClearBackBuffer(&commandList);

		commandList.SetViewport(viewport);
		commandList.SetScissorRect(scissorrect);

		pipelineState.PreparationForDrawing(&commandList);
		fbxModel.Draw(&commandList);

		doubleBuffer.BarriorToBackbuffer(&commandList, D3D12_RESOURCE_STATE_PRESENT);

		commandList.Close();
		commandList.Execute();
		commandList.Clear();

		doubleBuffer.Flip();
	}
	
	


	return 0;
}



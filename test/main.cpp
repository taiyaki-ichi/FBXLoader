#include"include/Loader.hpp"
#include"Window.hpp"
#include"DirectX12/Device.hpp"
#include"DirectX12/CommandList.hpp"
#include"DirectX12/DescriptorHeap/DescriptorHeap.hpp"
#include"DirectX12/Shader.hpp"
#include"DirectX12/Resource/VertexBufferResource.hpp"

#include"DirectX12/Utility.hpp"

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

	/*
	auto hwnd = Window::CreateSimpleWindow(L"test", 800, 600);

	DX12::Device device{};
	device.Initialize();

	DX12::CommandList commandList{};
	commandList.Initialize(&device);

	DX12::DescriptorHeap<DX12::DescriptorHeapType::CBV_SRV_UAV> descriptorHeap{};
	descriptorHeap.Initialize(&device, 10);

	DX12::Shader vertexShader{};
	vertexShader.Intialize(L"ShaderFile/VertexShader.hlsl", "main", "vs_5_0");

	DX12::Shader pixcelShader{};
	pixcelShader.Intialize(L"ShaderFile/PixelShader.hlsl", "main", "ps_5_0");

	Vector3 vertices[] = {
		{-1.f,-1.f,0.f},
		{-1.f,1.f,0.f},
		{1.f,-1.f,0.f},
	};

	DX12::VertexBufferResource vertexBuffer{};
	vertexBuffer.Initialize(&device, sizeof(vertices), sizeof(vertices) / sizeof(vertices[0]));
	vertexBuffer.Map(std::move(vertices));

	while (Window::UpdateWindow());
	*/


	constexpr auto hoge = DX12::GetStaticSamplerArray<
		DX12::StaticSamplers<DX12::StaticSampler::Normal, DX12::StaticSampler::Shadow, DX12::StaticSampler::Toon>
	>();

	static_assert(hoge[0].RegisterSpace == 0);
	static_assert(hoge[1].RegisterSpace == 1);
	static_assert(hoge[2].RegisterSpace == 2);





	return 0;
}



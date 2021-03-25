#include"include/Loader.hpp"
#include"Window.hpp"
#include"DirectX12/Device.hpp"
#include"DirectX12/CommandList.hpp"
#include"DirectX12/DescriptorHeap/DescriptorHeap.hpp"

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
	auto hwnd = Window::CreateSimpleWindow(L"test", 800, 600);

	DX12::Device device{};
	device.Initialize();

	DX12::CommandList commandList{};
	commandList.Initialize(&device);

	DX12::DescriptorHeap<DX12::DescriptorHeapType::CBV_SRV_UAV> descriptorHeap{};
	descriptorHeap.Initialize(&device, 10);

	while (Window::UpdateWindow());

	return 0;
}



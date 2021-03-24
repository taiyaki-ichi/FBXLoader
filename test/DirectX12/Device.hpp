#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{

	class Device
	{
		ID3D12Device* device = nullptr;

		IDXGIFactory5* factory = nullptr;
		IDXGIAdapter1* adaptor = nullptr;

	public:
		Device() = default;
		~Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		Device(Device&&) noexcept;
		Device& operator=(Device&&) noexcept;

		void Initialize();

		ID3D12Device* Get() const noexcept;
	};


}
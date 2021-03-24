#include"Device.hpp"

#include<array>


namespace DX12
{

	Device::~Device()
	{
		if (device)
			device->Release();
		if (factory)
			factory->Release();
		if (adaptor)
			adaptor->Release();
	}

	void Device::Initialize()
	{
		ID3D12Debug* debugLayer = nullptr;
		if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
			throw "D3D12GetDebugInterface is failed\n";
		}
		else {
			debugLayer->EnableDebugLayer();
			debugLayer->Release();
		}

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) 
			throw "CreateDXGIFactory1 is failed\n";


		UINT adapterIndex = 0;
		DXGI_ADAPTER_DESC1 desc{};
		while (true) {
			factory->EnumAdapters1(adapterIndex, &adaptor);
			adaptor->GetDesc1(&desc);

			//適切なアダプタが見つかった場合
			if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
				break;

			adapterIndex++;
			//見つからなかった場合
			if (adapterIndex == DXGI_ERROR_NOT_FOUND) 
				throw "adaptor is not found\n";
		}

		std::array levels{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		for (auto l : levels) {
			//適切に生成できた場合
			if (SUCCEEDED(D3D12CreateDevice(adaptor, l, IID_PPV_ARGS(&device))))
				break;
			if (l == levels.back())
				throw "D3D12CreateDevice is failed\n";
		}
	}

	ID3D12Device* Device::Get() const noexcept
	{
		return device;
	}

}
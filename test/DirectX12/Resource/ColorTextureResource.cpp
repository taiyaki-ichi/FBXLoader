#include"ColorTextureResource.hpp"

namespace DX12
{
	void ColorTextureResourceBase::Initialize(Device* device, unsigned int width, unsigned int height)
	{
		D3D12_HEAP_PROPERTIES texHeapProp{};
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//特殊な設定なのでdefaultでもuploadでもなく
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//ライトバックで
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//転送がL0つまりCPU側から直で
		texHeapProp.CreationNodeMask = 0;//単一アダプタのため0
		texHeapProp.VisibleNodeMask = 0;//単一アダプタのため0

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.Width = width;//幅
		resDesc.Height = height;//高さ
		resDesc.DepthOrArraySize = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;//
		resDesc.MipLevels = 1;//
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//レイアウトについては決定しない
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//とくにフラグなし

		ResourceBase::Initialize(
			device,
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,//特に指定なし
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr
		);
	}

	void GrayGradationTextureResource::Initialize(Device* device)
	{
		ColorTextureResourceBase::Initialize(device, 4, 256);

		//上が白くて下が黒いテクスチャデータを作成
		std::vector<unsigned int> data(4 * 256);
		auto it = data.begin();
		unsigned int c = 0xff;
		for (; it != data.end(); it += 4) {
			auto col = (0xff << 24) | RGB(c, c, c);
			std::fill(it, it + 4, col);
			--c;
		}

		if (FAILED(Get()->WriteToSubresource(0, nullptr, data.data(),
			4 * sizeof(unsigned int), sizeof(unsigned int) * static_cast<UINT>(data.size()))))
			throw "";
	}
}
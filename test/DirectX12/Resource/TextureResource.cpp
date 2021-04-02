#include"TextureResource.hpp"
#include"UploadResource.hpp"
#include"../Utility.hpp"
#include"../Device.hpp"
#include"../CommandList.hpp"

namespace DX12
{
	void TextureResource::Initialize(Device* device, CommandList* cl, const DirectX::TexMetadata* metaData, const DirectX::ScratchImage* scratchImage)
	{
		//画像のポインタ
		auto image = scratchImage->GetImage(0, 0, 0);

		//中間のバッファ
		UploadResource src{};
		src.Initialize(device, static_cast<UINT64>(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)) * image->height);

		//データをマップ
		uint8_t* mapforImage = nullptr;
		if (FAILED(src.Get()->Map(0, nullptr, (void**)&mapforImage))) {
			throw "";
		}

		uint8_t* srcAddress = image->pixels;
		size_t rowPitch = AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		for (size_t y = 0; y < image->height; ++y) {
			std::copy_n(srcAddress,
				image->rowPitch,//rowPitch,
				mapforImage);//コピー
			//1行ごとの辻褄を合わせてやる
			srcAddress += image->rowPitch;
			mapforImage += rowPitch;
		}
		src.Get()->Unmap(0, nullptr);


		D3D12_RESOURCE_DESC resdesc{};
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.SampleDesc.Count = 1;
		resdesc.SampleDesc.Quality = 0;
		resdesc.Format = metaData->format;
		resdesc.Width = static_cast<UINT>(metaData->width);//幅
		resdesc.Height = static_cast<UINT>(metaData->height);//高さ
		resdesc.DepthOrArraySize = static_cast<UINT16>(metaData->arraySize);//2Dで配列でもないので１
		resdesc.MipLevels = static_cast<UINT16>(metaData->mipLevels);//ミップマップしないのでミップ数は１つ
		resdesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData->dimension);//2Dテクスチャ用
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		D3D12_HEAP_PROPERTIES heapprop{};
		heapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
		//こっちもUNKNOWN
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		//単一アダプターのためそれぞれ0
		heapprop.CreationNodeMask = 0;
		heapprop.VisibleNodeMask = 0;

		ID3D12Resource* dstPtr = nullptr;

		if (FAILED(device->Get()->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&dstPtr)))) {
			throw "";
		}

		//
		//コピーのための構造体の設定
		//

		D3D12_TEXTURE_COPY_LOCATION srcLocation{};
		D3D12_TEXTURE_COPY_LOCATION dstLocation{};

		srcLocation.pResource = src.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		{
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
			UINT nrow;
			UINT64 rowsize, size;
			auto desc = dstPtr->GetDesc();
			device->Get()->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &nrow, &rowsize, &size);
			srcLocation.PlacedFootprint = footprint;
		}
		srcLocation.PlacedFootprint.Offset = 0;
		srcLocation.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData->width);
		srcLocation.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData->height);
		srcLocation.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData->depth);
		srcLocation.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
		srcLocation.PlacedFootprint.Footprint.Format = image->format;

		dstLocation.pResource = dstPtr;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = 0;

		//
		//戻り値のリソースにコピーするコマンドを実行
		//
		cl->Get()->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
		cl->Get()->Close();
		cl->Execute();
		cl->Clear();


		ResourceBase::Initialize(dstPtr);
	}

}
#include"TextureResource.hpp"
#include"UploadResource.hpp"
#include"../Utility.hpp"
#include"../Device.hpp"
#include"../CommandList.hpp"

namespace DX12
{
	void TextureResource::Initialize(Device* device, CommandList* cl, const DirectX::TexMetadata* metaData, const DirectX::ScratchImage* scratchImage)
	{
		//�摜�̃|�C���^
		auto image = scratchImage->GetImage(0, 0, 0);

		//���Ԃ̃o�b�t�@
		UploadResource src{};
		src.Initialize(device, static_cast<UINT64>(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)) * image->height);

		//�f�[�^���}�b�v
		uint8_t* mapforImage = nullptr;
		if (FAILED(src.Get()->Map(0, nullptr, (void**)&mapforImage))) {
			throw "";
		}

		uint8_t* srcAddress = image->pixels;
		size_t rowPitch = AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		for (size_t y = 0; y < image->height; ++y) {
			std::copy_n(srcAddress,
				image->rowPitch,//rowPitch,
				mapforImage);//�R�s�[
			//1�s���Ƃ̒�������킹�Ă��
			srcAddress += image->rowPitch;
			mapforImage += rowPitch;
		}
		src.Get()->Unmap(0, nullptr);


		D3D12_RESOURCE_DESC resdesc{};
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.SampleDesc.Count = 1;
		resdesc.SampleDesc.Quality = 0;
		resdesc.Format = metaData->format;
		resdesc.Width = static_cast<UINT>(metaData->width);//��
		resdesc.Height = static_cast<UINT>(metaData->height);//����
		resdesc.DepthOrArraySize = static_cast<UINT16>(metaData->arraySize);//2D�Ŕz��ł��Ȃ��̂łP
		resdesc.MipLevels = static_cast<UINT16>(metaData->mipLevels);//�~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��
		resdesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData->dimension);//2D�e�N�X�`���p
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		D3D12_HEAP_PROPERTIES heapprop{};
		heapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
		//��������UNKNOWN
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		//�P��A�_�v�^�[�̂��߂��ꂼ��0
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
		//�R�s�[�̂��߂̍\���̂̐ݒ�
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
		//�߂�l�̃��\�[�X�ɃR�s�[����R�}���h�����s
		//
		cl->Get()->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
		cl->Get()->Close();
		cl->Execute();
		cl->Clear();


		ResourceBase::Initialize(dstPtr);
	}

}
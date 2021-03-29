#include"ConstantBufferResource.hpp"

namespace DX12
{
	void ConstantBufferResource::Initialize(Device* device, std::size_t size)
	{
		//�T�C�Y��16�̔{�����Ⴀ�Ȃ��Ƃ����Ȃ��̂ŃA���C�����g
		size = (size + 0xff) & ~0xff;

		return UploadResource::Initialize(device, size);
	}

}
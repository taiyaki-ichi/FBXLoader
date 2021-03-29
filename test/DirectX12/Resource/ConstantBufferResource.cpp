#include"ConstantBufferResource.hpp"

namespace DX12
{
	void ConstantBufferResource::Initialize(Device* device, std::size_t size)
	{
		//サイズは16の倍数じゃあないといけないのでアライメント
		size = (size + 0xff) & ~0xff;

		return UploadResource::Initialize(device, size);
	}

}
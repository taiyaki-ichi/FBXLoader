#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


namespace DX12
{
	//DescriptorHeap��View����鎞�Ɏg�p
	namespace ViewType {

		//�萔�o�b�t�@
		struct ConstantBuffer {};

		//�[�x�X�e���V��
		struct DepthStencilBuffer {};

		//RGBA�̃e�N�X�`���̂悤�ȃf�[�^
		struct Float4ShaderResource {};

		//R�݂̂̃e�N�X�`���̂悤�ȃf�[�^
		struct FloatShaderResource {};
	}


	//
	//�ȉ��ADescriptorHeap::PushBackView���g���^�͓��ꉻ���Ă����K�v������
	//

	//namespace ViewType �̌^��ViewType�Ƃ���using����
	template<typename T>
	struct ViewTypeTraits 
	{
		using ViewType;
	};


	template<typename T>
	struct GetResourcePtrPolicy
	{
		static ID3D12Resource* GetResourcePtr(T* t) {
			return t->Get();
		}
	};

}
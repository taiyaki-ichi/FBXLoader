#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


namespace DX12
{
	//DescriptorHeapにViewを作る時に使用
	namespace ViewType {

		//定数バッファ
		struct ConstantBuffer {};

		//深度ステンシル
		struct DepthStencilBuffer {};

		//RGBAのテクスチャのようなデータ
		struct Float4ShaderResource {};

		//Rのみのテクスチャのようなデータ
		struct FloatShaderResource {};
	}


	//
	//以下、DescriptorHeap::PushBackViewを使う型は特殊化しておく必要がある
	//

	//namespace ViewType の型をViewTypeとしてusingする
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
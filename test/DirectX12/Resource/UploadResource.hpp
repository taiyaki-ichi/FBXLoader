#pragma once
#include"ResourceBase.hpp"

namespace DX12
{

	//インデックスバッファやバーテックスの基底クラスとなる
	//また、テクスチャのアップロード用
	class UploadResource : public ResourceBase
	{
	public:
		UploadResource() = default;
		virtual ~UploadResource() = default;

		void Initialize(Device*, std::size_t size);

		//コンテナ、配列用
		template<typename T>
		void Map(T&& container);

		//テクスチャデータ用
		void Map(uint8_t* data, std::size_t rowPitch, std::size_t height);
	};



	//
	//以下、テンプレートの部分の定義
	//

	template<typename T>
	inline void UploadResource::Map(T&& container)
	{
		using value_type = std::remove_reference_t<decltype(*std::begin(container))>;

		value_type* target = nullptr;
		auto result = Get()->Map(0, nullptr, (void**)&target);

		if (FAILED(result)) 
			throw "UploadResource::Map is failed\n";

		std::copy(std::begin(container), std::end(container), target);
		Get()->Unmap(0, nullptr);
	}



}
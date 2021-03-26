#pragma once
#include"ResourceBase.hpp"

namespace DX12
{

	//�C���f�b�N�X�o�b�t�@��o�[�e�b�N�X�̊��N���X�ƂȂ�
	//�܂��A�e�N�X�`���̃A�b�v���[�h�p
	class UploadResource : public ResourceBase
	{
	public:
		UploadResource() = default;
		virtual ~UploadResource() = default;

		void Initialize(Device*, std::size_t size);

		//�R���e�i�A�z��p
		template<typename T>
		void Map(T&& container);

		//�e�N�X�`���f�[�^�p
		void Map(uint8_t* data, std::size_t rowPitch, std::size_t height);
	};



	//
	//�ȉ��A�e���v���[�g�̕����̒�`
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
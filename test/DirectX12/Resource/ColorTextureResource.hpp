#pragma once
#include"ResourceBase.hpp"
#include"../DescriptorHeap/DescriptorHeapViewType.hpp"
#include<cstdint>
#include<vector>
#include<algorithm>

namespace DX12
{
	//�J���[�e�N�X�`���̃x�[�X�ƂȂ�N���X
	class ColorTextureResourceBase : public ResourceBase
	{
	public:
		virtual ~ColorTextureResourceBase() = default;

		void Initialize(Device*, unsigned int width, unsigned int height);
	};

	//4*4�̒P�F�̃��\�[�X
	template<std::uint8_t Color>
	class SimpleColorTextureResource : public ColorTextureResourceBase
	{
	public:
		void Initialize(Device* device);
	};


	//�����e�N�X�`��
	using WhiteTextureResource = SimpleColorTextureResource<0xff>;

	template<>
	struct ViewTypeTraits<WhiteTextureResource> {
		using ViewType = ViewType::Float4ShaderResource;
	};


	//�����e�N�X�`��
	using BlackTextureResource = SimpleColorTextureResource<0x00>;

	template<>
	struct ViewTypeTraits<BlackTextureResource> {
		using ViewType = ViewType::Float4ShaderResource;
	};


	//�O���C�O���f�[�V����
	class GrayGradationTextureResource : public ColorTextureResourceBase
	{
	public:
		void Initialize(Device*);
	};

	template<>
	struct ViewTypeTraits<GrayGradationTextureResource> {
		using ViewType = ViewType::Float4ShaderResource;
	};


	//
	//�ȉ��Asimple_color_texture_resource�̒�`
	//


	template<std::uint8_t Color>
	inline void SimpleColorTextureResource<Color>::Initialize(Device* device)
	{
		ColorTextureResourceBase::Initialize(device, 4, 4);

		std::vector<std::size_t> data(4 * 4 * 4);
		std::fill(data.begin(), data.end(), Color);

		if (FAILED(Get()->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size())))
			throw "color write is failed\n";
	}
}
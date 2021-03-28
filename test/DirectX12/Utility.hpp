#pragma once
#include<type_traits>
#include<array>
#include<tuple>
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{
	namespace DescriptorRange
	{
		struct SRV {
			constexpr static auto value = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		};

		struct UAV {
			constexpr static auto value = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		};

		struct CBV {
			constexpr static auto value = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		};

		struct Sampler {
			constexpr static auto value = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		};
	}


	template<typename... DescriptorRangeTypes>
	struct RootParameter
	{
		using Types = std::tuple<DescriptorRangeTypes...>;
	};

	template<typename DescriptorRangeType>
	constexpr auto GetDescriptorRangeStruct(std::array<std::size_t, 4>&& nums)
	{
		D3D12_DESCRIPTOR_RANGE descriptorRange{};
		descriptorRange.NumDescriptors = 1;
		descriptorRange.RangeType = DescriptorRangeType::value;
		descriptorRange.BaseShaderRegister = nums[DescriptorRangeType::value];
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		nums[DescriptorRangeType::value]++;

		return std::make_pair(std::move(descriptorRange), std::move(nums));
	}

	template<typename RootParameterType, std::size_t Index,std::size_t N>
	constexpr auto GetDescriptorRangeStructArray(std::array<D3D12_DESCRIPTOR_RANGE, N>& result, std::array<std::size_t, 4>&& nums)
	{
		if constexpr (Index == std::tuple_size_v<typename RootParameterType::Types>)
		{
			return std::move(nums);
		}
		else
		{
			auto [descriptorRange, returnNums] = GetDescriptorRangeStruct<std::tuple_element_t<Index, RootParameterType::Types>>(std::move(nums));
			result[Index] = std::move(descriptorRange);
			return GetDescriptorRangeStructArray<RootParameterType, Index + 1>(result, std::move(returnNums));
		}
	}


	struct End {};

	template<typename Head,typename... Tail>
	constexpr std::size_t GetSize()
	{
		if constexpr (std::is_same_v<Head, End>) {
			return 0;
		}
		else {
			return std::tuple_size_v<Head::Types>+GetSize<Tail...>();
		}
	}

	template<typename  First,typename Second, typename... Tail>
	constexpr auto GetRootParamsImpl(std::array<std::size_t, 4>&& nums)
	{
		std::array<D3D12_DESCRIPTOR_RANGE, std::tuple_size_v<typename First::Types>> rangeArray{};
		auto returnNums = GetDescriptorRangeStructArray<First, 0>(rangeArray, std::move(nums));

		if constexpr (std::is_same_v<Second, End>)
		{
			return std::make_tuple(std::move(rangeArray));
		}
		else 
		{
			auto lest = GetRootParamsImpl<Second, Tail...>(std::move(returnNums));
			return std::tuple_cat(std::make_tuple(std::move(rangeArray)), std::move(lest));
		}
	}

	template<typename... RootParameters>
	constexpr auto GetRootParams()
	{
		return GetRootParamsImpl<RootParameters... ,End>({});
	}


	namespace StaticSampler
	{
		//�ʏ�̕`�ʗp
		struct Normal {};

		//Toon�p
		struct Toon {};

		//ShadowMapping�p
		struct Shadow {};
	}

	template<typename... StaticSamplerTypes>
	struct StaticSamplers
	{
		using Types = std::tuple<StaticSamplerTypes...>;
	};


	template<typename StaticSamplerType,std::size_t Index>
	struct GetStaticSamplerDesc {};

	template<std::size_t Index>
	struct GetStaticSamplerDesc<StaticSampler::Normal,Index>{
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ�
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ�
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
			result.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//��Ԃ��Ȃ�(�j�A���X�g�l�C�o�[)
			result.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
			result.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//�I�[�o�[�T���v�����O�̍ۃ��T���v�����O���Ȃ��H
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂݉�
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<std::size_t Index>
	struct GetStaticSamplerDesc<StaticSampler::Shadow, Index> {
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
			result.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//��Ԃ��Ȃ�(�j�A���X�g�l�C�o�[)
			result.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
			result.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//�I�[�o�[�T���v�����O�̍ۃ��T���v�����O���Ȃ�
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂݉�
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<std::size_t Index>
	struct GetStaticSamplerDesc<StaticSampler::Toon, Index> {
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
			result.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//��r���ʂ����j�A�⊮
			result.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
			result.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂݉�
			result.MaxAnisotropy = 1;//�[�x�X�΂�L����
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<typename StaticSamplersType,std::size_t Index>
	constexpr auto GetStaticSamplerArray(std::array<D3D12_STATIC_SAMPLER_DESC, std::tuple_size_v<typename StaticSamplersType::Types>>& resultArray)
	{
		if constexpr (Index<std::tuple_size_v<typename StaticSamplersType::Types>)
		{
			resultArray[Index] = GetStaticSamplerDesc<std::tuple_element_t<Index, typename StaticSamplersType::Types>, Index>::Get();
			GetStaticSamplerArray<StaticSamplersType, Index + 1>(resultArray);
		}
	}

	template<typename StaticSamplersType>
	constexpr auto GetStaticSamplerArray()
	{
		std::array<D3D12_STATIC_SAMPLER_DESC, std::tuple_size_v<typename StaticSamplersType::Types>> result{};
		GetStaticSamplerArray<StaticSamplersType, 0>(result);

		return result;
	}
}


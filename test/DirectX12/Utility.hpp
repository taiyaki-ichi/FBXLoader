#pragma once
#include<type_traits>
#include<array>
#include<tuple>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXTex.h>
#include<Windows.h>
#include<optional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DirectXTex.lib")

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
	struct DescriptorTable
	{
		using Types = std::tuple<DescriptorRangeTypes...>;
	};

	template<typename... RootParameterTypes>
	struct DescrriptorTableArray
	{
		using Types = std::tuple<RootParameterTypes...>;
	};

	template<typename DescriptorRangeType>
	constexpr auto GetDescriptorRange(std::array<std::size_t, 4>&& nums)
	{
		D3D12_DESCRIPTOR_RANGE descriptorRange{};
		descriptorRange.NumDescriptors = 1;
		descriptorRange.RangeType = DescriptorRangeType::value;
		descriptorRange.BaseShaderRegister = nums[DescriptorRangeType::value];
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		nums[DescriptorRangeType::value]++;

		return std::make_pair(std::move(descriptorRange), std::move(nums));
	}

	template<typename DescriptorTableType, std::size_t Index,std::size_t N>
	constexpr auto GetDescriptorTableImpl(std::array<D3D12_DESCRIPTOR_RANGE, N>& result, std::array<std::size_t, 4>&& nums)
	{
		if constexpr (Index == std::tuple_size_v<typename DescriptorTableType::Types>)
		{
			return std::move(nums);
		}
		else
		{
			auto [descriptorRange, returnNums] = GetDescriptorRange<std::tuple_element_t<Index, DescriptorTableType::Types>>(std::move(nums));
			result[Index] = std::move(descriptorRange);
			return GetDescriptorTableImpl<DescriptorTableType, Index + 1>(result, std::move(returnNums));
		}
	}

	template<typename DescriptorTableType>
	constexpr auto GetDescriptorTable(std::array<std::size_t, 4>&& nums)
	{
		std::array<D3D12_DESCRIPTOR_RANGE, std::tuple_size_v<typename DescriptorTableType::Types>> result{};
		auto returnNums = GetDescriptorTableImpl<DescriptorTableType, 0>(result, std::move(nums));
		return std::make_pair(std::move(result), std::move(returnNums));
	}

	template<typename DescriptorTableType,std::size_t Index>
	constexpr auto GetDescriptorTableTupleDataImpl(std::array<std::size_t, 4>&& nums)
	{
		using NowDescriptorTableType = std::tuple_element_t<Index, typename DescriptorTableType::Types>;

		auto [rangeArray, returnNums] = GetDescriptorTable<NowDescriptorTableType>(std::move(nums));

		if constexpr (Index+1 < std::tuple_size_v<typename DescriptorTableType::Types>)
		{
			auto lest = GetDescriptorTableTupleDataImpl<DescriptorTableType, Index + 1>(std::move(returnNums));
			return std::tuple_cat(std::make_tuple(std::move(rangeArray)), std::move(lest));
		}
		else
		{
			return std::make_tuple(std::move(rangeArray));
		}
	}

	template<typename DescriptorTableArrayType>
	constexpr auto GetDescriptorTableTupleData()
	{
		return GetDescriptorTableTupleDataImpl<DescriptorTableArrayType, 0>({});
	}

	template<std::size_t Index, typename DescriptorTableDataTupleType,typename ResultArray>
	void GetDescriptorTableStructArrayImpl(const DescriptorTableDataTupleType& descriptorTableArray, ResultArray& resultArray)
	{
		if constexpr (Index < std::tuple_size_v<DescriptorTableDataTupleType>)
		{
			D3D12_ROOT_PARAMETER descriptorTable{};
			descriptorTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			descriptorTable.DescriptorTable.pDescriptorRanges = std::get<Index>(descriptorTableArray).data();
			descriptorTable.DescriptorTable.NumDescriptorRanges = std::get<Index>(descriptorTableArray).size();
			descriptorTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			resultArray[Index] = std::move(descriptorTable);

			GetDescriptorTableStructArrayImpl<Index + 1>(descriptorTableArray, resultArray);
		}
	}

	template<typename DescriptorTableDataTupleType>
	auto GetDescriptorTableStructArray(const DescriptorTableDataTupleType& descriptorTableArray)
	{
		std::array<D3D12_ROOT_PARAMETER, std::tuple_size_v<DescriptorTableDataTupleType>> result{};
		GetDescriptorTableStructArrayImpl<0>(descriptorTableArray, result);;
		return result;
	}




	namespace StaticSampler
	{
		//通常の描写用
		struct Normal {};

		//Toon用
		struct Toon {};

		//ShadowMapping用
		struct Shadow {};
	}

	template<typename... StaticSamplerTypes>
	struct StaticSamplers
	{
		using Types = std::tuple<StaticSamplerTypes...>;
	};


	template<typename StaticSamplerType,std::size_t Index>
	struct StaticSamplerDescGetter {};

	template<std::size_t Index>
	struct StaticSamplerDescGetter<StaticSampler::Normal,Index>{
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
			result.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//補間しない(ニアレストネイバー)
			result.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
			result.MinLOD = 0.0f;//ミップマップ最小値
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//オーバーサンプリングの際リサンプリングしない？
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ可視
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<std::size_t Index>
	struct StaticSamplerDescGetter<StaticSampler::Shadow, Index> {
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
			result.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//補間しない(ニアレストネイバー)
			result.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
			result.MinLOD = 0.0f;//ミップマップ最小値
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//オーバーサンプリングの際リサンプリングしない
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ可視
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<std::size_t Index>
	struct StaticSamplerDescGetter<StaticSampler::Toon, Index> {
		constexpr static auto Get() {

			D3D12_STATIC_SAMPLER_DESC result{};
			result.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
			result.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//比較結果をリニア補完
			result.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
			result.MinLOD = 0.0f;//ミップマップ最小値
			result.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			result.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ可視
			result.MaxAnisotropy = 1;//深度傾斜を有効に
			result.RegisterSpace = Index;

			return result;
		}
	};

	template<typename StaticSamplersType,std::size_t Index>
	constexpr auto GetStaticSamplerArray(std::array<D3D12_STATIC_SAMPLER_DESC, std::tuple_size_v<typename StaticSamplersType::Types>>& resultArray)
	{
		if constexpr (Index<std::tuple_size_v<typename StaticSamplersType::Types>)
		{
			resultArray[Index] = StaticSamplerDescGetter<std::tuple_element_t<Index, typename StaticSamplersType::Types>, Index>::Get();
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

	inline constexpr size_t AlignmentSize(size_t size, size_t alignment) {
		return size + alignment - size % alignment;
	}

	inline std::wstring ToWstring(const std::string& str)
	{
		if (str.empty())
			return {};

		int neededSize = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), NULL, 0);
		std::wstring result(neededSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &result[0], neededSize);

		return result;
	}

	//画像データの取得
	//あと、ScratchImageはコピー不可
	inline std::optional<std::pair<DirectX::TexMetadata, DirectX::ScratchImage>>
		GetTexture(const std::string& fileName) {

		DirectX::TexMetadata metaData{};
		DirectX::ScratchImage scratch{};
		if (FAILED(LoadFromWICFile(ToWstring(fileName).c_str(), DirectX::WIC_FLAGS_NONE, &metaData, scratch))) {
			return std::nullopt;
		}
		return std::make_pair(std::move(metaData), std::move(scratch));
	}

}


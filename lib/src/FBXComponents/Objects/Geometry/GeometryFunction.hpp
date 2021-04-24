#pragma once
#include"GeometryStruct.hpp"
#include"../../GlobalSettings/GlobalSettingsStruct.hpp"
#include<functional>
#include<stdexcept>
#include<tuple>

namespace FBXL
{
	//GeometryMeshの取得
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);

	//GeometyMeshNodeからマテリアルのインデックス配列を取得
	inline std::optional<std::vector<std::int32_t>> GetPrimitiveMaterialIndeces(const Node* geometyMesh);


	//ポリゴンごとの頂点を参照するインデックスとマテリアルを参照するインデックスのペア
	//と、頂点の配列を取得
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings);


	template<typename DataType>
	std::pair<std::vector<std::int32_t>, DataType> GetIndecesAndData(DataType&&);

	inline std::vector<std::int32_t> UpdateIndeces(std::vector<std::int32_t>&& prevIndeces, std::vector<std::int32_t>&& newIndeces);
	inline std::vector<std::int32_t> UpdateIndeces(std::vector<std::int32_t>&& prevIndeces, const std::vector<std::int32_t>& newIndeces);


	template<typename T,std::size_t N,typename... Args>
	struct TupleNImpl {
		using Type = typename TupleNImpl<T, N - 1, T, Args...>::Type;
	};

	template<typename T,typename... Args>
	struct TupleNImpl<T,0,Args...> {
		using Type = std::tuple<Args...>;
	};

	template<std::size_t N>
	using DoubleTuple = typename TupleNImpl<double, N>::Type;

	template<std::size_t N>
	using Int32Tuple = typename TupleNImpl<std::int32_t, N>::Type;


	template<std::size_t N>
	std::vector<DoubleTuple<N>> GetDoubleTuples(std::vector<double>&&);

	//vectorからindex番目のDoubleTupleの取得
	template<std::size_t N>
	DoubleTuple<N> GetDoubleTuplesHelper(std::vector<double>&, std::size_t index);

	template<>
	DoubleTuple<1> GetDoubleTuplesHelper<1>(std::vector<double>&, std::size_t index);


	struct LayerElementNormalInformation {
		constexpr static char layerElementName[] = "LayerElementNormal";
		constexpr static char rawDoubleDataName[] = "Normals";
		constexpr static char indexDataName[] = "NormalsIndex";

		constexpr static std::size_t dimension = 3;
	};

	struct LayerElementUVInformation {
		constexpr static char layerElementName[] = "LayerElementUV";
		constexpr static char rawDoubleDataName[] = "UV";
		constexpr static char indexDataName[] = "UVIndex";

		constexpr static std::size_t dimension = 2;
	};

	template<typename LayerElementInformation, typename DataType = std::vector<DoubleTuple<LayerElementInformation::dimension>>>
	std::pair<std::vector<std::int32_t>,DataType> 
		GetLayerElementIndecesAndData(const Node* geometryMesh, const std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces);


	//全てが正のインデックスとそれを三角形のポリゴンとして解釈するためのインデックスインデックス
	//あとポリゴンに対応したマテリアルのインデックスも
	inline std::tuple<std::vector<std::int32_t>, std::vector<std::int32_t>, std::vector<std::int32_t>>
		GetIndecesAndIndexIndecesAndMaterialIndeces(const Node* geometryMesh);


	inline std::pair<std::vector<std::int32_t>, std::vector<DoubleTuple<3>>> GetPositionIndecesAndData(
		const Node* geometryMesh, std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces);


	template<typename IndecesHead,typename... IndecesTails>
	auto GetIndexTuples(IndecesHead&& indecesHead, IndecesTails&&... indecesTails);

	template<typename... Indeces>
	auto GetIndexIndecesAndIndeces(Indeces&&... indecesArgs);


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> GetTrianglePolygonAndMaterialIndexPairs(
		std::vector<std::int32_t>&& indeces, std::vector<std::int32_t>&& materialIndeces);

	template<std::size_t Index,typename Tuple,typename DataArray>
	decltype(auto) GetDataFromIndexTuple(Tuple& tuple, DataArray& dataArray);

	template<typename ResultType,std::size_t... TupleIndeces,typename IndexTuple,typename... DataArrays>
	ResultType GetResultTypeFromIndexTupleImpl(std::index_sequence<TupleIndeces...>, IndexTuple& indexTuple, DataArrays&... dataArray);

	template<typename ResultType, typename IndexTuple, typename... DataArrays>
	ResultType GetResultTypeFromIndexTuple(IndexTuple&& indexTuple, DataArrays&... dataArrays);

	template<typename ResultType, typename IndexTuples, typename... DataArrays>
	std::vector<ResultType> GetResultTypesFromIndexTuples(IndexTuples&& tuples, DataArrays&&... datas);

	template<typename VectorND,typename CreateVectorNDPolicy,std::size_t N>
	std::vector<VectorND> GetVectorNDs(std::vector<DoubleTuple<N>>&&, const GlobalSettings& globalSettings);

	inline DoubleTuple<3> AxisConversionDoubleTuple3(DoubleTuple<3>&&, const GlobalSettings& globalSettings);


	//
	//以下、実装
	//

	inline std::optional<std::vector<std::int32_t>> GetPrimitiveMaterialIndeces(const Node* geometyMesh)
	{
		auto layerElementMaterialNode = GetSingleChildrenNode(geometyMesh, "LayerElementMaterial");

		if (layerElementMaterialNode)
		{
			auto materialNode = GetSingleChildrenNode(layerElementMaterialNode.value(), "Materials");

			return GetProperty<std::vector<std::int32_t>>(materialNode.value(), 0).value();
		}
		else
			return std::nullopt;
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings)
	{
		auto [indeces, indexIndeces, materialIndeces] = GetIndecesAndIndexIndecesAndMaterialIndeces(geometryMesh);
		auto [posIndeces,posData] = GetPositionIndecesAndData(geometryMesh, indeces, indexIndeces);
		auto [normalIndeces,normalData] = GetLayerElementIndecesAndData<LayerElementNormalInformation>(geometryMesh, indeces, indexIndeces);
		auto [uvIndeces,uvData] = GetLayerElementIndecesAndData<LayerElementUVInformation>(geometryMesh, indeces, indexIndeces);

		auto posVector3Ds = GetVectorNDs<Vector3D,CreateVector3DPolicy,3>(std::move(posData), globalSettings);
		auto normalVector3Ds = GetVectorNDs<Vector3D, CreateVector3DPolicy,3>(std::move(normalData), globalSettings);
		auto uvVector2Ds = GetVectorNDs<Vector2D, CreateVector2DPolicy,2>(std::move(uvData), globalSettings);

		auto [indexTupleIndeces, indexTuples] = GetIndexIndecesAndIndeces(std::move(posIndeces), std::move(normalIndeces), std::move(uvIndeces));

		auto vertices = GetResultTypesFromIndexTuples<Vertex<Vector2D, Vector3D>>(
			std::move(indexTuples), std::move(posVector3Ds), std::move(normalVector3Ds), std::move(uvVector2Ds));

		auto indexPairs = GetTrianglePolygonAndMaterialIndexPairs(std::move(indexTupleIndeces), std::move(materialIndeces));

		return { std::move(indexPairs),std::move(vertices) };
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(geormetryMesh.name == "Geometry" && GetProperty<std::string>(&geormetryMesh, 2) == "Mesh");

		auto index = GetProperty<std::int64_t>(&geormetryMesh, 0).value();

		auto [primitiveIndeces, vertices] =
			GetIndecesAndVertecesAndPolygonInfomationTuple<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh, globalSettings);


		return std::make_pair(GeometryMesh<Vector2D, Vector3D>{primitiveIndeces, vertices }, index);

	}


	template<typename DataType>
	std::pair<std::vector<std::int32_t>, DataType> GetIndecesAndData(DataType&& data)
	{
		auto returnData = data;
		std::vector<std::int32_t> indeces(data.size());

		{
			std::sort(returnData.begin(), returnData.end());
			auto iter = std::unique(returnData.begin(), returnData.end());
			returnData.erase(iter, returnData.end());
		}

		for (std::size_t i = 0; i < indeces.size(); i++)
		{
			auto iter = std::lower_bound(returnData.begin(), returnData.end(), data[i]);
			indeces[i] = iter - returnData.begin();
		}

		return { std::move(indeces),std::move(returnData) };
	}


	inline std::vector<std::int32_t> UpdateIndeces(std::vector<std::int32_t>&& prevIndeces, std::vector<std::int32_t>&& newIndeces)
	{
		for (std::size_t i = 0; i < newIndeces.size(); i++)
			newIndeces[i] = prevIndeces[newIndeces[i]];

		return newIndeces;
	}

	inline std::vector<std::int32_t> UpdateIndeces(std::vector<std::int32_t>&& prevIndeces, const std::vector<std::int32_t>& newIndeces)
	{
		auto indeces = newIndeces;
		return UpdateIndeces(std::move(prevIndeces), std::move(indeces));
	}

	template<std::size_t N>
	std::vector<DoubleTuple<N>> GetDoubleTuples(std::vector<double>&& data)
	{
		std::vector<DoubleTuple<N>> result{};
		result.reserve(data.size() / N);
		for (std::size_t i = 0; i < data.size() / N; i++)
			result.emplace_back(GetDoubleTuplesHelper<N>(data, i * N));

		return result;
	}

	template<std::size_t N>
	DoubleTuple<N> GetDoubleTuplesHelper(std::vector<double>& data, std::size_t index)
	{
		return std::tuple_cat(std::make_tuple(data[index]), GetDoubleTuplesHelper<N - 1>(data, index + 1));
	}

	template<>
	DoubleTuple<1> GetDoubleTuplesHelper<1>(std::vector<double>& data, std::size_t index)
	{
		return std::make_tuple(data[index]);
	}



	template<typename LayerElementInformation, typename DataType>
	std::pair<std::vector<std::int32_t>, DataType> GetLayerElementIndecesAndData(const Node* geometryMesh, const std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces)
	{
		auto layerElementNode = GetSingleChildrenNode(geometryMesh, LayerElementInformation::layerElementName);

		auto doubleDataNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::rawDoubleDataName);
		auto doubleData = GetProperty<std::vector<double>>(doubleDataNode.value(), 0).value();

		auto doubleTuples = GetDoubleTuples<LayerElementInformation::dimension>(std::move(doubleData));

		auto [resultIndeces, resultData] = GetIndecesAndData(std::move(doubleTuples));

		auto referenceInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "ReferenceInformationType");
		if (referenceInformationTypeNode && GetProperty<std::string>(referenceInformationTypeNode.value(), 0) == "IndexToDirect")
		{
			auto indexNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::indexDataName);
			auto primitiveIndeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();
			resultIndeces = UpdateIndeces(std::move(resultIndeces), std::move(primitiveIndeces));
		}

		auto mappingInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "MappingInformationType");
		if (mappingInformationTypeNode && GetProperty<std::string>(mappingInformationTypeNode.value(), 0) == "ByControlPoint")
		{
			resultIndeces = UpdateIndeces(std::move(resultIndeces), indeces);
		}

		resultIndeces = UpdateIndeces(std::move(resultIndeces), indexIndeces);

		return { std::move(resultIndeces), std::move(resultData) };
	}

	inline std::tuple<std::vector<std::int32_t>, std::vector<std::int32_t>, std::vector<std::int32_t>>
		GetIndecesAndIndexIndecesAndMaterialIndeces(const Node* geometryMesh)
	{
		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

		auto primitiveMaterialIndeces = GetPrimitiveMaterialIndeces(geometryMesh);

		//マテリアルが単一の場合は常に0
		auto getMaterialIndex = [&primitiveMaterialIndeces](std::size_t polygonIndex) {
			return (primitiveMaterialIndeces && primitiveMaterialIndeces.value().size() > 1) ? primitiveMaterialIndeces.value()[polygonIndex] : 0;
		};

		std::vector<std::int32_t> indexIndeces{};
		indexIndeces.reserve(indeces.size());

		std::vector<std::int32_t> materialIndeces{};
		materialIndeces.reserve(indeces.size());

		std::int32_t i = 0;
		std::int32_t j = 0;
		std::size_t polygonIndex = 0;
		while (i < indeces.size())
		{
			j = i + 1;
			do {
				indexIndeces.push_back(i);
				indexIndeces.push_back(j);
				indexIndeces.push_back(j + 1);

				materialIndeces.push_back(getMaterialIndex(polygonIndex));

				j++;
			} while (j < indeces.size() && indeces[j] >= 0);
			i = j + 1;
			polygonIndex++;
		}

		//ポリゴンの境目のマイナスのインデックスはもとに戻しておく
		for (std::size_t i = 0; i < indeces.size(); i++)
			if (indeces[i] < 0)
				indeces[i] = -indeces[i] - 1;

		return { std::move(indeces),std::move(indexIndeces),std::move(materialIndeces) };
	}


	inline std::pair<std::vector<std::int32_t>, std::vector<DoubleTuple<3>>> GetPositionIndecesAndData(
		const Node* geometryMesh, std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces)
	{
		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		auto doubleVector = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		auto doubleTuples = GetDoubleTuples<3>(std::move(doubleVector));

		auto [resultIndeces, resultDatas] = GetIndecesAndData(std::move(doubleTuples));

		resultIndeces = UpdateIndeces(std::move(resultIndeces), indeces);
		resultIndeces = UpdateIndeces(std::move(resultIndeces), indexIndeces);

		return { std::move(resultIndeces),std::move(resultDatas) };
	}


	template<typename IndecesHead, typename... IndecesTails>
	auto GetIndexTuples(IndecesHead&& indecesHead, IndecesTails&&... indecesTails)
	{
		if constexpr (sizeof...(IndecesTails) <= 0)
		{
			std::vector<std::tuple<typename IndecesHead::value_type>> result{};
			result.reserve(indecesHead.size());
			for (std::size_t i = 0; i < indecesHead.size(); i++) {
				if constexpr (std::is_reference_v<IndecesHead>)
					result.emplace_back(std::move(indecesHead[i]));
				else
					result.emplace_back(indecesHead[i]);
			}

			return result;
		}
		else
		{
			auto prevTuples = GetIndexTuples(std::forward<IndecesTails>(indecesTails)...);
			using NowTupleType = std::tuple<typename IndecesHead::value_type>;
			using PrevTupleType = typename decltype(prevTuples)::value_type;
			using ReturnTupleType = std::invoke_result_t<decltype(std::tuple_cat<NowTupleType, PrevTupleType>), NowTupleType, PrevTupleType>;

			std::vector<ReturnTupleType> result{};
			result.reserve(indecesHead.size());
			for (std::size_t i = 0; i < indecesHead.size(); i++)
			{
				if constexpr (std::is_reference_v<IndecesHead>)
					result.emplace_back(std::tuple_cat<NowTupleType,PrevTupleType>(std::move(indecesHead[i]), std::move(prevTuples[i])));
				else
					result.emplace_back(std::tuple_cat<NowTupleType, PrevTupleType>(indecesHead[i], std::move(prevTuples[i])));
			}

			return result;
		}
	}

	template<typename... Indeces>
	auto GetIndexIndecesAndIndeces(Indeces&&... indecesArgs)
	{
		static_assert(sizeof...(Indeces) > 0);

		auto indeces = GetIndexTuples(std::forward<Indeces>(indecesArgs)...);
		auto indecesMemo = indeces;

		{
			std::sort(indeces.begin(), indeces.end());
			auto iter = std::unique(indeces.begin(), indeces.end());
			indeces.erase(iter, indeces.end());
		}

		std::vector<std::int32_t> indexIndeces(indecesMemo.size());
		for (std::size_t i = 0; i < indecesMemo.size(); i++)
		{
			auto iter = std::lower_bound(indeces.begin(), indeces.end(), indecesMemo[i]);
			indexIndeces[i] = iter - indeces.begin();
		}

		return std::make_pair(std::move(indexIndeces), std::move(indeces));
	}


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> GetTrianglePolygonAndMaterialIndexPairs(std::vector<std::int32_t>&& indeces, std::vector<std::int32_t>&& materialIndeces)
	{
		std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> result{};
		result.reserve(indeces.size() / 3);

		for (std::size_t i = 0; i < indeces.size() / 3; i++)
			result.emplace_back(std::make_tuple(indeces[i * 3], indeces[i * 3 + 1], indeces[i * 3 + 2]), materialIndeces[i]);

		return result;
	}

	template<std::size_t Index, typename Tuple, typename DataArray>
	decltype(auto) GetDataFromIndexTuple(Tuple& tuple, DataArray& dataArray)
	{
		return std::move(dataArray[std::get<Index>(tuple)]);
	}

	template<typename ResultType, std::size_t ...TupleIndeces, typename IndexTuple, typename ...DataArrays>
	ResultType GetResultTypeFromIndexTupleImpl(std::index_sequence<TupleIndeces...>, IndexTuple& indexTuple, DataArrays & ... dataArray)
	{
		return { GetDataFromIndexTuple<TupleIndeces>(indexTuple,dataArray)... };
	}

	template<typename ResultType, typename IndexTuple, typename ...DataArrays>
	ResultType GetResultTypeFromIndexTuple(IndexTuple&& indexTuple, DataArrays & ...dataArrays)
	{
		return GetResultTypeFromIndexTupleImpl<ResultType>(std::make_index_sequence<sizeof...(DataArrays)>{}, indexTuple, dataArrays...);
	}

	template<typename ResultType, typename IndexTuples, typename ...DataArrays>
	std::vector<ResultType> GetResultTypesFromIndexTuples(IndexTuples&& tuples, DataArrays && ...datas)
	{
		std::vector<ResultType> result{};
		result.reserve(tuples.size());
		for (std::size_t i = 0; i < tuples.size(); i++)
			result.emplace_back(GetResultTypeFromIndexTuple<ResultType>(std::move(tuples[i]), datas...));

		return result;
	}

	template<typename VectorND, typename CreateVectorNDPolicy, std::size_t N>
	std::vector<VectorND> GetVectorNDs(std::vector<DoubleTuple<N>>&& doubleTuples, const GlobalSettings& globalSettings)
	{
		static_assert(N == 2 || N == 3);

		std::vector<VectorND> result{};
		result.reserve(doubleTuples.size());

		for (std::size_t i = 0; i < doubleTuples.size(); i++)
		{
			if constexpr (N == 2)
				//
				//N==2はuvしかないので2つ目はマイナス決め打ち
				//
				result.emplace_back(CreateVectorNDPolicy::Create(std::get<0>(doubleTuples[i]), -std::get<1>(doubleTuples[i])));
			else if constexpr (N == 3)
			{
				auto [x, y, z] = AxisConversionDoubleTuple3(std::move(doubleTuples[i]), globalSettings);
				result.emplace_back(CreateVectorNDPolicy::Create(x, y, z));
			}
		}

		return result;
	}

	inline DoubleTuple<3> AxisConversionDoubleTuple3(DoubleTuple<3>&& doubleTuple, const GlobalSettings& globalSettings)
	{
		double nums[] = { std::get<0>(doubleTuple),std::get<1>(doubleTuple) ,std::get<2>(doubleTuple) };

		return { nums[globalSettings.coordAxis] * globalSettings.coordAxisSign,
			nums[globalSettings.upAxis] * globalSettings.upAxisSign,
			nums[globalSettings.frontAxis] * globalSettings.coordAxisSign
		};
	}
}
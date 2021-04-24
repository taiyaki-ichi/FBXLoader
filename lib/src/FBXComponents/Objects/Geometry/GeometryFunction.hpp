#pragma once
#include"GeometryStruct.hpp"
#include"../../GlobalSettings/GlobalSettingsStruct.hpp"
#include<functional>
#include<stdexcept>
#include<tuple>

namespace FBXL
{
	//GeometyMeshNodeからマテリアルのインデックス配列を取得
	inline std::optional<std::vector<std::int32_t>> GetPrimitiveMaterialIndeces(const Node* geometyMesh);

	//GlobalSettingを適用するためのインターフェース
	template<typename CreateVector3DPolicy>
	struct CreateVector3DInterfacePolicy
	{
		static auto Invoke(double a, double b, double c, const GlobalSettings& globalSettings);
	};


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

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings);

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);

	template<typename DataType>
	struct IndecedData {
		using DataValueType = typename DataType::value_type;

		std::vector<std::int32_t> primitiveIndeces{};
		DataType data{};
	};

	template<typename DataType>
	IndecedData<DataType> GetIndecedData(DataType&& data);

	template<typename DataType>
	std::pair<std::vector<std::int32_t>, std::vector<DataType>> GetIndecesAndData(DataType&&);


	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&&, std::vector<std::int32_t>&& primitiveIndeces);

	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&&, const std::vector<std::int32_t>& primitiveIndeces);

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

	//vecctorからindex番目のDoubleTupleの取得
	template<std::size_t N>
	DoubleTuple<N> GetDoubleTuplesHelper(std::vector<double>&, std::size_t index);

	template<>
	DoubleTuple<1> GetDoubleTuplesHelper<1>(std::vector<double>&, std::size_t index);

	//指定した情報からIndecedDataの取得
	template<typename LayerElementInformation,typename DataType= std::vector<DoubleTuple<LayerElementInformation::dimension>>>
	IndecedData<DataType> GetLayerElementIndecedData(const Node* geometryMesh, const std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces);;


	//全てが正のインデックスとそれを三角形のポリゴンとして解釈するためのインデックスインデックス
	//あとポリゴンに対応したマテリアルのインデックスも
	inline std::tuple<std::vector<std::int32_t>, std::vector<std::int32_t>, std::vector<std::int32_t>>
		GetIndecesAndIndexIndecesAndMaterialIndeces(const Node* geometryMesh);

	//Positionのインデクス化されたデータの取得
	inline IndecedData<std::vector<DoubleTuple<3>>> GetPositionIndecedData(
		const Node* geometryMesh, std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces);

	IndecedData<std::vector<Int32Tuple<3>>> GetIndexTupleIndecedData(
		const IndecedData<std::vector<DoubleTuple<3>>>& positionIndecedData,
		const IndecedData<std::vector<DoubleTuple<3>>>& normalIndecedData,
		const IndecedData<std::vector<DoubleTuple<2>>>& uvIndecedData
		);

	template<typename IndecesHead,typename... IndecesTails>
	auto GetIndexTuples(IndecesHead&& indecesHead, IndecesTails&&... indecesTails);

	template<typename... Indeces>
	auto GetIndexIndecesAndIndeces(Indeces&&... indecesArgs);

	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVerteces(std::vector<Int32Tuple<3>>&& indexTuples,
		IndecedData<std::vector<DoubleTuple<3>>>&& positionIndecedData,
		IndecedData<std::vector<DoubleTuple<3>>>&& normalIndecedData,
		IndecedData<std::vector<DoubleTuple<2>>>&& uvIndecedData,
		const GlobalSettings& globalSettings
	);

	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> GetTrianglePolygonAndMaterialIndexPairs(
		std::vector<std::int32_t>&& indeces, std::vector<std::int32_t>&& materialIndeces);

	template<std::size_t Index,typename Tuple,typename DataArray>
	decltype(auto) GetDataFromIndexTuple(Tuple& tuple, DataArray& dataArray);

	template<typename ResultType,std::size_t... TupleIndeces,typename IndexTuple,typename... DataArrays>
	ResultType GetResultTypeFromIndexTupleImpl(std::index_sequence<TupleIndeces...>, IndexTuple& indexTuple, DataArrays&... dataArray);

	template<typename ResultType, typename IndexTuple, typename... DataArrays>
	ResultType GetResultTypeFromIndexTuple(IndexTuple&& indexTuple, DataArrays&... dataArrays);

	template<typename ResultType, typename IndexTuples, typename... Datas>
	std::vector<ResultType> GetResultTypesFromIndexTuples(IndexTuples&& tuples, Datas&&... datas);

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


	template<typename CreateVector3DPolicy>
	inline auto CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(double a, double b, double c, const GlobalSettings& globalSettings)
	{
		double nums[] = { a,b,c };
		return CreateVector3DPolicy::Create(
			nums[globalSettings.coordAxis] * globalSettings.coordAxisSign,
			nums[globalSettings.upAxis] * globalSettings.upAxisSign,
			nums[globalSettings.frontAxis] * globalSettings.coordAxisSign);
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings)
	{
		auto [indeces, indexIndeces, materialIndeces] = GetIndecesAndIndexIndecesAndMaterialIndeces(geometryMesh);
		auto positionIndecedData = GetPositionIndecedData(geometryMesh, indeces, indexIndeces);
		auto normalIndecedData = GetLayerElementIndecedData<LayerElementNormalInformation>(geometryMesh, indeces, indexIndeces);
		auto uvIndecedData = GetLayerElementIndecedData<LayerElementUVInformation>(geometryMesh, indeces, indexIndeces);

		auto indexTuples = GetIndexTupleIndecedData(positionIndecedData, normalIndecedData, uvIndecedData);
		auto vertices = GetVerteces<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(
				std::move(indexTuples.data), std::move(positionIndecedData), std::move(normalIndecedData), std::move(uvIndecedData), globalSettings);
		auto indexPairs = GetTrianglePolygonAndMaterialIndexPairs(std::move(indexTuples.primitiveIndeces), std::move(materialIndeces));

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
	IndecedData<DataType> GetIndecedData(DataType&& data)
	{
		auto returnData = data;
		std::vector<std::int32_t> primitiveIndeces(data.size());

		{
			std::sort(returnData.begin(), returnData.end());
			auto iter = std::unique(returnData.begin(), returnData.end());
			returnData.erase(iter, returnData.end());
		}

		for (std::size_t i = 0; i < primitiveIndeces.size(); i++)
		{
			auto iter = std::lower_bound(returnData.begin(), returnData.end(), data[i]);
			primitiveIndeces[i] = iter - returnData.begin();
		}

		return { std::move(primitiveIndeces),std::move(returnData) };
	}

	template<typename DataType>
	std::pair<std::vector<std::int32_t>, std::vector<DataType>> GetIndecesAndData(DataType&& data)
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

	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&& indecedData, std::vector<std::int32_t>&& primitiveIndeces)
	{
		for (std::size_t i = 0; i < primitiveIndeces.size(); i++)
			primitiveIndeces[i] = indecedData.primitiveIndeces[primitiveIndeces[i]];

		indecedData.primitiveIndeces = std::move(primitiveIndeces);

		return indecedData;
	}

	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&& indecedData, const std::vector<std::int32_t>& primitiveIndeces)
	{
		auto tmpIndeces = primitiveIndeces;
		return UpdateIndeces(std::move(indecedData), std::move(tmpIndeces));
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
	IndecedData<DataType> GetLayerElementIndecedData(const Node* geometryMesh, const std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces)
	{
		IndecedData<DataType> result{};

		auto layerElementNode = GetSingleChildrenNode(geometryMesh, LayerElementInformation::layerElementName);

		auto doubleDataNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::rawDoubleDataName);
		auto doubleData = GetProperty<std::vector<double>>(doubleDataNode.value(), 0).value();

		auto doubleTuples = GetDoubleTuples<LayerElementInformation::dimension>(std::move(doubleData));

		auto indecedData = GetIndecedData(std::move(doubleTuples));

		auto referenceInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "ReferenceInformationType");
		if (referenceInformationTypeNode && GetProperty<std::string>(referenceInformationTypeNode.value(), 0) == "IndexToDirect")
		{
			auto indexNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::indexDataName);
			auto primitiveIndeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();
			indecedData = UpdateIndeces(std::move(indecedData), std::move(primitiveIndeces));
		}

		auto mappingInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "MappingInformationType");
		if (mappingInformationTypeNode && GetProperty<std::string>(mappingInformationTypeNode.value(), 0) == "ByControlPoint")
		{
			indecedData = UpdateIndeces(std::move(indecedData), indeces);
		}

		indecedData = UpdateIndeces(std::move(indecedData), indexIndeces);

		return indecedData;
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

	inline IndecedData<std::vector<DoubleTuple<3>>> GetPositionIndecedData(
		const Node* geometryMesh, std::vector<std::int32_t>& indeces, const std::vector<std::int32_t>& indexIndeces)
	{
		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		auto doubleVector = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		auto doubleTuples = GetDoubleTuples<3>(std::move(doubleVector));

		auto indecedData = GetIndecedData(std::move(doubleTuples));

		indecedData = UpdateIndeces(std::move(indecedData), indeces);
		indecedData = UpdateIndeces(std::move(indecedData), indexIndeces);

		return indecedData;
	}

	IndecedData<std::vector<Int32Tuple<3>>> GetIndexTupleIndecedData(const IndecedData<std::vector<DoubleTuple<3>>>& positionIndecedData, const IndecedData<std::vector<DoubleTuple<3>>>& normalIndecedData, const IndecedData<std::vector<DoubleTuple<2>>>& uvIndecedData)
	{
		std::vector<Int32Tuple<3>> int32Tuples{};
		int32Tuples.reserve(positionIndecedData.primitiveIndeces.size());

		for (std::size_t i = 0; i < positionIndecedData.primitiveIndeces.size(); i++)
			int32Tuples.emplace_back(positionIndecedData.primitiveIndeces[i], normalIndecedData.primitiveIndeces[i], uvIndecedData.primitiveIndeces[i]);

		return GetIndecedData(std::move(int32Tuples));
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
		static_assert(sizeof...(indecesArgs) > 0);

		auto indeces = GetIndexTuples(std::forward<Indeces>(indecesArgs)...);
		auto indecesMemo = indeces;

		{
			std::sort(indeces.begin(), indeces.end());
			auto iter = std::unique(indeces.begin(), indeces.end());
			indeces.erase(iter, indeces.end());
		}

		std::vector<std::int32_t> indexIndeces{};
		indexIndeces.reserve(indecesMemo.size());
		for (std::size_t i = 0; indecesMemo.size(); i++)
		{
			auto iter = std::lower_bound(indeces.begin(), indeces.end(), indecesMemo[i]);
			indexIndeces[i] = iter - indeces.begin();
		}

		return { std::move(indexIndeces),std::move(indeces) };
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVerteces(std::vector<Int32Tuple<3>>&& indexTuples, 
		IndecedData<std::vector<DoubleTuple<3>>>&& positionIndecedData, IndecedData<std::vector<DoubleTuple<3>>>&& normalIndecedData, IndecedData<std::vector<DoubleTuple<2>>>&& uvIndecedData,
		const GlobalSettings& globalSettings)
	{
		auto getPosition = [&positionIndecedData, &globalSettings](std::int32_t i) -> Vector3D {
			return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
				std::get<0>(positionIndecedData.data[i]), std::get<1>(positionIndecedData.data[i]), std::get<2>(positionIndecedData.data[i]), globalSettings);
		};

		auto getNormal = [&normalIndecedData, &globalSettings](std::int32_t i)->Vector3D {
			return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
				std::get<0>(normalIndecedData.data[i]), std::get<1>(normalIndecedData.data[i]), std::get<2>(normalIndecedData.data[i]), globalSettings);
		};

		auto getUV = [&uvIndecedData, &globalSettings](std::int32_t i)->Vector2D {
			//
			//uvの2つ目はマイナス
			//
			return CreateVector2DPolicy::Create(std::get<0>(uvIndecedData.data[i]), -std::get<1>(uvIndecedData.data[i]));
		};

		std::vector<Vertex<Vector2D, Vector3D>> result{};
		result.reserve(indexTuples.size());

		for (std::size_t i = 0; i < indexTuples.size(); i++)
			result.emplace_back(Vertex<Vector2D, Vector3D>{ getPosition(std::get<0>(indexTuples[i])), getNormal(std::get<1>(indexTuples[i])), getUV(std::get<2>(indexTuples[i])) });

		return result;
	}


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> FBXL::GetTrianglePolygonAndMaterialIndexPairs(std::vector<std::int32_t>&& indeces, std::vector<std::int32_t>&& materialIndeces)
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

	template<typename ResultType, typename IndexTuples, typename ...Datas>
	std::vector<ResultType> GetResultTypesFromIndexTuples(IndexTuples&& tuples, Datas && ...datas)
	{
		std::vector<ResultType> result{};
		result.reserve(tuples.size());
		for (std::size_t i = 0; i < tuples.size(); i++)
			result.emplace_back(GetResultTypeFromIndexTuple<ResultType>(std::move(tuples[i]), datas...));

		return result;
	}
}
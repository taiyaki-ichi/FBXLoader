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


	using RawData = std::vector<double>;
	using IndexAndRawDataPair = std::pair<std::vector<std::int32_t>, std::vector<double>>;

	using DoubleDataVariant = std::variant<RawData, IndexAndRawDataPair>;

	struct PrimitiveDoubleData
	{
		bool isByPolygonVertex{};
		DoubleDataVariant dataVarivant{};
	};

	std::size_t GetPrimitiveDoubleDataIndex(const PrimitiveDoubleData&, const std::vector<std::int32_t>& primitiveIndeces, std::int32_t index);

	PrimitiveDoubleData GetVertecesPrimitiveDoubleData(const Node* geometryMesh);

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

	template<typename LayerElementInformation>
	PrimitiveDoubleData GetPrimitiveDoubleData(const Node* geometryMesh);

	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& vertices, const std::vector<std::int32_t>& primitiveIndeces, std::size_t index);

	struct GetPrimitiveDoubleDataIndexVisitor {
		std::int32_t index;
		std::size_t operator()(const RawData&);
		std::size_t operator()(const IndexAndRawDataPair&);
	};

	template<typename Vector2D, typename CreateVector2DPolicy>
	struct CreateVector2DFromIndexVisitor {
		const GlobalSettings& globalSettings;
		std::size_t index;

		Vector2D operator()(const RawData&);
		Vector2D operator()(const IndexAndRawDataPair&);
	};

	template<typename Vector3D, typename CreateVector3DPolicy>
	struct CreateVector3DFromIndexVisitor {
		const GlobalSettings& globalSettings;
		std::size_t index;

		Vector3D operator()(const RawData&);
		Vector3D operator()(const IndexAndRawDataPair&);
	};

	//pos,normal,uvの順
	using IndexTuple = std::tuple<std::size_t, std::size_t, std::size_t>;

	std::pair<std::vector<IndexTuple>, std::vector<std::int32_t>> GetIndexTuplesAndIndexPolygonInformationPair(const std::vector<std::int32_t>& primitiveIndeces,
		const PrimitiveDoubleData& pos, const PrimitiveDoubleData& normal, const PrimitiveDoubleData& uv,
		std::optional<std::vector<std::int32_t>>&& primitiveMaterialIndeces);

	//頂点を参照する用のインデックスと取得重複のないIndexTupleのデータ
	std::pair<std::vector<std::size_t>, std::vector<IndexTuple>> GetIndecesAndIndexTuplesPair(std::vector<IndexTuple>&&);

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertecesFromIndexTuples(std::vector<IndexTuple>&&,
		PrimitiveDoubleData&& pos, PrimitiveDoubleData&& normal, PrimitiveDoubleData&& uv, const GlobalSettings& globalSettings);

	inline std::vector<TrianglePolygonIndex> GetTrianglePolygonIndeces(std::vector<std::size_t>&&);

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings);

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);


	struct DoubleTuplesAndIndeces {
		std::vector<std::tuple<double, double, double>> returnDoubleTuples{};
		std::vector<std::int64_t> primitiveIndeces{};
	};

	inline DoubleTuplesAndIndeces GetDoubleTuplesAndIndeces(std::vector<double>&&);

	template<typename DataType>
	struct IndecedData {
		using DataValueType = typename DataType::value_type;

		std::vector<std::int32_t> primitiveIndeces{};
		DataType data{};
	};

	template<typename DataType>
	IndecedData<DataType> GetIndecedData(DataType&& data);

	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&&, std::vector<std::int32_t>&& primitiveIndeces);

	template<typename DataType>
	IndecedData<DataType> UpdateIndeces(IndecedData<DataType>&&, const std::vector<std::int32_t>& primitiveIndeces);

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

	//
	IndecedData<std::vector<Int32Tuple<3>>> GetIndexTupleIndecedData(
		const IndecedData<std::vector<DoubleTuple<3>>>& positionIndecedData,
		const IndecedData<std::vector<DoubleTuple<3>>>& normalIndecedData,
		const IndecedData<std::vector<DoubleTuple<2>>>& uvIndecedData
		);

	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVerteces(std::vector<Int32Tuple<3>>&& indexTuples,
		IndecedData<std::vector<DoubleTuple<3>>>&& positionIndecedData,
		IndecedData<std::vector<DoubleTuple<3>>>&& normalIndecedData,
		IndecedData<std::vector<DoubleTuple<2>>>&& uvIndecedData,
		const GlobalSettings& globalSettings
	);
	
	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> GetTrianglePolygonAndMaterialIndexPairs(
		std::vector<std::int32_t>&& indeces, std::vector<std::int32_t>&& materialIndeces);


	//
	//以下、実装
	//



	std::pair<std::vector<IndexTuple>, std::vector<std::int32_t>> GetIndexTuplesAndIndexPolygonInformationPair(const std::vector<std::int32_t>& primitiveIndeces,
		const PrimitiveDoubleData& pos, const PrimitiveDoubleData& normal, const PrimitiveDoubleData& uv, std::optional<std::vector<std::int32_t>>&& primitiveMaterialIndeces)
	{
		std::vector<IndexTuple> indexTuples{};
		std::vector<std::int32_t> indexTupleInfos{};

		auto getPosIndex = std::bind(GetPrimitiveDoubleDataIndex, pos, primitiveIndeces, std::placeholders::_1);
		auto getNormalIndex = std::bind(GetPrimitiveDoubleDataIndex, normal, primitiveIndeces, std::placeholders::_1);
		auto getUVIndex = std::bind(GetPrimitiveDoubleDataIndex, uv, primitiveIndeces, std::placeholders::_1);

		auto pushBackIndexTuple = [&indexTuples, &getPosIndex, &getNormalIndex, &getUVIndex](std::size_t index) {
			indexTuples.emplace_back(std::make_tuple(getPosIndex(index), getNormalIndex(index), getUVIndex(index)));
		};

		//マテリアルが単一の場合は常に0
		auto getMaterialIndex = [&primitiveMaterialIndeces](std::size_t polygonIndex) {
			return (primitiveMaterialIndeces && primitiveMaterialIndeces.value().size() > 1) ? primitiveMaterialIndeces.value()[polygonIndex] : 0;
		};

		std::size_t i = 0;
		std::size_t j = 0;
		std::size_t polygonIndex = 0;
		while (i < primitiveIndeces.size())
		{
			j = i + 1;
			do {

				auto materialIndex = getMaterialIndex(polygonIndex);

				try {

					pushBackIndexTuple(i);
					pushBackIndexTuple(j);
					pushBackIndexTuple(j + 1);

					indexTupleInfos.push_back(materialIndex);

				}
				catch (std::out_of_range) {};

				j++;
			} while (j < primitiveIndeces.size() && primitiveIndeces[j] >= 0);
			i = j + 1;
			polygonIndex++;
		}

		return std::make_pair(std::move(indexTuples), std::move(indexTupleInfos));
	}

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

	std::size_t FBXL::GetPrimitiveDoubleDataIndex(const PrimitiveDoubleData& primitiveDoubleData, const std::vector<std::int32_t>& primitiveIndeces, std::int32_t index)
	{
		if (primitiveDoubleData.isByPolygonVertex)
			return std::visit(GetPrimitiveDoubleDataIndexVisitor{ index }, primitiveDoubleData.dataVarivant);
		else
			return std::visit(GetPrimitiveDoubleDataIndexVisitor{ primitiveIndeces.at(index) }, primitiveDoubleData.dataVarivant);
	}

	PrimitiveDoubleData GetVertecesPrimitiveDoubleData(const Node* geometryMesh)
	{
		PrimitiveDoubleData result{};

		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		result.dataVarivant = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		result.isByPolygonVertex = false;

		return result;
	}


	template<typename LayerElementInformation>
	PrimitiveDoubleData GetPrimitiveDoubleData(const Node* geometryMesh)
	{
		PrimitiveDoubleData result{};

		auto layerElementNode = GetSingleChildrenNode(geometryMesh, LayerElementInformation::layerElementName);
		//複数のおなじNodeがある場合がある
		if (!layerElementNode) {
			auto tmp = GetChildrenNodes(geometryMesh, LayerElementInformation::layerElementName);
			layerElementNode = tmp[0];
		}

		auto doubleDataNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::rawDoubleDataName);
		auto doubleData = GetProperty<std::vector<double>>(doubleDataNode.value(), 0).value();


		auto mappingInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "MappingInformationType");
		result.isByPolygonVertex = GetProperty<std::string>(mappingInformationTypeNode.value(), 0) == "ByPolygonVertex";

		auto referenceInformationTypeNode = GetSingleChildrenNode(layerElementNode.value(), "ReferenceInformationType");

		if (referenceInformationTypeNode && GetProperty<std::string>(referenceInformationTypeNode.value(), 0) == "Direct")
		{
			result.dataVarivant = std::move(doubleData);
		}
		else if (referenceInformationTypeNode && GetProperty<std::string>(referenceInformationTypeNode.value(), 0) == "IndexToDirect")
		{
			auto indexNode = GetSingleChildrenNode(layerElementNode.value(), LayerElementInformation::indexDataName);
			auto index = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();
			result.dataVarivant = std::make_pair(std::move(index), std::move(doubleData));
		}


		return result;
	}

	inline std::size_t FBXL::GetPrimitiveDoubleDataIndexVisitor::operator()(const RawData&)
	{
		auto i = (index < 0) ? -index - 1 : index;
		return i;
	}

	inline std::size_t FBXL::GetPrimitiveDoubleDataIndexVisitor::operator()(const IndexAndRawDataPair& indexAndRawDataPair)
	{
		auto i = (index < 0) ? -index - 1 : index;
		return indexAndRawDataPair.first.at(i);
	}

	std::pair<std::vector<std::size_t>, std::vector<IndexTuple>> GetIndecesAndIndexTuplesPair(std::vector<IndexTuple>&& indexTuples)
	{
		auto indexTuplesResult = indexTuples;

		{
			std::sort(indexTuplesResult.begin(), indexTuplesResult.end());
			auto iter = std::unique(indexTuplesResult.begin(), indexTuplesResult.end());
			indexTuplesResult.erase(iter, indexTuplesResult.end());
		}

		std::vector<std::size_t> primitiveIndeces(indexTuples.size());
		for (std::size_t i = 0; i < indexTuples.size(); i++)
		{
			auto iter = std::lower_bound(indexTuplesResult.begin(), indexTuplesResult.end(), indexTuples[i]);
			primitiveIndeces[i] = iter - indexTuplesResult.begin();
		}

		return std::make_pair(std::move(primitiveIndeces), std::move(indexTuplesResult));
	}



	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& vertices, const std::vector<std::int32_t>& primitiveIndeces, std::size_t index)
	{
		auto i = (primitiveIndeces.at(index) < 0) ? -primitiveIndeces.at(index) - 1 : primitiveIndeces.at(index);

		return CreateVector3DPolicy::Create(
			vertices.at(i * 3),
			vertices.at(i * 3 + 1),
			vertices.at(i * 3 + 2)
		);
	}

	template<typename Vector2D, typename CreateVector2DPolicy>
	inline Vector2D CreateVector2DFromIndexVisitor<Vector2D, CreateVector2DPolicy>::operator()(const RawData& rawData)
	{
		//
		//マイナス？？？
		//今のところuvのしか使わない
		//
		return CreateVector2DPolicy::Create(rawData.at(index * 2), -rawData.at(index * 2 + 1));
	}

	template<typename Vector2D, typename CreateVector2DPolicy>
	inline Vector2D CreateVector2DFromIndexVisitor<Vector2D, CreateVector2DPolicy>::operator()(const IndexAndRawDataPair& indexAndRawData)
	{
		//
		//マイナス？？？
		//今のところuvのしか使わない
		//
		return  CreateVector2DPolicy::Create(indexAndRawData.second.at(index * 2), -indexAndRawData.second.at(index * 2 + 1));
	}



	template<typename Vector3D, typename CreateVector3DPolicy>
	inline Vector3D CreateVector3DFromIndexVisitor<Vector3D, CreateVector3DPolicy>::operator()(const RawData& rawData)
	{
		return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
			rawData.at(index * 3),
			rawData.at(index * 3 + 1),
			rawData.at(index * 3 + 2),
			globalSettings
		);
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	inline Vector3D CreateVector3DFromIndexVisitor<Vector3D, CreateVector3DPolicy>::operator()(const IndexAndRawDataPair& indexAndRawDataPair)
	{
		return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
			indexAndRawDataPair.second.at(index * 3),
			indexAndRawDataPair.second.at(index * 3 + 1),
			indexAndRawDataPair.second.at(index * 3 + 2),
			globalSettings
		);
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertecesFromIndexTuples(std::vector<IndexTuple>&& indexTuples, PrimitiveDoubleData&& pos, PrimitiveDoubleData&& normal, PrimitiveDoubleData&& uv, const GlobalSettings& globalSettings)
	{
		std::vector<Vertex<Vector2D, Vector3D>> result{};
		result.reserve(indexTuples.size());

		auto getPosition = [&globalSettings, &pos](std::size_t i) {
			return std::visit(CreateVector3DFromIndexVisitor<Vector3D, CreateVector3DPolicy>{ globalSettings, i }, pos.dataVarivant);
		};
		auto getNormal = [&globalSettings, &normal](std::size_t i) {
			return std::visit(CreateVector3DFromIndexVisitor<Vector3D, CreateVector3DPolicy>{ globalSettings, i }, normal.dataVarivant);
		};
		auto getUV = [&globalSettings, &uv](std::size_t i) {
			return std::visit(CreateVector2DFromIndexVisitor<Vector2D, CreateVector2DPolicy>{ globalSettings, i }, uv.dataVarivant);
		};

		for (auto&& it : indexTuples)
			result.emplace_back(Vertex<Vector2D, Vector3D>{ getPosition(std::get<0>(it)), getNormal(std::get<1>(it)), getUV(std::get<2>(it)) });

		return result;
	}

	inline std::vector<TrianglePolygonIndex> GetTrianglePolygonIndeces(std::vector<std::size_t>&& primitiveIndeces)
	{
		std::vector<TrianglePolygonIndex> result(primitiveIndeces.size() / 3);

		for (std::size_t i = 0; i < result.size(); i++)
			result[i] = std::make_tuple(primitiveIndeces[i * 3], primitiveIndeces[i * 3 + 1], primitiveIndeces[i * 3 + 2]);

		return result;
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings)
	{
		{
			auto [indeces, indexIndeces, materialIndeces] = GetIndecesAndIndexIndecesAndMaterialIndeces(geometryMesh);
			auto positionIndecedData = GetPositionIndecedData(geometryMesh, indeces, indexIndeces);
			auto normalIndecedData = GetLayerElementIndecedData<LayerElementNormalInformation>(geometryMesh, indeces, indexIndeces);
			auto uvIndecedData = GetLayerElementIndecedData<LayerElementUVInformation>(geometryMesh, indeces, indexIndeces);

			auto indexTuples = GetIndexTupleIndecedData(positionIndecedData, normalIndecedData, uvIndecedData);
			auto verteces = GetVerteces<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(
				std::move(indexTuples.data), std::move(positionIndecedData), std::move(normalIndecedData), std::move(uvIndecedData), globalSettings);
			auto indexPairs = GetTrianglePolygonAndMaterialIndexPairs(std::move(indexTuples.primitiveIndeces), std::move(materialIndeces));

			//たぶん動いている
		}


		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();


		auto verticesPrimitiveData = GetVertecesPrimitiveDoubleData(geometryMesh);
		auto normalsPrimitiveData = GetPrimitiveDoubleData<LayerElementNormalInformation>(geometryMesh);
		auto uvsPrimitveData = GetPrimitiveDoubleData<LayerElementUVInformation>(geometryMesh);
		auto materialIndeces = GetPrimitiveMaterialIndeces(geometryMesh);

		auto [indexTuples, polygonMaterialIndeces] = GetIndexTuplesAndIndexPolygonInformationPair(indeces, verticesPrimitiveData, normalsPrimitiveData, uvsPrimitveData, std::move(materialIndeces));
		auto [indexTupleIndeces, indexTuples2] = GetIndecesAndIndexTuplesPair(std::move(indexTuples));

		auto vertices = GetVertecesFromIndexTuples<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(
			std::move(indexTuples2), std::move(verticesPrimitiveData), std::move(normalsPrimitiveData), std::move(uvsPrimitveData), globalSettings);

		auto trianglePolygonIndeces = GetTrianglePolygonIndeces(std::move(indexTupleIndeces));

		std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> indexAndMaterialIndexPairs(trianglePolygonIndeces.size());
		for (std::size_t i = 0; i < indexAndMaterialIndexPairs.size(); i++)
			indexAndMaterialIndexPairs[i] = std::make_pair(std::move(trianglePolygonIndeces[i]), polygonMaterialIndeces[i]);

		return { std::move(indexAndMaterialIndexPairs), std::move(vertices) };
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

	inline DoubleTuplesAndIndeces GetDoubleTuplesAndIndeces(std::vector<double>&& data)
	{
		std::vector<std::tuple<double, double, double>> returnDoubleTuples{};
		returnDoubleTuples.reserve(data.size() / 3);
		std::vector<std::int64_t> primitiveIndeces(data.size() / 3);

		for (std::size_t i = 0; i < data.size() / 3; i++)
			returnDoubleTuples.emplace_back(std::make_tuple(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));

		std::vector<std::tuple<double, double, double>> doubleTuples{ returnDoubleTuples };
		//並び変えて重複削除
		{
			std::sort(returnDoubleTuples.begin(), returnDoubleTuples.end());
			auto iter = std::unique(returnDoubleTuples.begin(), returnDoubleTuples.end());
			returnDoubleTuples.erase(iter, returnDoubleTuples.end());
		}

		for (std::size_t i = 0; i < primitiveIndeces.size() / 3; i++)
		{
			auto iter = std::lower_bound(returnDoubleTuples.begin(), returnDoubleTuples.end(), doubleTuples[i]);
			primitiveIndeces[i] = iter - returnDoubleTuples.begin();
		}

		return { std::move(returnDoubleTuples),std::move(primitiveIndeces) };
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

	IndecedData<std::vector<Int32Tuple<3>>> FBXL::GetIndexTupleIndecedData(const IndecedData<std::vector<DoubleTuple<3>>>& positionIndecedData, const IndecedData<std::vector<DoubleTuple<3>>>& normalIndecedData, const IndecedData<std::vector<DoubleTuple<2>>>& uvIndecedData)
	{
		std::vector<Int32Tuple<3>> int32Tuples{};
		int32Tuples.reserve(positionIndecedData.primitiveIndeces.size());

		for (std::size_t i = 0; i < positionIndecedData.primitiveIndeces.size(); i++)
			int32Tuples.emplace_back(positionIndecedData.primitiveIndeces[i], normalIndecedData.primitiveIndeces[i], uvIndecedData.primitiveIndeces[i]);

		return GetIndecedData(std::move(int32Tuples));
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVerteces(std::vector<Int32Tuple<3>>&& indexTuples, 
		IndecedData<std::vector<DoubleTuple<3>>>&& positionIndecedData, IndecedData<std::vector<DoubleTuple<3>>>&& normalIndecedData, IndecedData<std::vector<DoubleTuple<2>>>&& uvIndecedData,
		const GlobalSettings& globalSettings)
	{
		auto getPosition = [&positionIndecedData, &globalSettings](std::int32_t i) -> Vector3D {
			auto j = positionIndecedData.primitiveIndeces[i];
			return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
				std::get<0>(positionIndecedData.data[j]), std::get<1>(positionIndecedData.data[j]), std::get<2>(positionIndecedData.data[j]), globalSettings);
		};

		auto getNormal = [&normalIndecedData, &globalSettings](std::int32_t i)->Vector3D {
			auto j = normalIndecedData.primitiveIndeces[i];
			return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
				std::get<0>(normalIndecedData.data[j]), std::get<1>(normalIndecedData.data[j]), std::get<2>(normalIndecedData.data[j]), globalSettings);
		};

		auto getUV = [&uvIndecedData, &globalSettings](std::int32_t i)->Vector2D {
			auto j = uvIndecedData.primitiveIndeces[i];
			return CreateVector2DPolicy::Create(std::get<0>(uvIndecedData.data[j]), -std::get<1>(uvIndecedData.data[j]));
		};

		std::vector<Vertex<Vector2D, Vector3D>> result{};
		result.reserve(indexTuples.size());

		for (std::size_t i = 0; i < indexTuples.size(); i++)
			result.emplace_back(Vertex<Vector2D, Vector3D>{ getPosition(i), getNormal(i), getUV(i) });

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
}
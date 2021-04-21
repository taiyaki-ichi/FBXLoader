#pragma once
#include"GeometryStruct.hpp"
#include"../../GlobalSettings/GlobalSettingsStruct.hpp"
#include<functional>
#include<stdexcept>
#include<tuple>

namespace FBXL
{
	//GeometyMeshNodeからマテリアルのインデックス配列を取得
	inline std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh);

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

	std::size_t GetPrimitiveDoubleDataIndex(const PrimitiveDoubleData&, const std::vector<std::int32_t>& indeces, std::int32_t index);

	PrimitiveDoubleData GetVertecesPrimitiveDoubleData(const Node* geometryMesh);

	struct LayerElementNormalInformation {
		constexpr static char layerElementName[] = "LayerElementNormal";
		constexpr static char rawDoubleDataName[] = "Normals";
		constexpr static char indexDataName[] = "NormalsIndex";

		using DataType = std::tuple<double, double, double>;
	};

	struct LayerElementUVInformation {
		constexpr static char layerElementName[] = "LayerElementUV";
		constexpr static char rawDoubleDataName[] = "UV";
		constexpr static char indexDataName[] = "UVIndex";

		using DataType = std::tuple<double, double>;
	};

	template<typename LayerElementInformation>
	PrimitiveDoubleData GetPrimitiveDoubleData(const Node* geometryMesh);

	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& vertices, const std::vector<std::int32_t>& indeces, std::size_t index);

	struct GetPrimitiveDoubleDataIndexVisitor {
		std::int32_t index;
		std::size_t operator()(const RawData&);
		std::size_t operator()(const IndexAndRawDataPair&);
	};

	template<typename Vector2D,typename CreateVector2DPolicy>
	struct CreateVector2DFromIndexVisitor {
		const GlobalSettings& globalSettings;
		std::size_t index;

		Vector2D operator()(const RawData&);
		Vector2D operator()(const IndexAndRawDataPair&);
	};

	template<typename Vector3D,typename CreateVector3DPolicy>
	struct CreateVector3DFromIndexVisitor {
		const GlobalSettings& globalSettings;
		std::size_t index;

		Vector3D operator()(const RawData&);
		Vector3D operator()(const IndexAndRawDataPair&);
	};

	//pos,normal,uvの順
	using IndexTuple = std::tuple<std::size_t, std::size_t, std::size_t>;

	std::pair<std::vector<IndexTuple>, std::vector<std::int32_t>> GetIndexTuplesAndIndexPolygonInformationPair(const std::vector<std::int32_t>& indeces,
		const PrimitiveDoubleData& pos, const PrimitiveDoubleData& normal, const PrimitiveDoubleData& uv,
		std::optional<std::vector<std::int32_t>>&& materialIndeces);

	//頂点を参照する用のインデックスと取得重複のないIndexTupleのデータ
	std::pair<std::vector<std::size_t>, std::vector<IndexTuple>> GetIndecesAndIndexTuplesPair(std::vector<IndexTuple>&&);

	template<typename Vector2D,typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertecesFromIndexTuples(std::vector<IndexTuple>&&,
		PrimitiveDoubleData&& pos, PrimitiveDoubleData&& normal, PrimitiveDoubleData&& uv, const GlobalSettings& globalSettings);

	inline std::vector<TrianglePolygonIndex> GetTrianglePolygonIndeces(std::vector<std::size_t>&&);

	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>>
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings);

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);


	struct DoubleTuplesAndIndeces {
		std::vector<std::tuple<double, double, double>> returnDoubleTuples{};
		std::vector<std::int64_t> indeces{};
	};

	inline DoubleTuplesAndIndeces GetDoubleTuplesAndIndeces(std::vector<double>&&);



	/*
	template<typename DataType>
	struct IndecedData {

		//頂点のインデックス
		std::optional<const std::vector<std::int64_t>*> vertexIndeces = std::nullopt;

		//それ以下のインデックスたち
		std::vector<std::vector<std::int64_t>> indeces{};

		//実際のデータ
		std::remove_reference_t<DataType> data{};
	};

	//DataTypeからIndeceｄDataの取得
	//重複している要素は省かれる
	template<typename DataType>
	IndecedData<DataType> GetIndecedDataFromDataType(DataType&&);

	template<typename LayerElementInformation>
	IndecedData<typename LayerElementInformation::DataType> GetIndecedDataFromLayerElement(
		const Node* geometryMesh, const std::vector<std::int32_t>& vertexIndeces);
		*/

	//
	//以下、実装
	//



	std::pair<std::vector<IndexTuple>, std::vector<std::int32_t>> GetIndexTuplesAndIndexPolygonInformationPair(const std::vector<std::int32_t>& indeces,
		const PrimitiveDoubleData& pos, const PrimitiveDoubleData& normal, const PrimitiveDoubleData& uv, std::optional<std::vector<std::int32_t>>&& materialIndeces)
	{
		std::vector<IndexTuple> indexTuples{};
		std::vector<std::int32_t> indexTupleInfos{};

		auto getPosIndex = std::bind(GetPrimitiveDoubleDataIndex, pos, indeces, std::placeholders::_1);
		auto getNormalIndex = std::bind(GetPrimitiveDoubleDataIndex, normal, indeces, std::placeholders::_1);
		auto getUVIndex = std::bind(GetPrimitiveDoubleDataIndex, uv, indeces, std::placeholders::_1);

		auto pushBackIndexTuple = [&indexTuples, &getPosIndex, &getNormalIndex, &getUVIndex](std::size_t index) {
			indexTuples.emplace_back(std::make_tuple(getPosIndex(index), getNormalIndex(index), getUVIndex(index)));
		};

		//マテリアルが単一の場合は常に0
		auto getMaterialIndex = [&materialIndeces](std::size_t polygonIndex) {
			return (materialIndeces && materialIndeces.value().size() > 1) ? materialIndeces.value()[polygonIndex] : 0;
		};

		std::size_t i = 0;
		std::size_t j = 0;
		std::size_t polygonIndex = 0;
		while (i < indeces.size())
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
			} while (j < indeces.size() && indeces[j] >= 0);
			i = j + 1;
			polygonIndex++;
		}

		return std::make_pair(std::move(indexTuples), std::move(indexTupleInfos));
	}

	inline std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh)
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

	std::size_t FBXL::GetPrimitiveDoubleDataIndex(const PrimitiveDoubleData& primitiveDoubleData, const std::vector<std::int32_t>& indeces,std::int32_t index)
	{
		if (primitiveDoubleData.isByPolygonVertex)
			return std::visit(GetPrimitiveDoubleDataIndexVisitor{ index }, primitiveDoubleData.dataVarivant);
		else
			return std::visit(GetPrimitiveDoubleDataIndexVisitor{ indeces.at(index) }, primitiveDoubleData.dataVarivant);
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

		//
		//auto hoge = GetIndecedDataFromDataType(doubleData);
		//DoubleTuple<3> a{};
		//
		

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

		std::vector<std::size_t> indeces(indexTuples.size());
		for (std::size_t i = 0; i < indexTuples.size(); i++)
		{
			auto iter = std::lower_bound(indexTuplesResult.begin(), indexTuplesResult.end(), indexTuples[i]);
			indeces[i] = iter - indexTuplesResult.begin();
		}

		return std::make_pair(std::move(indeces), std::move(indexTuplesResult));
	}



	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& vertices, const std::vector<std::int32_t>& indeces, std::size_t index)
	{
		auto i = (indeces.at(index) < 0) ? -indeces.at(index) - 1 : indeces.at(index);

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
			return std::visit(CreateVector3DFromIndexVisitor<Vector3D, CreateVector3DPolicy>{ globalSettings,i }, normal.dataVarivant);
		};
		auto getUV = [&globalSettings, &uv](std::size_t i) {
			return std::visit(CreateVector2DFromIndexVisitor<Vector2D, CreateVector2DPolicy>{ globalSettings, i }, uv.dataVarivant);
		};

		for (auto&& it : indexTuples)
			result.emplace_back(Vertex<Vector2D, Vector3D>{ getPosition(std::get<0>(it)), getNormal(std::get<1>(it)), getUV(std::get<2>(it)) });

		return result;
	}

	inline std::vector<TrianglePolygonIndex> GetTrianglePolygonIndeces(std::vector<std::size_t>&& indeces)
	{
		std::vector<TrianglePolygonIndex> result(indeces.size() / 3);

		for (std::size_t i = 0; i < result.size(); i++)
			result[i] = std::make_tuple(indeces[i * 3], indeces[i * 3 + 1], indeces[i * 3 + 2]);
		
		return result;
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::tuple<std::vector<std::pair<TrianglePolygonIndex,std::int64_t>>, std::vector<Vertex<Vector2D, Vector3D>>> 
		GetIndecesAndVertecesAndPolygonInfomationTuple(const Node* geometryMesh, const GlobalSettings& globalSettings)
	{
		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

	
		auto verticesPrimitiveData = GetVertecesPrimitiveDoubleData(geometryMesh);
		auto normalsPrimitiveData = GetPrimitiveDoubleData<LayerElementNormalInformation>(geometryMesh);
		auto uvsPrimitveData = GetPrimitiveDoubleData<LayerElementUVInformation>(geometryMesh);
		auto materialIndeces = GetMaterialIndeces(geometryMesh);

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

		auto [indeces, vertices] =
			GetIndecesAndVertecesAndPolygonInfomationTuple<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh, globalSettings);


		return std::make_pair(GeometryMesh<Vector2D, Vector3D>{indeces, vertices }, index);
	
	}

	inline DoubleTuplesAndIndeces GetDoubleTuplesAndIndeces(std::vector<double>&& data)
	{
		std::vector<std::tuple<double, double, double>> returnDoubleTuples{};
		returnDoubleTuples.reserve(data.size() / 3);
		std::vector<std::int64_t> indeces(data.size() / 3);

		for (std::size_t i = 0; i < data.size() / 3; i++)
			returnDoubleTuples.emplace_back(std::make_tuple(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));

		std::vector<std::tuple<double, double, double>> doubleTuples{ returnDoubleTuples };
		//並び変えて重複削除
		{
			std::sort(returnDoubleTuples.begin(), returnDoubleTuples.end());
			auto iter = std::unique(returnDoubleTuples.begin(), returnDoubleTuples.end());
			returnDoubleTuples.erase(iter, returnDoubleTuples.end());
		}

		for (std::size_t i = 0; i < indeces.size() / 3; i++)
		{
			auto iter = std::lower_bound(returnDoubleTuples.begin(), returnDoubleTuples.end(), doubleTuples[i]);
			indeces[i] = iter - returnDoubleTuples.begin();
		}

		return { std::move(returnDoubleTuples),std::move(indeces) };
	}

	/*

	template<typename DataType>
	IndecedData<DataType> GetIndecedDataFromDataType(DataType&& data)
	{
		IndecedData<DataType> result{};

		auto returnData = data;
		std::vector<std::int64_t> indeces(data.size());

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

		result.indeces.emplace_back(std::move(indeces));
		result.data = std::move(returnData);

		return result;
	}
	*/
	
}
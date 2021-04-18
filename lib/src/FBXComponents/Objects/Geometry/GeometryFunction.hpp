#pragma once
#include"GeometryStruct.hpp"
#include"../../GlobalSettings/GlobalSettingsStruct.hpp"
#include<functional>
#include<stdexcept>

namespace FBXL
{
	//GeometryMeshのノードからGeometryMeshとそのインデックスの取得
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);



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

	template<typename Vector2D, typename CreateVector2DPolicy>
	Vector2D GetVector2DFromPrimitiveData(const PrimitiveDoubleData&, const std::vector<std::int32_t>&, const GlobalSettings&, std::int32_t);

	template<typename Vector3D, typename CreateVector3DPolicy>
	Vector3D GetVector3DFromPrimitiveData(const PrimitiveDoubleData&, const std::vector<std::int32_t>&, const GlobalSettings&, std::int32_t);

	PrimitiveDoubleData GetVertecesPrimitiveDoubleData(const Node* geometryMesh);

	struct NormalDoubleDataInformation {
		constexpr static char layerElementName[] = "LayerElementNormal";
		constexpr static char rawDoubleDataName[] = "Normals";
		constexpr static char indexDataName[] = "NormalsIndex";
	};

	struct UVDoubleDataInformation {
		constexpr static char layerElementName[] = "LayerElementUV";
		constexpr static char rawDoubleDataName[] = "UV";
		constexpr static char indexDataName[] = "UVIndex";
	};

	template<typename DoubleDataInformation>
	PrimitiveDoubleData GetPrimitiveDoubleData(const Node* geometryMesh);

	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& verteces, const std::vector<std::int32_t>& indeces, std::size_t index);

	template<typename Vector2D, typename CreateVector2DPolicy>
	struct GetVector2DFromDoubleDataVariantVisitor {

		const GlobalSettings& globalSettings;
		std::int32_t index;

		Vector2D operator()(const RawData&);
		Vector2D operator()(const IndexAndRawDataPair&);
	};

	template<typename Vector3D, typename CreateVector3DPolicy>
	struct GetVector3DFromDoubleDataVariantVisitor {

		const GlobalSettings& globalSettings;
		std::int32_t index;

		Vector3D operator()(const RawData&);
		Vector3D operator()(const IndexAndRawDataPair&);
	};


	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int32_t>>
		GetTrianglePolygonAndMaterialNumberPairs(const Node* geometryMesh, const GlobalSettings& globalSettings);


	template<typename Vector2D, typename Vector3D>
	GeometryMesh<Vector2D,Vector3D> GetGeometryMeshFromTrianglePolygonAndMaterialNumberPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int32_t>>&& pair);


	//
	//以下、実装
	//

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(geormetryMesh.name == "Geometry" && GetProperty<std::string>(&geormetryMesh, 2) == "Mesh");

		GeometryMesh<Vector2D, Vector3D> result{};

		auto index = GetProperty<std::int64_t>(&geormetryMesh, 0).value();

		auto vertexAndMaterialNumPairs = GetTrianglePolygonAndMaterialNumberPairs<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh, globalSettings);
		auto gm = GetGeometryMeshFromTrianglePolygonAndMaterialNumberPairs(std::move(vertexAndMaterialNumPairs));

		return std::make_pair(std::move(gm), index);
	
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


	template<typename Vector2D, typename CreateVector2DPolicy>
	Vector2D GetVector2DFromPrimitiveData(const PrimitiveDoubleData& primitiveDoubleData, const std::vector<std::int32_t>& indeces, const GlobalSettings& globalSettings, std::int32_t index)
	{
		if (primitiveDoubleData.isByPolygonVertex)
		{
			return std::visit(
				GetVector2DFromDoubleDataVariantVisitor<Vector2D, CreateVector2DPolicy>{globalSettings, index},
				primitiveDoubleData.dataVarivant
			);
		}
		else
		{
			return std::visit(
				GetVector2DFromDoubleDataVariantVisitor<Vector2D, CreateVector2DPolicy>{globalSettings, indeces[index]},
				primitiveDoubleData.dataVarivant
			);
		}

	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	Vector3D GetVector3DFromPrimitiveData(const PrimitiveDoubleData& primitiveDoubleData, const std::vector<std::int32_t>& indeces, const GlobalSettings& globalSettings, std::int32_t index)
	{
		if (primitiveDoubleData.isByPolygonVertex)
		{
			return std::visit(
				GetVector3DFromDoubleDataVariantVisitor<Vector3D, CreateVector3DPolicy>{globalSettings, index},
				primitiveDoubleData.dataVarivant
			);
		}
		else
		{
			return std::visit(
				GetVector3DFromDoubleDataVariantVisitor<Vector3D, CreateVector3DPolicy>{globalSettings, indeces[index]},
				primitiveDoubleData.dataVarivant
			);
		}
	}


	PrimitiveDoubleData GetVertecesPrimitiveDoubleData(const Node* geometryMesh)
	{
		PrimitiveDoubleData result{};

		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		result.dataVarivant = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		result.isByPolygonVertex = false;

		return result;
	}

	template<typename DoubleDataInformation>
	PrimitiveDoubleData GetPrimitiveDoubleData(const Node* geometryMesh)
	{
		PrimitiveDoubleData result{};

		auto layerElementNode = GetSingleChildrenNode(geometryMesh, DoubleDataInformation::layerElementName);
		//複数のおなじNodeがある場合がある
		if (!layerElementNode) {
			auto tmp = GetChildrenNodes(geometryMesh, DoubleDataInformation::layerElementName);
			layerElementNode = tmp[0];
		}

		auto doubleDataNode = GetSingleChildrenNode(layerElementNode.value(), DoubleDataInformation::rawDoubleDataName);
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
			auto indexNode = GetSingleChildrenNode(layerElementNode.value(), DoubleDataInformation::indexDataName);
			auto index = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();
			result.dataVarivant = std::make_pair(std::move(index), std::move(doubleData));
		}

		return result;
	}

	template<typename CreateVector3DPolicy>
	auto GetVertexFromIndex(const std::vector<double>& verteces, const std::vector<std::int32_t>& indeces, std::size_t index)
	{
		auto i = (indeces.at(index) < 0) ? -indeces.at(index) - 1 : indeces.at(index);

		return CreateVector3DPolicy::Create(
			verteces.at(i * 3),
			verteces.at(i * 3 + 1),
			verteces.at(i * 3 + 2)
		);
	}


	template<typename Vector2D, typename CreateVector2DPolicy>
	inline Vector2D GetVector2DFromDoubleDataVariantVisitor<Vector2D, CreateVector2DPolicy>::operator()(const RawData& rawData)
	{
		auto i = (index < 0) ? -index - 1 : index;

		//
		//マイナス？？？
		//
		return CreateVector2DPolicy::Create(rawData.at(i * 2), -rawData.at(i * 2 + 1));
	}

	template<typename Vector2D, typename CreateVector2DPolicy>
	inline Vector2D GetVector2DFromDoubleDataVariantVisitor<Vector2D, CreateVector2DPolicy>::operator()(const IndexAndRawDataPair& indexAndRawDataPair)
	{
		auto i = (index < 0) ? -index - 1 : index;

		//
		//マイナス？？？
		//
		return CreateVector2DPolicy::Create(
			indexAndRawDataPair.second.at(indexAndRawDataPair.first.at(i) * 2),
			-indexAndRawDataPair.second.at(indexAndRawDataPair.first.at(i) * 2 + 1)
		);
	}


	template<typename Vector3D, typename CreateVector3DPolicy>
	inline Vector3D GetVector3DFromDoubleDataVariantVisitor<Vector3D, CreateVector3DPolicy>::operator()(const RawData& rawData)
	{
		auto i = (index < 0) ? -index - 1 : index;

		return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(rawData.at(i * 3), rawData.at(i * 3 + 1), rawData.at(i * 3 + 2), globalSettings);
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	inline Vector3D GetVector3DFromDoubleDataVariantVisitor<Vector3D, CreateVector3DPolicy>::operator()(const IndexAndRawDataPair& indexAndRawDataPair)
	{
		auto i = (index < 0) ? -index - 1 : index;

		return CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(
			indexAndRawDataPair.second.at(indexAndRawDataPair.first.at(i) * 3),
			indexAndRawDataPair.second.at(indexAndRawDataPair.first.at(i) * 3 + 1),
			indexAndRawDataPair.second.at(indexAndRawDataPair.first.at(i) * 3 + 2),
			globalSettings
		);
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int32_t>>
		GetTrianglePolygonAndMaterialNumberPairs(const Node* geometryMesh, const GlobalSettings& globalSettings)
	{
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int32_t>> result{};

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

		auto vertices = GetVertecesPrimitiveDoubleData(geometryMesh);

		auto normals = GetPrimitiveDoubleData<NormalDoubleDataInformation>(geometryMesh);

		auto uvs = GetPrimitiveDoubleData<UVDoubleDataInformation>(geometryMesh);

		auto materialIndeces = GetMaterialIndeces(geometryMesh);

		auto getVertexVector3D = std::bind(GetVector3DFromPrimitiveData<Vector3D, CreateVector3DPolicy>, vertices, indeces, globalSettings, std::placeholders::_1);
		auto getNormalVector3D = std::bind(GetVector3DFromPrimitiveData<Vector3D, CreateVector3DPolicy>, normals, indeces, globalSettings, std::placeholders::_1);
		auto getUVVector2D = std::bind(GetVector2DFromPrimitiveData<Vector2D, CreateVector2DPolicy>, uvs, indeces, globalSettings, std::placeholders::_1);

		auto getMaterialIndex = [&materialIndeces](std::size_t polygonIndex) {
			return (materialIndeces && materialIndeces.value().size() > 1) ? materialIndeces.value()[polygonIndex] : 0;
		};

		auto getVertex = [&getVertexVector3D, &getNormalVector3D, &getUVVector2D, &getMaterialIndex](std::size_t index) {

			Vertex<Vector2D, Vector3D> vertex{};
			vertex.position = getVertexVector3D(index);
			vertex.normal = getNormalVector3D(index);
			vertex.uv = getUVVector2D(index);

			return vertex;
		};

		std::size_t i = 0;
		std::size_t j = 0;
		std::size_t polygonIndex = 0;
		TrianglePolygon<Vector2D,Vector3D> tmpTrianglePolygon{};
		while (i < indeces.size())
		{
			j = i + 1;
			do {

				auto materialIndex = getMaterialIndex(polygonIndex);

				try {

					tmpTrianglePolygon[0] = getVertex(i);
					tmpTrianglePolygon[1] = getVertex(j);
					tmpTrianglePolygon[2] = getVertex(j + 1);

					result.emplace_back(std::make_pair(std::move(tmpTrianglePolygon), materialIndex));
				}
				catch (std::out_of_range) {};
				
				j++;
			} while (j < indeces.size() && indeces[j] >= 0);
			i = j + 1;
			polygonIndex++;
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	GeometryMesh<Vector2D, Vector3D> GetGeometryMeshFromTrianglePolygonAndMaterialNumberPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int32_t>>&& pair)
	{
		GeometryMesh<Vector2D, Vector3D> result{};

		if (pair.size() == 0)
			return result;

		std::sort(pair.begin(), pair.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		result.trianglePolygons.resize(pair.size());
		result.materialRange.resize(pair[pair.size() - 1].second + 1);

		std::fill(result.materialRange.begin(), result.materialRange.end(), 0);

		for (std::size_t i = 0; i < pair.size(); i++) {
			result.trianglePolygons[i] = std::move(pair[i].first);

			result.materialRange[pair[i].second]++;
		}

		return result;
	}

}
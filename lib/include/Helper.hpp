#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include<cassert>
#include<algorithm>
#include"DataStruct.hpp"
#include"DefaultPolicyTraits.hpp"

#include<iostream>

namespace FBXL
{

	//node.mChildrenのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetChildrenNodes(const Node* modelMesh, const std::string& name);

	//単一の子ノードの取得
	//複数ある場合はnullopt
	std::optional<const Node*> GetSingleChildrenNode(const Node* modelMesh, const std::string& name);

	//Node.mPropertiesの情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index);

	//propertyのインデックス3つを指定しVector３Dを生成
	template<typename Vector3D, typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	template<typename Vector3D,typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index);



	//GeometyMeshNodeからマテリアルのインデックス配列を取得
	std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh);

	//GeometyMeshNodeから法線の配列を取得
	template<typename Vector3D,typename CreateVector3DPolicy>
	std::vector<Vector3D> GetNormals(const Node* geometryMesh);

	//GeometyMeshNodeからUVの取得
	template<typename Vector2D,typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh);

	//materialの指定がないver
	template<typename Vector2D, typename Vector3D>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertexStructArrayAndMaterialRange(
		std::vector<Vector3D>&& positions, std::vector<Vector3D>&& normals, std::vector<Vector2D>&& uv);

	//geometryMeshのノードからシンプルな頂点データなどを取得
	template<typename Vector2D,typename Vector3D>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh);

	//geometryMeshから取得したシンプルなデータを加工
	template<typename Vector2D,typename Vector3D>
	std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> 
		GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair);

	//modelMeshのノードからデータの取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>, std::int64_t> GetModelMesh(Node&& modelMesh);

	//GeometryMeshのノードからデータを取得
	template<typename Vector2D, typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh);

	//materialのノードからデータを取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>, std::int64_t> GetMaterial(Node&& modelMesh);

	//PrimitiveDataからObjectsの取得
	template<typename Vector2D, typename Vector3D,
		typename CreateVector2DPolicy = DefaultCreateVector2D<Vector2D>,
		typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
		std::optional<Objects<Vector2D, Vector3D>> GetObjects(Node&& objects);
	
	


	//
	//以下、関数テンプレートの実装
	//


	template<typename Vector3D,typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>,std::int64_t> GetModelMesh(Node&& modelMesh)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(modelMesh.name == "Model" && GetProperty<std::string>(&modelMesh, 2) == "Mesh");

		ModelMesh<Vector3D> result;

		auto index = GetProperty<std::int64_t>(&modelMesh, 0).value();

		auto prop70 = GetSingleChildrenNode(&modelMesh, "Properties70").value();
		
		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Transration", 4, 5, 6);
			if (vec)
				result.localTranslation = vec.value();
			else
				result.localTranslation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Rotation", 4, 5, 6);
			if (vec)
				result.localRotation = vec.value();
			else
				result.localRotation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Scaling", 4, 5, 6);
			if (vec)
				result.localScaling = vec.value();
			else
				result.localScaling = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		return std::make_pair(std::move(result), index);
	}
	

	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D,Vector3D>,std::int64_t> GetGeometryMesh(Node&& geormetryMesh)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(geormetryMesh.name == "Geometry" && GetProperty<std::string>(&geormetryMesh, 2) == "Mesh");

		GeometryMesh<Vector2D, Vector3D> result{};

		auto index = GetProperty<std::int64_t>(&geormetryMesh, 0).value();

		auto vertexAndMaterialNumPairs = GetVertexAndMaterialNumberPairs<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh);
		auto [vertices, materialRange] = GetVerticesAndMaterialRange(std::move(vertexAndMaterialNumPairs));
		result.verteces = std::move(vertices);
		result.materialRange = std::move(materialRange);

		return std::make_pair(std::move(result), index);
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>,std::int64_t> GetMaterial(Node&& modelMesh)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(modelMesh.name == "Material");

		Material<Vector3D> result;

		auto index = GetProperty<std::int64_t>(&modelMesh, 0).value();

		auto prop70 = GetSingleChildrenNode(&modelMesh, "Properties70").value();

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "DiffuseColor", 4, 5, 6);
			if (vec)
				result.diffuseColor = vec.value();
			else
				result.diffuseColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "DiffuseFactor", 4);
			if (fac)
				result.diffuseFactor = fac.value();
			else
				result.diffuseFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "AmbientColor", 4, 5, 6);
			if (vec)
				result.ambientColor = vec.value();
			else
				result.ambientColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "AmbientFactor", 4);
			if (fac)
				result.ambientFactor = fac.value();
			else
				result.ambientFactor = 0.0;
		}
		
		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "SpecularColor", 4, 5, 6);
			if (vec)
				result.specularColor = vec.value();
			else
				result.specularColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "SpecularFactor", 4);
			if (fac)
				result.specularFactor = fac.value();
			else
				result.specularFactor = 0.0;
		}

		{
			auto shi = GetProperities70Data<double>(prop70, "Shininess", 4);
			if (shi)
				result.shininess = shi.value();
			else
				result.shininess = 0.0;
		}

		{
			auto shiFac = GetProperities70Data<double>(prop70, "ShininessFactor", 4);
			if (shiFac)
				result.shininessFactor = shiFac.value();
			else
				result.shininessFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Emissive", 4, 5, 6);
			if (vec)
				result.emissive = vec.value();
			else
				result.emissive = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto f = GetProperities70Data<double>(prop70, "EmissiveFactor", 4);
			if (f)
				result.emissiveFactor = f.value();
			else
				result.emissiveFactor = 0.0;
		}

		{
			auto t = GetProperities70Data<double>(prop70, "Transparent", 4);
			if (t)
				result.transparent = t.value();
			else
				result.transparentFactor = 0.0;
		}

		{
			auto tf = GetProperities70Data<double>(prop70, "TransparentFactor", 4);
			if (tf)
				result.transparentFactor = tf.value();
			else
				result.transparentFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "ReflectionColor", 4, 5, 6);
			if (vec)
				result.reflectionColor = vec.value();
			else
				result.reflectionColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto rf = GetProperities70Data<double>(prop70, "ReflectionFactor", 4);
			if (rf)
				result.reflectionFactor = rf.value();
			else
				result.reflectionFactor = 0.0;
		}

		return std::make_pair(std::move(result), index);
	}


	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetProperty<T>(p.value(), index);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), xIndex, yIndex, zIndex);
		else
			return std::nullopt;
	}


	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh,std::size_t index)
	{
		if (std::holds_alternative<T>(modelMesh->properties[index]))
			return std::get<T>(modelMesh->properties[index]);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		auto x = GetProperty<double>(modelMesh, xIndex);
		auto y = GetProperty<double>(modelMesh, yIndex);
		auto z = GetProperty<double>(modelMesh, zIndex);

		if (x && y && z)
			return CreateVector3DPolicy::Create(x.value(), y.value(), z.value());
		else
			return std::nullopt;
	}




	template<typename Vector3D, typename CreateVector3DPolicy>
	std::vector<Vector3D> GetPositions(const Node* geometryMesh)
	{
		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices").value();
		auto vertices = GetProperty<std::vector<double>>(verticesNode, 0).value();

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex").value();
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode, 0).value();

		std::vector<Vector3D> result{};
		//少なくともインデックスの大きさ以上
		result.reserve(indeces.size());

		std::size_t i = 0;
		std::size_t j = 0;
		while (i < indeces.size())
		{
			j = i + 1;

			while (indeces[j + 1] >= 0)
			{
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[i] * 3], vertices[indeces[i] * 3 + 1], vertices[indeces[i] * 3 + 2]));
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j] * 3], vertices[indeces[j] * 3 + 1], vertices[indeces[j] * 3 + 2]));
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j + 1] * 3], vertices[indeces[j + 1] * 3 + 1], vertices[indeces[j + 1] * 3 + 2]));

				j++;
			}

			result.push_back(CreateVector3DPolicy::Create(vertices[indeces[i] * 3], vertices[indeces[i] * 3 + 1], vertices[indeces[i] * 3 + 2]));
			result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j] * 3], vertices[indeces[j] * 3 + 1], vertices[indeces[j] * 3 + 2]));
			result.push_back(CreateVector3DPolicy::Create(vertices[(-indeces[j + 1] - 1) * 3], vertices[(-indeces[j + 1] - 1) * 3 + 1], vertices[(-indeces[j + 1] - 1) * 3 + 2]));
			i = j + 2;
		}
		
		return result;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::vector<Vector3D> GetNormals(const Node* geometryMesh)
	{
		auto layerElementNormalNode = GetSingleChildrenNode(geometryMesh, "LayerElementNormal");

		auto modelMesh = GetSingleChildrenNode(layerElementNormalNode.value(), "Normals");

		auto vec = GetProperty<std::vector<double>>(modelMesh.value(), 0).value();
		std::vector<Vector3D> result{};
		result.reserve(vec.size() / 3);
		for (std::size_t i = 0; i < vec.size() / 3; i++)
			result.push_back(CreateVector3DPolicy::Create(vec[i * 3], vec[i * 3 + 1], vec[i * 3 + 2]));

		return result;
	}

	template<typename Vector2D, typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh)
	{
		auto layerElementUVNode = GetSingleChildrenNode(geometryMesh, "LayerElementUV");

		auto uvNode = GetSingleChildrenNode(layerElementUVNode.value(), "UV");
		auto uvIndexNode = GetSingleChildrenNode(layerElementUVNode.value(), "UVIndex");

		auto uv = GetProperty<std::vector<double>>(uvNode.value(), 0).value();
		auto uvIndex = GetProperty<std::vector<std::int32_t>>(uvIndexNode.value(), 0).value();

		std::vector<Vector2D> result{};
		result.reserve(uvIndex.size());
		for (std::size_t i = 0; i < uvIndex.size(); i++)
			result.push_back(CreateVector2DPolicy::Create(uv[uvIndex[i] * 2], uv[uvIndex[i] * 2 + 1]));

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertexStructArrayAndMaterialRange(std::vector<Vector3D>&& positions, std::vector<Vector3D>&& normals, std::vector<Vector2D>&& uv)
	{
		assert(positions.size() == normals.size() == uv.size());

		std::vector<Vertex<Vector2D, Vector3D>> result(positions.size());

		for (std::size_t i = 0; i < result.size(); i++)
		{
			result[i].position = std::move(positions[i]);
			result[i].normal = std::move(normals[i]);
			result[i].uv = std::move(uv[i]);
		}

		return result;
	}


	template<typename Vector2D, typename Vector3D,
		typename CreateVector2DPolicy=DefaultCreateVector2D<Vector2D>,
		typename CreateVector3DPolicy=DefaultCreateVector3D<Vector3D>
	>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh)
	{
		std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> result{};

		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		auto vertices = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

		auto normals = GetNormals<Vector3D,CreateVector3DPolicy>(geometryMesh);
		auto uvs = GetUVs<Vector2D, CreateVector2DPolicy>(geometryMesh);

		auto materialIndeces = GetMaterialIndeces(geometryMesh);

		auto pushBack = [&vertices, &normals, &uvs, &result, &materialIndeces](std::size_t index1, std::size_t index2, std::size_t index3, std::size_t offset) {
			Vertex<Vector2D, Vector3D> tmpVec;
			tmpVec.position = CreateVector3DPolicy::Create(vertices[index1], vertices[index2], vertices[index3]);
			tmpVec.normal = normals[offset];
			tmpVec.uv = uvs[offset];
			if (materialIndeces && materialIndeces.value().size() > 1)
				result.emplace_back(std::make_pair(std::move(tmpVec), materialIndeces.value()[offset]));
			else
				result.emplace_back(std::make_pair(std::move(tmpVec), 0));
		};

		std::size_t offset = 0;
		std::size_t i = 0;
		std::size_t j = 0;
		Vertex<Vector2D, Vector3D> tmpVec;
		while (i < indeces.size())
		{
			j = i + 1;

			while (indeces[j + 1] >= 0)
			{
				pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
				pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
				pushBack(indeces[j + 1] * 3, indeces[j + 1] * 3 + 1, indeces[j + 1] * 3 + 2, offset);

				j++;
			}

			pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
			pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
			pushBack((-indeces[j + 1] - 1) * 3, (-indeces[j + 1] - 1) * 3 + 1, (-indeces[j + 1] - 1) * 3 + 2, offset);

			i = j + 2;

			offset++;
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair)
	{
		std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> result{};

		if (pair.size() == 0)
			return result;

		std::sort(pair.begin(), pair.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		result.first.resize(pair.size());
		result.second.resize(pair[pair.size() - 1].second + 1);

		std::fill(result.second.begin(), result.second.end(), 0);

		for (std::size_t i = 0; i < pair.size(); i++) {
			result.first[i] = std::move(pair[i].first);
			result.second[pair[i].second]++;
		}

		return result;
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::optional<Objects<Vector2D, Vector3D>> GetObjects(Node&& objects)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(objects.name == "Objects");

		Objects<Vector2D, Vector3D> result{};

		for (auto& node : objects.children)
		{
			if (node.name == "Model" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [modelMesh, index] = GetModelMesh<Vector3D, CreateVector3DPolicy>(std::move(node));
				result.modelMeshes.emplace(index, std::move(modelMesh));
			}
			else if (node.name == "Geometry" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [geometryMesh, index] = GetGeometryMesh<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(node));
				result.geometryMeshes.emplace(index, std::move(geometryMesh));
			}
			else if (node.name == "Material")
			{
				auto [material, index] = GetMaterial<Vector3D, CreateVector3DPolicy>(std::move(node));
				result.materials.emplace(index, std::move(material));
			}
		}

		return result;
	}




}
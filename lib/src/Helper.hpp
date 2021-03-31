#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include<cassert>
#include<algorithm>
#include"ImplDataStruct.hpp"
#include"include/DataStruct.hpp"

#include<iostream>

namespace FBXL
{
	std::optional<const Node*> GetTopLevelNode(const PrimitiveData* primitiveData, const std::string& nodeName);

	//node.mChildrenのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetChildrenNodes(const Node* modelMesh, const std::string& name);

	//単一の子ノードの取得
	//複数ある場合はnullopt
	std::optional<const Node*> GetSingleChildrenNode(const Node* modelMesh, const std::string& name);

	//Node.mPropertiesの情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index);

	//propertyのインデックス3つを指定しVector３Dを生成
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);


	//GeometyMeshNodeからマテリアルのインデックス配列を取得
	std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh);

	//GeometyMeshNodeから法線のデータとポリゴンに対応じているのか頂点に対応しているのか
	std::pair<std::vector<double>, bool> GetNormalData(const Node* geometryMesh);

	//GeometyMeshNodeからUVの取得
	template<typename Vector2D, typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh);

	//materialの指定がないver
	template<typename Vector2D, typename Vector3D>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertexStructArrayAndMaterialRange(
		std::vector<Vector3D>&& positions, std::vector<Vector3D>&& normals, std::vector<Vector2D>&& uv);

	//geometryMeshのノードからシンプルな頂点データなどを取得
	template<typename Vector2D, typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh);

	//geometryMeshから取得したシンプルなデータを加工
	template<typename Vector2D, typename Vector3D>
	std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>>
		GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair);


	//modelMeshのノードからデータの取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>, std::int64_t> GetModelMesh(Node&& modelMesh);

	//GeometryMeshのノードからデータを取得
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh);

	//materialのノードからデータを取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>, std::int64_t> GetMaterial(Node&& modelMesh);

	std::pair<Texture, std::int64_t> GetTexture(Node&& textureNode);

	//PrimitiveDataからObjectsの取得
	template<typename Vector2D, typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	std::optional<Objects<Vector2D, Vector3D>> GetObjects(Node&& objects);

	//connectiosの取得
	Connections GetConnections(Node&& connections);

	//dstからインデックスを取得
	std::vector<ObjectOrPropertyIndex> GetConnectionByDestination(const Connections& connections, const ObjectOrPropertyIndex& key);


	//TransformのPolicyが追加される予定
	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> GetModel3DParts(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections);


	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> AppendModel3DParts(Model3DParts<Vector2D, Vector3D>&& a, Model3DParts<Vector2D, Vector3D>&& b);

	//
	//GeometryMEshにローカル座標を適用し変換
	//まだ仮
	//
	template<typename Vector2D, typename Vector3D,typename TranslationVector3DPolicy,typename RotationVector3DPolicy,typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh);

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> GetVertecesAndMaterialIndecsArray(Model3DParts<Vector2D, Vector3D>&& v);


	//マテリアエルにテクスチャの情報を加える
	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomarion(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios);

	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials);


	template<typename CreateVector3DPolicy>
	struct CreateVector3DInterfacePolicy
	{
		static auto Invoke(double a, double b, double c,std::size_t indexX,std::size_t indexY,std::size_t indexZ)
		{
			double nums[] = { a,b,c };
			return CreateVector3DPolicy::Create(nums[indexX], nums[indexY], nums[indexZ]);
		}
	};

	//
	//以下、関数テンプレートの実装
	//

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>, std::int64_t> GetModelMesh(Node&& modelMesh)
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
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Rotation", 4, 5, 6);
			if (vec)
				result.localRotation = vec.value();
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "PreRotation", 4, 5, 6);
			if (vec)
				result.localRotation = vec.value();
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Scaling", 4, 5, 6);
			if (vec)
				result.localScaling = vec.value();
		}

		return std::make_pair(std::move(result), index);
	}


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, std::size_t indexX, std::size_t indexY, std::size_t indexZ)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(geormetryMesh.name == "Geometry" && GetProperty<std::string>(&geormetryMesh, 2) == "Mesh");

		GeometryMesh<Vector2D, Vector3D> result{};

		auto index = GetProperty<std::int64_t>(&geormetryMesh, 0).value();

		auto vertexAndMaterialNumPairs = GetVertexAndMaterialNumberPairs<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh, indexX, indexY, indexZ);
		auto [vertices, materialRange] = GetVerticesAndMaterialRange(std::move(vertexAndMaterialNumPairs));
		result.vertices = std::move(vertices);
		result.materialRange = std::move(materialRange);

		return std::make_pair(std::move(result), index);
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>, std::int64_t> GetMaterial(Node&& modelMesh)
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
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index)
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



	template<typename Vector2D, typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh)
	{

		auto layerElementUVNode = GetSingleChildrenNode(geometryMesh, "LayerElementUV");

		auto uvNode = GetSingleChildrenNode(layerElementUVNode.value(), "UV");
		auto uvIndexNode = GetSingleChildrenNode(layerElementUVNode.value(), "UVIndex");

		auto uv = GetProperty<std::vector<double>>(uvNode.value(), 0).value();
		auto uvIndex = GetProperty<std::vector<std::int32_t>>(uvIndexNode.value(), 0).value();

		auto isVailedIndex = [&uv](std::int32_t index) {
			return 0 <= index && index < uv.size();
		};


		std::vector<Vector2D> result{};
		result.reserve(uvIndex.size());
		for (std::size_t i = 0; i < uvIndex.size(); i++) {
			if (isVailedIndex(uvIndex[i] * 2) && isVailedIndex(uvIndex[i] * 2 + 1))
				result.push_back(CreateVector2DPolicy::Create(uv[uvIndex[i] * 2], uv[uvIndex[i] * 2 + 1]));
			else
				result.push_back(CreateVector2DPolicy::Create(0.0, 0.0));
		}

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


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh,std::size_t indexX,std::size_t indexY,std::size_t indexZ)
	{
		std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> result{};

		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		auto vertices = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

		auto [normals, normalIsByPolygon] = GetNormalData(geometryMesh);
		auto uvs = GetUVs<Vector2D, CreateVector2DPolicy>(geometryMesh);

		auto materialIndeces = GetMaterialIndeces(geometryMesh);

		
		//assert((normalIsByPolygon && normals.size() == indeces.size()) ||
			//(!normalIsByPolygon && normals.size() == vertices.size()));

		//uvはByPolygonVertex一択のはず
		assert(uvs.size() == indeces.size());


		auto pushBack = [&vertices, &normals, normalIsByPolygon, &uvs, &result, &materialIndeces,indexX,indexY,indexZ](std::size_t index1, std::size_t index2, std::size_t index3, std::size_t offset) {

			Vertex<Vector2D, Vector3D> tmpVec;
			tmpVec.position = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(vertices[index1], vertices[index2], vertices[index3], indexX, indexY, indexZ);
			//CreateVector3DPolicy::Create(vertices[index1], vertices[index2], vertices[index3]);

			if (normalIsByPolygon)
				tmpVec.normal = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(normals[offset * 3], normals[offset * 3 + 1], normals[offset * 3 + 2], indexX, indexY, indexZ);
			//CreateVector3DPolicy::Create(normals[offset * 3], normals[offset * 3 + 1], normals[offset * 3 + 2]);
			else
				tmpVec.normal = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(normals[index1], normals[index2], normals[index3], indexX, indexY, indexZ);
			//CreateVector3DPolicy::Create(normals[index1], normals[index2], normals[index3]);


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
	std::optional<Objects<Vector2D, Vector3D>> GetObjects(Node&& objects,std::size_t indexX,std::size_t indexY,std::size_t indexZ)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(objects.name == "Objects");

		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
		std::unordered_map<std::int64_t, Texture> textures{};

		for (auto& node : objects.children)
		{
			if (node.name == "Model" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [modelMesh, index] = GetModelMesh<Vector3D, CreateVector3DPolicy>(std::move(node));
				modelMeshes.emplace(index, std::move(modelMesh));
			}
			else if (node.name == "Geometry" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [geometryMesh, index] = GetGeometryMesh<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(node), indexX, indexY, indexZ);
				geometryMeshes.emplace(index, std::move(geometryMesh));
			}
			else if (node.name == "Material")
			{
				auto [material, index] = GetMaterial<Vector3D, CreateVector3DPolicy>(std::move(node));
				materials.emplace(index, std::move(material));
			}
			else if (node.name == "Texture")
			{
				auto [texture, index] = GetTexture(std::move(node));
				textures.emplace(index, std::move(texture));
			}
		}

		return std::make_tuple(std::move(modelMeshes), std::move(geometryMeshes), std::move(materials), std::move(textures));
	}

	template<typename Vector2D, typename Vector3D,typename TranslationVector3DPolicy,typename RotationVector3DPolicy,typename ScallingVector3DPolicy>
	Model3DParts<Vector2D, Vector3D> GetModel3DParts(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections)
	{
		Model3DParts<Vector2D, Vector3D> result{};

		for (auto&& modelMesh : modelMeshes)
		{
			auto cs = GetConnectionByDestination(connections, modelMesh.first);

			std::optional<GeometryMesh<Vector2D, Vector3D>> geometryMesh = std::nullopt;
			std::vector<std::int64_t> materialIndex{};

			for (auto&& c : cs)
			{
				//オブジェクトとのつながりのみ
				if (c.index() == 0)
				{
					//geometryMeshを探す
					//moveしてしまう
					{
						auto iter = geometryMeshes.find(std::get<0>(c));
						if (iter != geometryMeshes.end())
						{
							geometryMesh = std::move(iter->second);
							geometryMeshes.erase(iter);
						}
					}

					//Materialを探す
					//そのインデックスだけメモ
					//順番あっているか分からん
					{
						auto iter = materials.find(std::get<0>(c));
						if (iter != materials.end())
							materialIndex.push_back(std::get<0>(c));
					}
				}
			}

			if (geometryMesh)
			{
				assert(geometryMesh.value().materialRange.size() == materialIndex.size());

				auto g = TransformGeometryMesh<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
					std::move(geometryMesh.value()), std::move(modelMesh.second));
				result = AppendModel3DParts<Vector2D, Vector3D>(std::move(result), std::make_pair(std::move(g), std::move(materialIndex)));
			}
		}

		return result;
	}


	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> AppendModel3DParts(Model3DParts<Vector2D, Vector3D>&& a, Model3DParts<Vector2D, Vector3D>&& b)
	{
		Model3DParts<Vector2D, Vector3D> result{};

		auto vertex = GetVertecesAndMaterialIndecsArray<Vector2D, Vector3D>(std::move(a));

		{
			auto tmpV = GetVertecesAndMaterialIndecsArray<Vector2D, Vector3D>(std::move(b));
			std::move(tmpV.begin(), tmpV.end(), std::back_inserter(vertex));
		}

		std::sort(vertex.begin(), vertex.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});


		for (auto&& v : vertex)
		{
			if (result.second.size() == 0 || result.second.back() != v.second)
			{
				result.first.materialRange.push_back(v.first.size());
				std::move(v.first.begin(), v.first.end(), std::back_inserter(result.first.vertices));
				result.second.push_back(v.second);
			}
			else //if (result.second.back() == v.second)
			{
				result.first.materialRange.back() += v.first.size();
				std::move(v.first.begin(), v.first.end(), std::back_inserter(result.first.vertices));
			}
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> GetVertecesAndMaterialIndecsArray(Model3DParts<Vector2D, Vector3D>&& v)
	{
		std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> result{};

		std::vector<Vertex<Vector2D, Vector3D>> tmpVertex{};
		std::size_t vertexOffset{};

		for (std::size_t i = 0; i < v.second.size(); i++)
		{
			std::move(v.first.vertices.begin() + vertexOffset, v.first.vertices.begin() + vertexOffset + v.first.materialRange[i], std::back_inserter(tmpVertex));
			result.push_back(std::make_pair(std::move(tmpVertex), v.second[i]));
			vertexOffset += v.first.materialRange[i];
		}

		return result;
	}


	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomarion(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios)
	{
		for (auto& [index, material] : materials)
		{
			auto cs = GetConnectionByDestination(connectios, std::make_pair(index, "DiffuseColor"));

			for (auto& c : cs)
			{
				//とりあえずプロパティとしてマテリアルとつながっているテクスチャのみ
				//Diffuseのテクスチャのみ
				if (c.index() == 0)
				{
					auto& index = std::get<0>(c);

					auto textureIter = textures.find(index);
					if (textureIter != textures.end())
						material.diffuseColorTexturePath = textureIter->second.relativeFileName;
				}
			}
		}

		return materials;
	}


	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials)
	{
		Model3D<Vector2D, Vector3D> result{};
		result.vertices = std::move(model3DPair.first.vertices);
		result.materialRange = std::move(model3DPair.first.materialRange);

		result.material.resize(model3DPair.second.size());
		for (std::size_t i = 0; i < model3DPair.second.size(); i++)
		{
			auto iter = materials.find(model3DPair.second[i]);
			if (iter != materials.end())
				result.material[i] = std::move(iter->second);
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh)
	{
		for (std::size_t i = 0; i < geometryMesh.vertices.size(); i++)
		{
			if (modelMesh.localScaling)
				geometryMesh.vertices[i].position = ScalinngVector3DPolicy::Scalling(std::move(geometryMesh.vertices[i].position), modelMesh.localScaling.value());
			if (modelMesh.localRotation) {
				geometryMesh.vertices[i].position = RotationVector3DPolicy::Rotation(std::move(geometryMesh.vertices[i].position), modelMesh.localRotation.value());
				geometryMesh.vertices[i].normal = RotationVector3DPolicy::Rotation(std::move(geometryMesh.vertices[i].normal), modelMesh.localRotation.value());
			}
			if (modelMesh.localTranslation)
				geometryMesh.vertices[i].position = TranslationVector3DPolicy::Translation(std::move(geometryMesh.vertices[i].position), modelMesh.localTranslation.value());
	
		}

		return geometryMesh;
	}
}
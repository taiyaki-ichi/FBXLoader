#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

#include<iostream>

namespace FBXL
{
	std::optional<const Node*> GetTopLevelNode(const PrimitiveData* primitiveData, const std::string& nodeName)
	{
		for (auto& node : primitiveData->nodes)
		{
			if (node.name == nodeName)
				return &node;
		}

		return std::nullopt;
	}

	std::optional<Node> RemoveTopLevelNode(PrimitiveData* primitiveData, const std::string& nodeName)
	{
		for (auto iter = primitiveData->nodes.begin(); iter != primitiveData->nodes.end(); iter++)
		{
			if (iter->name == nodeName) {
				auto result = std::move(*iter);
				primitiveData->nodes.erase(iter);
				return result;
			}
		}

		return std::nullopt;
	}


	std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(node->children.begin(), node->children.end(),
			[&name, &object](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
	}

	std::optional<const Node*> GetSingleChildrenNode(const Node* node, const std::string& name)
	{
		std::optional<const Node*> resultIndex = std::nullopt;
		for (std::size_t i = 0; i < node->children.size(); i++)
		{
			if (!resultIndex && node->children[i].name == name)
				resultIndex = &node->children[i];
			else if (resultIndex && node->children[i].name == name) {
				resultIndex = std::nullopt;
				break;
			}
		}
		return resultIndex;
	}


	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name)
	{
		assert(prop70->name == "Properties70");

		auto ps = GetChildrenNodes(prop70, "P");

		for (auto p : ps)
		{
			if (GetProperty<std::string>(p, 0).value() == name)
				return p;
		}

		return std::nullopt;
	}


	std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh)
	{
		auto layerElementMaterialNode = GetSingleChildrenNode(geometyMesh, "LayerElementMaterial");

		if (layerElementMaterialNode)
		{
			auto materialNode = GetSingleChildrenNode(layerElementMaterialNode.value(), "Materials").value();

			return GetProperty<std::vector<std::int32_t>>(materialNode, 0).value();
		}
		else
			return std::nullopt;
	}
	namespace
	{
		ObjectOrPropertyIndex GetObjectIndex(const Node* node, std::size_t offset)
		{
			return GetProperty<std::int64_t>(node, offset).value();
		}

		ObjectOrPropertyIndex GetPropertyIndex(const Node* node, std::size_t offset)
		{
			return std::make_pair(GetProperty<std::int64_t>(node, offset).value(), GetProperty<std::string>(node, offset + 1).value());
		}
	}

	std::pair<Texture, std::int64_t> GetTexture(Node&& textureNode)
	{
		assert(textureNode.name == "Texture");

		Texture result{};

		auto index = GetProperty<std::int64_t>(&textureNode, 0).value();

		{
			auto node = GetSingleChildrenNode(&textureNode, "FileName").value();
			result.fileName = GetProperty<std::string>(node, 0).value();
		}

		{
			auto node = GetSingleChildrenNode(&textureNode, "RelativeFilename").value();
			result.relativeFileName = GetProperty<std::string>(node, 0).value();
		}

		return std::make_pair(std::move(result), index);
	}

	Connections GetConnections(Node&& connections)
	{
		assert(connections.name == "Connections");

		Connections result{};
		result.reserve(connections.children.size());

		std::size_t offset{};
		ObjectOrPropertyIndex src{};
		ObjectOrPropertyIndex dst{};
		for (auto& node : connections.children)
		{
			auto type = GetProperty<std::string>(&node, 0);

			if (type.value()[0] == 'O') {
				src = GetObjectIndex(&node, 1);
				offset = 2;
			}
			else if (type.value()[0] == 'P') {
				offset = 3;
				src = GetPropertyIndex(&node, 1);
			}

			if (type.value()[1] == 'O')
				dst = GetObjectIndex(&node, offset);
			else if (type.value()[1] == 'P')
				dst = GetPropertyIndex(&node, offset);

			result.emplace_back(std::make_pair(std::move(src), std::move(dst)));
		}

		return result;
	}


	std::vector<ObjectOrPropertyIndex> GetConnectionByDestination(const Connections& connections, const ObjectOrPropertyIndex& key)
	{
		std::vector<ObjectOrPropertyIndex> result{};

		for (const auto& c : connections)
		{

			if ((key.index() == 0 &&
				c.second.index() == 0 &&
				std::get<0>(key) == std::get<0>(c.second)) ||
				(key.index() == 1 &&
					c.second.index() == 1 &&
					std::get<1>(key) == std::get<1>(c.second)))
			{
				result.emplace_back(c.first);
			}
		}

		return result;
	}

	GlobalSettings GetGlobalSettings(Node&& globalSettingsNode)
	{
		assert(globalSettingsNode.name == "GlobalSettings");

		GlobalSettings result{};

		auto globalSettingsProp70 = GetSingleChildrenNode(&globalSettingsNode, "Properties70");

		auto coordAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "CoordAxis", 4);
		if (coordAxis)
			result.coordAxis = coordAxis.value();

		auto coordAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "CoordAxisSign", 4);
		if (coordAxisSign)
			result.coordAxisSign = coordAxisSign.value();

		auto upAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "UpAxis", 4);
		if (upAxis)
			result.upAxis = upAxis.value();

		auto upAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "UpAxisSign", 4);
		if (upAxisSign)
			result.upAxisSign = upAxisSign.value();


		auto frontAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "FrontAxis", 4);
		if (frontAxis)
			result.frontAxis = frontAxis.value();

		auto frontAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "FrontAxisSign", 4);
		if (frontAxisSign)
			result.frontAxisSign = frontAxisSign.value();

		return result;
	}

	std::pair<std::vector<double>, bool> GetNormalData(const Node* geometryMesh)
	{
		auto layerElementNormalNode = GetSingleChildrenNode(geometryMesh, "LayerElementNormal");

		auto modelMesh = GetSingleChildrenNode(layerElementNormalNode.value(), "Normals");

		auto mappingInformationTypeNode = GetSingleChildrenNode(layerElementNormalNode.value(), "MappingInformationType");
		bool isByPolygon{};
		if (mappingInformationTypeNode)
			isByPolygon = (GetProperty<std::string>(mappingInformationTypeNode.value(), 0) == "ByPolygonVertex");

		auto result = GetProperty<std::vector<double>>(modelMesh.value(), 0).value();

		return std::make_pair(std::move(result), isByPolygon);
	}

}
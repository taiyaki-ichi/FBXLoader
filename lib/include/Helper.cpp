#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

#include<iostream>

namespace FBXL
{

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

	std::vector<const Node*> GetNodes(const PrimitiveData* data, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(data->nodes.begin(), data->nodes.end(),
			[&object, &name](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
	}

	std::optional<const Node*> GetSingleNode(const PrimitiveData* data, const std::string& name)
	{
		std::optional<const Node*> resultIndex = std::nullopt;
		for (std::size_t i = 0; i < data->nodes.size(); i++)
		{
			if (!resultIndex && data->nodes[i].name == name)
				resultIndex = &data->nodes[i];
			else if (resultIndex && data->nodes[i].name == name) {
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

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index)
	{
		assert(connection->name == "Connections");

		std::vector<std::int64_t> object{};
		std::vector<std::pair<std::int64_t, std::string>> prop{};

		std::int64_t src;
		std::string srcStr;
		std::int64_t dst;
		std::size_t offset{};
		for (auto& n : connection->children)
		{
			auto type = GetProperty<std::string>(&n, 0);

			src = GetProperty<std::int64_t>(&n, 1).value();

			if (type.value()[0] == 'O')
				offset = 2;
			else if (type.value()[0] == 'P')
				offset = 3;

			if (type.value()[1] == 'O')
			{
				dst = GetProperty<std::int64_t>(&n, offset).value();
				if (dst == index)
					object.push_back(src);
			}
			else if (type.value()[1] == 'P')
			{
				dst = GetProperty<std::int64_t>(&n, offset).value();
				srcStr = GetProperty<std::string>(&n, offset + 1).value();

				if (dst == index)
					prop.push_back(std::make_pair(src, srcStr));
			}
		}

		return std::make_pair(object, prop);
	}

	namespace {

		std::vector<std::pair<std::tuple<std::int32_t,std::int32_t,std::int32_t>,std::int32_t>> 
			GetTrianglePolygon(std::vector<std::int32_t>&& indecse, std::vector<std::int32_t>&& material)
		{
			std::vector<std::pair<std::tuple<std::int32_t, std::int32_t, std::int32_t>, std::int32_t>> resultIndex{};

			std::size_t materialOffset = 0;
			std::size_t i = 0;
			std::size_t j = 0;
			while (i < indecse.size())
			{
				j = i + 1;

				while (indecse[j + 1] >= 0)
				{
					resultIndex.emplace_back(std::make_tuple(indecse[i], indecse[j], indecse[j + 1]), material[materialOffset]);
					j++;
				}

				resultIndex.emplace_back(std::make_tuple(indecse[i], indecse[j], -indecse[j + 1] - 1), material[materialOffset]);

				i = j + 2;
				materialOffset++;
			}

			return resultIndex;
		}

		std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>
			GetIndexArray(std::vector<std::pair<std::tuple<std::int32_t, std::int32_t, std::int32_t>, std::int32_t>>&& a)
		{
			std::vector<std::int32_t> resultIndex{};
			std::vector<std::int32_t> materialRangeArray(a[a.size() - 1].second + 1);

		
			resultIndex.reserve(a.size() * 3);
			for (auto& [vec,i] : a)
			{
				resultIndex.push_back(std::get<0>(vec));
				resultIndex.push_back(std::get<1>(vec));
				resultIndex.push_back(std::get<2>(vec));

				materialRangeArray[i]++;
			}
			return std::make_pair(std::move(resultIndex), std::move(materialRangeArray));
		}

		std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>
			GetSingleMaterialIndeces(std::vector<std::int32_t>&& indecse)
		{
			std::vector<std::int32_t> resultIndex{};

			std::size_t i = 0;
			std::size_t j = 0;
			while (i < indecse.size())
			{
				j = i + 1;

				while (indecse[j + 1] >= 0)
				{
					resultIndex.push_back(indecse[i]);
					resultIndex.push_back(indecse[j]);
					resultIndex.push_back(indecse[j + 1]);
					j++;
				}

				resultIndex.push_back(indecse[i]);
				resultIndex.push_back(indecse[j]);
				resultIndex.push_back(-indecse[j + 1] - 1);

				i = j + 2;
			}


			std::vector<std::int32_t> resultMaterialRange{ static_cast<std::int32_t>(resultIndex.size()) };

			return { resultIndex,resultMaterialRange };
		}
	}

	std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>> GetIndecesAndMAterialRange(std::vector<std::int32_t>&& indecse,std::vector<std::int32_t>&& material)
	{
		if (material.size() == 1)
			return GetSingleMaterialIndeces(std::move(indecse));
		else
		{
			auto i = GetTrianglePolygon(std::move(indecse), std::move(material));
			std::sort(i.begin(), i.end(), [](auto& a, auto& b) {
				return a.second < b.second;
				});

			return GetIndexArray(std::move(i));
		}
	}
}
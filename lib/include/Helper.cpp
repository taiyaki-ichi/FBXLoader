#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

#include<iostream>

namespace FBXL
{

	std::vector<const Node*> GetChildrenNode(const Node* node, const std::string& name)
	{
		std::vector<const Node*> result{};
		std::for_each(node->mChildren.begin(), node->mChildren.end(),
			[&name, &result](const Node& n) {if (n.mName == name)result.push_back(&n); }
		);
		return result;
	}

	std::vector<const Node*> GetNode(const Data* data, const std::string& name)
	{
		std::vector<const Node*> result{};
		std::for_each(data->mNodes.begin(), data->mNodes.end(),
			[&result, &name](const Node& n) {if (n.mName == name)result.push_back(&n); }
		);
		return result;
	}

	std::optional<NodeAttribute> GetNodeAttribute(const Node* node)
	{
		if (node->mName != "NodeAttribute")
			return std::nullopt;

		NodeAttribute result{};

		{
			auto r = GetProperty<std::int64_t>(node, 0);
			if (r)
				result.mIndex = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 1);
			if (r)
				result.mMagic = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 2);
			if (r)
				result.mName = r.value();
		}

		auto prop70 = GetChildrenNode(node, "Properties70");
		
		if (prop70.size() == 1)
		{
			NodeAttributeOption option{};

			auto p = GetChildrenNode(prop70[0], "P");
			assert(p.size() == 1);


			for (int i = 0; i < 3; i++)
			{
				auto r = GetProperty<std::string>(p[0], i);
				if (r)
					option.mProp70Strings[i] = r.value();
			}
			
			{
				auto r = GetProperty<double>(p[0], 4);
				if (r)
					option.mProp70Double = r.value();
			}

			result.mOption = std::move(option);
		}

		auto typeFlag = GetChildrenNode(node, "TypeFlags");
		assert(typeFlag.size() == 1);

		{
			auto r = GetProperty<std::string>(typeFlag[0], 0);
			if (r)
				result.mTypeFlag = r.value();
		}

		return result;
	}

	void Print(const NodeAttribute& na)
	{
		std::cout << "NodeAttribute\n";
		std::cout << "index : " << na.mIndex << "\n";
		std::cout << "magic : " << na.mMagic << "\n";
		std::cout << "name : " << na.mName << "\n";
		if (na.mOption)
		{
			for (int i = 0; i < 3; i++)
				std::cout << "prop70 " << i << " : " << na.mOption.value().mProp70Strings[i] << "\n";
			std::cout << "prop70D : " << na.mOption.value().mProp70Double << "\n";
		}
		std::cout << "typeflag : " << na.mTypeFlag << "\n";

		std::cout << "\n";
	}



}
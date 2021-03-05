#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

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



}
#include"include/Loader.hpp"

#include<iostream>

template<typename T>
void printFunc(std::vector<T>& v)
{
	for (auto& tmp : v)
		std::cout << v << " ";
	std::cout << "\n";
}

template<typename T>
void ptintFunc(T& t)
{
	std::cout << t << "\n";
}

std::vector<FBXL::Node> findNode(const FBXL::Node& n, const std::string& s)
{
	std::vector<FBXL::Node> result{};

	if (n.mName == s)
		result.push_back(n);

	for (auto& tmpNode : n.mChildren)
	{
		auto ns = findNode(tmpNode, s);
		result.reserve(result.size() + ns.size());
		for (auto& tmp : ns)
			result.push_back(tmp);
	}

	if (result.size() > 0)
		std::cout << n.mName << "\n";
	return result;
}

std::vector<FBXL::Node> findNode(const std::vector<FBXL::Node>& ns, const std::string& s)
{
	std::vector<FBXL::Node> result{};

	for (auto& n : ns)
	{
		auto tmp = findNode(n, s);
		for (auto& t : tmp)
			result.push_back(t);
	}

	return result;
}

int main()
{


	//auto rootNode = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto rootNode = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto rootNode = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	auto rootNode = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");

	/*
	FBXL::Node object;
	for (auto& n : rootNode)
		if (n.mName == "Objects")
			object = n;

	auto v = findNode(object, "Vertices");
	
	auto m = findNode(rootNode, "Material");

	auto d = findNode(rootNode, "DiffuseColor");

	auto mm = findNode(rootNode, "Mesh");

	auto t = findNode(rootNode, "Texture");

	auto l = findNode(rootNode, "LimbNode");
	*/

	return 0;
}
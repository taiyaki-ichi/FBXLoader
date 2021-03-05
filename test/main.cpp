#include"include/Loader.hpp"
#include"include/Helper.hpp"

#include<iostream>


int main()
{


	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	//auto data2 = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");

	auto objects = FBXL::GetNode(&data, "Objects");

	auto atttributes = FBXL::GetChildrenNode(objects[0], "NodeAttribute");

	FBXL::NodeAttribute tmp;
	for (auto attri : atttributes)
	{
		tmp = FBXL::GetNodeAttribute(attri).value();
		FBXL::Print(tmp);
	}


	return 0;
}
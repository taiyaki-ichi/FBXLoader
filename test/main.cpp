#include"include/Loader.hpp"
#include"include/Helper.hpp"

#include<iostream>
#include<fstream>


struct Vector3
{
	double x{};
	double y{};
	double z{};

	Vector3(double inX, double inY, double inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}
};


int main()
{

	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/Yuno_ver1.01/Mesh/CS_girl_MasterFile_Sock.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/test_body.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/test_dog_003.fbx");

    //Roundnessによって値が変わるはず
    //auto data = FBXL::LoadFBX("../../Assets/test_dog_004_r50.fbx");
    //auto data2 = FBXL::LoadFBX("../../Assets/test_dog_004_r10.fbx");

	auto data = FBXL::LoadFBX("../../Assets/cube005.fbx");


    //モディファイアの情報は頂点として保存されてそう


	auto objects = FBXL::GetNode(&data, "Objects");

	assert(objects.size() == 1);

	auto geometry = FBXL::GetChildrenNode(objects[0], "Geometry");

	std::ofstream file("hoge.txt");

	for (auto g : geometry)
	{
		auto hoge = FBXL::GetGeometryMesh<std::vector<Vector3>, std::vector<std::int32_t>>(g);

		if (hoge)
		{
			for (auto v : hoge.value().vertices)
				file << v.x << " " << v.y << " " << v.z << "\n";
		}
	}


	return 0;
}



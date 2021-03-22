#include"include/Loader.hpp"
#include"include/Helper.hpp"

#include<iostream>
#include<fstream>


struct Vector3
{
	double x{};
	double y{};
	double z{};
	/*
	Vector3() = default;

	Vector3(double inX, double inY, double inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}
	*/
};

struct Vector2
{
	double x{};
	double y{};

	/*
	Vector2() = default;

	Vector2(double inX, double inY)
	{
		x = inX;
		y = inY;
	}
	*/

};


int main()
{

	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");
   // auto data = FBXL::LoadFBX("../../Assets/Yuno_ver1.01/Mesh/CS_girl_MasterFile_Sock.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/test_body.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/test_dog_003.fbx");

    //Roundnessによって値が変わるはず
    //auto data = FBXL::LoadFBX("../../Assets/test_dog_004_r50.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/test_dog_004_r10.fbx");

	//auto data = FBXL::LoadFBX("../../Assets/cube005.fbx");

	//auto data = FBXL::LoadFBX("../../Assets/test_material_texture.fbx");
	auto data = FBXL::LoadFBX("../../Assets/test_texture_002.fbx");


    //モディファイアの情報は頂点として保存されてそう


	std::optional<FBXL::Objects<Vector2, Vector3>> objects{};
	std::optional<FBXL::Connections> connections{};

	for (auto& node : data.nodes)
	{
		if (node.name == "Objects") {
			objects = FBXL::GetObjects<Vector2, Vector3>(std::move(node));
		}
		else if (node.name == "Connections")
		{
			connections = FBXL::GetConnections(std::move(node));
			/*
			for (auto& [src, dst] : connections.value())
			{
				if (src.index() == 1)
					std::cout << std::get<1>(src).first << " " << std::get<1>(src).second << "\n";

				if (dst.index() == 1)
					std::cout << std::get<1>(dst).first << " " << std::get<1>(dst).second << "\n";
			}
			*/
		}
	}

	if (objects && connections)
	{
		auto&& [modelMeshes, geometryMeshes, materials, textures] = std::move(objects.value());
		auto hoge = FBXL::GetModel3DParts<Vector2, Vector3>(std::move(modelMeshes), std::move(geometryMeshes), materials, connections.value());

		auto huga = FBXL::AddTextureInfomarion<Vector3>(std::move(materials), std::move(textures), connections.value());


	}


	return 0;
}



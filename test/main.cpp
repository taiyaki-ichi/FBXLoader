#include"include/Loader.hpp"
#include"include/Helper.hpp"

#include<iostream>

void printBlank(std::size_t n)
{
	for (int i = 0; i < n; i++)
		std::cout << " ";
}

void print(const FBXL::Node* cn, const FBXL::Node* obj, std::int64_t index,std::size_t nestSize)
{
	auto n = FBXL::GetNodeByIndex(obj, index);
	printBlank(nestSize * 2);

	//•ª‚©‚è‚É‚­‚¢‚Ì‚ÅDeformer‚Ê‚©‚·
	if (n && n.value()->mName != "Deformer")
	{
		if (n)
			std::cout << n.value()->mName << " (" << FBXL::GetProperty<std::string>(n.value(), 2).value() << ")\n";
		else
			std::cout << index << " is nod found\n";

		auto is = FBXL::GetConnectionObjectByDestination(cn, index);
		for (auto i : is)
			print(cn, obj, i, nestSize + 1);
	}
}


int main()
{


	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	//auto data2 = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");

	
	auto objects = FBXL::GetNode(&data, "Objects");

	auto cn = FBXL::GetNode(&data, "Connections");

	auto is = FBXL::GetConnectionObjectByDestination(cn[0], 0);

	print(cn[0], objects[0], is[1], 0);


	return 0;
}


/*
Model (Null)
  NodeAttribute (Null)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
  Model (Mesh)
    Geometry (Mesh)
          Material ()
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
      Texture ()
        Video (Clip)
*/
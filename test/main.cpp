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

	//•ª‚©‚è‚É‚­‚¢‚Ì‚ÅDeformer‚Ê‚©‚·
    if (!n || n.value()->mName != "Deformer")
    {
        printBlank(nestSize * 2);
        if (n)
            std::cout << n.value()->mName << " (" << FBXL::GetProperty<std::string>(n.value(), 2).value() << ")\n";
        else
            std::cout << index << " is nod found\n";

        auto [o, p] = FBXL::GetConnectionByDestination(cn, index);
        for (auto i : o)
            print(cn, obj, i, nestSize + 1);

        for (auto& [i, str] : p)
        {
            auto a = FBXL::GetNodeByIndex(obj, i);
            //
            if (a && a.value()->mName != "AnimationCurveNode")
            {
                printBlank(nestSize * 2 + 1);
                std::cout << a.value()->mName << "  " << str << "\n";
            }
        }

    }
}


int main()
{


	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	//auto data2 = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");
    //auto data = FBXL::LoadFBX("../../Assets/Yuno_ver1.01/Mesh/CS_girl_MasterFile_Sock.fbx");


	
	auto objects = FBXL::GetNode(&data, "Objects");

    
	auto cn = FBXL::GetNode(&data, "Connections");
    
    
	//auto [a,b] = FBXL::GetConnectionByDestination(cn[0], 0);

    print(cn[0], objects[0], 0, 0);
    

    /*
    for (auto& n : cn[0]->mChildren)
    {
        auto type = FBXL::GetProperty<std::string>(&n, 0);
        if (type)
            std::cout << type.value() << "\n";
        else
            std::cout << "not found\n";
    }
    */
    


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
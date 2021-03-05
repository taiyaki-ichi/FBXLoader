#include"include/Loader.hpp"
#include"include/Helper.hpp"

#include<iostream>


int main()
{


	//auto data = FBXL::LoadFBX("../../Assets/Fox.FBX");
	//auto data = FBXL::LoadFBX("../../Assets/Handgun/Handgun_fbx_7.4_binary.fbx");
	//auto data = FBXL::LoadFBX("../../Assets/Dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
	auto data = FBXL::LoadFBX("../../Assets/unitychan/unitychan.fbx");
	auto data2 = FBXL::LoadFBX("../../Assets/unitychan/BoxUnityChan.fbx");

	auto c = FBXL::GetNode(&data, "Connections");

	/*
	for (auto& n : c[0]->mChildren)
	{

		auto type = FBXL::GetProperty<std::string>(&n, 0);
		//std::cout << type << " ";

		std::size_t offset = 1;
		for (std::size_t i = 0; i < 2; i++)
		{
			if (type[i] == 'O')
			{
				//std::cout << FBXL::GetProperty<std::int64_t>(&n, offset) << " ";
				auto index = FBXL::GetProperty<std::int64_t>(&n, offset);
				try
				{
					FBXL::GetNodeObject(&data, index);
				}
				catch (std::exception& e)
				{
					//std::cout << "type : " << type << "      " << e.what() << " : " << index << "\n";
					//std::cout << index << "\n";

					
					try {
						FBXL::GetNodeObject(&data2, index);
					}
					catch (std::exception& e)
					{
						std::cout << index << "\n";
					}
					

					//std::cout << n.mName << "\n";

					std::cout << " invailed P ->  ";
				}
				
				offset += 1;
			}
			else if (type[i] == 'P')
			{
				std::cout << FBXL::GetProperty<std::int64_t>(&n, offset) << " " << FBXL::GetProperty<std::string>(&n, offset + 1) << " ";
				offset += 2;
			}
			else {
				//std::cout << type[i] << " <- ?    ";
			}
		}
		std::cout << "\n";
	}
	
	*/
	//auto hoge = FBXL::GetNodeObject(&data, 5360957968);
	//auto hoge = FBXL::GetNodeObject(&data, std::int64_t{ 5442126640 });

	//105553121979200
	return 0;
}
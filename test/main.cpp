#include"include/Loader.hpp"

#include<iostream>
#include<fstream>


struct Vector3
{
	double x{};
	double y{};
	double z{};

};

struct Vector2
{
	double x{};
	double y{};

};


int main()
{
	auto model3D = FBXL::LoadModel3D<Vector2, Vector3>("../../Assets/test_texture_002.fbx");

	return 0;
}



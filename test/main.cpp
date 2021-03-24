#include"include/Loader.hpp"
#include"Window.hpp"

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
	CreateSimpleWindow(L"test", 800, 600);

	while (UpdateWindow());

	return 0;
}



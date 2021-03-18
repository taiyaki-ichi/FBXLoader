#pragma once

namespace FBXL
{

	template<typename VertexContainer>
	struct DefaultVertexContainerPushBack
	{
		static void PushBack(VertexContainer& c,double x, double y, double z)
		{
			c.emplace_back(x, y, z);
		}
	};

	template<typename IndexContainer>
	struct DefaultIndexContainerPushBack
	{
		static void PushBack(IndexContainer& c, std::int32_t i)
		{
			c.push_back(i);
		}
	};

	template<typename Vector3D>
	struct DefaultCreateVector3D
	{
		static Vector3D Create(double x, double y, double z)
		{
			return Vector3D{ x,y,z };
		}
		
	};

	template<typename Vector2D>
	struct DefaultCreateVector2D
	{
		static Vector2D Create(double x, double y) {
			return Vector2D{ x,y };
		}
	};
}
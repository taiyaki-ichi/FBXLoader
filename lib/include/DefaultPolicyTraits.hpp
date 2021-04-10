#pragma once
#include<cmath>

namespace FBXL
{
	constexpr double PI = 3.141592653589;

	template<typename Vector3D>
	struct DefaultCreateVector3D
	{
		static Vector3D Create(double x, double y, double z)
		{
			return Vector3D{ static_cast<float>(x),static_cast<float>(y),static_cast<float>(z) };
		}
		
	};

	template<typename Vector2D>
	struct DefaultCreateVector2D
	{
		static Vector2D Create(double x, double y) 
		{
			return Vector2D{ static_cast<float>(x),static_cast<float>(y) };
		}
	};

	template<typename Vector3D>
	struct DefaultRotationVector3DPolicy
	{
		static Vector3D Rotation(Vector3D&& vec, const Vector3D& rotationVec)
		{
			auto rot = [](auto& a, auto& b, auto rot) {
				auto memoA = a;
				auto memoB = b;
				a = memoA * std::cos(rot) - memoB * std::sin(rot);
				b = memoA * std::sin(rot) + memoB * std::cos(rot);
			};

			rot(vec.x, vec.y, rotationVec.z / 180.f * PI);
			rot(vec.y, vec.z, rotationVec.x / 180.f * PI);
			rot(vec.z, vec.x, rotationVec.y / 180.f * PI);

			return vec;
		}
	};

	template<typename Vector3D>
	struct DefaultTranslationVector3DPolicy
	{
		static Vector3D Translation(Vector3D&& vec, const Vector3D& transformVec)
		{
			vec.x += transformVec.x;
			vec.y += transformVec.y;
			vec.z += transformVec.z;

			return vec;
		}
	};

	template<typename Vector3D>
	struct DefaultScallingVector3DPolicy
	{
		static Vector3D Scalling(Vector3D&& vec, const Vector3D& scallingVec)
		{
			vec.x *= scallingVec.x;
			vec.y *= scallingVec.y;
			vec.z *= scallingVec.z;

			return vec;
		}
	};
}
#pragma once
#include<cstdint>

namespace FBXL
{
	struct GlobalSettings
	{
		std::int32_t coordAxis = 0;
		std::int32_t coordAxisSign = 1;
		std::int32_t upAxis = 1;
		std::int32_t upAxisSign = 1;
		std::int32_t frontAxis = 2;
		std::int32_t frontAxisSign = 1;
	};
}
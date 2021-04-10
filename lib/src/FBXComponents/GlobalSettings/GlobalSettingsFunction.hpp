#pragma once
#include"GlobalSettingsStruct.hpp"
#include"../../PrimitiveData/PrimitiveDataFunction.hpp"

namespace FBXL
{

	//NodeÇ©ÇÁGlobalSettingsÇÃéÊìæ
	inline GlobalSettings GetGlobalSettings(Node&& globalSettingNode);


	//
	//à»â∫ÅAíËã`
	//

	inline GlobalSettings GetGlobalSettings(Node&& globalSettingsNode)
	{
		assert(globalSettingsNode.name == "GlobalSettings");

		GlobalSettings result{};

		auto globalSettingsProp70 = GetSingleChildrenNode(&globalSettingsNode, "Properties70");

		auto coordAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "CoordAxis", 4);
		if (coordAxis)
			result.coordAxis = coordAxis.value();

		auto coordAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "CoordAxisSign", 4);
		if (coordAxisSign)
			result.coordAxisSign = coordAxisSign.value();

		auto upAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "UpAxis", 4);
		if (upAxis)
			result.upAxis = upAxis.value();

		auto upAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "UpAxisSign", 4);
		if (upAxisSign)
			result.upAxisSign = upAxisSign.value();

		auto frontAxis = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "FrontAxis", 4);
		if (frontAxis)
			result.frontAxis = frontAxis.value();

		auto frontAxisSign = GetProperities70Data<std::int32_t>(globalSettingsProp70.value(), "FrontAxisSign", 4);
		if (frontAxisSign)
			result.frontAxisSign = frontAxisSign.value();

		return result;
	}
}
#pragma once
#include"include/DataStruct.hpp"
#include"ImplDataStruct.hpp"

namespace FBXL
{
	//単純なデータとして読み込む
	PrimitiveData LoadPrimitiveData(const std::string& filePath);

}
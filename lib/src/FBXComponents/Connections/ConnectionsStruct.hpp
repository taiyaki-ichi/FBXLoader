#pragma once
#include<variant>
#include<vector>
#include<string>

namespace FBXL
{
	using ObjectOrPropertyIndex = std::variant<std::int64_t, std::pair<std::int64_t, std::string>>;
	using Connections = std::vector<std::pair<ObjectOrPropertyIndex, ObjectOrPropertyIndex>>;
}
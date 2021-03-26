#pragma once
#include<d3dcommon.h>

namespace DX12
{
	class Shader
	{
		ID3DBlob* blob = nullptr;

	public:
		Shader() = default;
		~Shader();

		//ファイル名、関数名、インクルードオプション
		void Intialize(const wchar_t* fileName, const char* funcName, const char* include);

		ID3DBlob* Get() const noexcept;
	};

}
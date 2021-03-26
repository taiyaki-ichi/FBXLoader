#include"Shader.hpp"
#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<string>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

namespace DX12
{

	Shader::~Shader()
	{
		if (blob)
			blob->Release();
	}

	void Shader::Intialize(const wchar_t* fileName, const char* funcName, const char* include)
	{
		ID3DBlob* errorBlob = nullptr;

		auto result = D3DCompileFromFile(
			fileName,
			nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			funcName, include,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0, &blob, &errorBlob);

		//Ž¸”s‚µ‚½ê‡
		if (FAILED(result))
		{
			if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				throw fileName + std::wstring{ L" is not found\n" };
			else {
				std::wstring errstr;
				errstr.resize(errorBlob->GetBufferSize());
				std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
				throw errstr + L" : " + fileName + L"\n";
			}
		}
	}

	ID3DBlob* Shader::Get() const noexcept
	{
		return blob;
	}

}
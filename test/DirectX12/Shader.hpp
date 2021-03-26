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

		//�t�@�C�����A�֐����A�C���N���[�h�I�v�V����
		void Intialize(const wchar_t* fileName, const char* funcName, const char* include);

		ID3DBlob* Get() const noexcept;
	};

}
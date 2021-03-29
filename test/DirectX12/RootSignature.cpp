#include"RootSignature.hpp"

namespace DX12
{

	RootSignature::~RootSignature()
	{
		if (rootSignature)
			rootSignature->Release();
	}

	RootSignature::RootSignature(RootSignature&& rhs) noexcept
	{
		rootSignature = rhs.rootSignature;
		rhs.rootSignature = nullptr;
	}

	RootSignature& RootSignature::operator=(RootSignature&& rhs) noexcept
	{
		rootSignature = rhs.rootSignature;
		rhs.rootSignature = nullptr;

		return *this;
	}

	ID3D12RootSignature* RootSignature::Get()
	{
		return rootSignature;
	}

}
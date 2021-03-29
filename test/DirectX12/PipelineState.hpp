#pragma once
#include"RootSignature.hpp"
#include"Shader.hpp"
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace DX12
{
	class Device;
	class CommandList;

	class PipelineState
	{
		RootSignature rootSignature{};
		ID3D12PipelineState* pipelineState = nullptr;

	public:
		PipelineState() = default;
		~PipelineState();

		PipelineState(const PipelineState&) = delete;
		PipelineState& operator=(const PipelineState&) = delete;

		PipelineState(PipelineState&&) noexcept;
		PipelineState& operator=(PipelineState&&) noexcept;

		void Initialize(Device*, RootSignature&&, Shader&& vs, Shader&& ps);

		void PreparationForDrawing(CommandList*);
	};


}
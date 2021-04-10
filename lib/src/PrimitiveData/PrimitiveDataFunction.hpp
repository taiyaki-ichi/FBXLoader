#pragma once
#include"PrimitiveDataStruct.hpp"
#include<optional>
#include<algorithm>
#include<cassert>

namespace FBXL
{

	//PrimitiveData.nodes����nodeName����v����m�[�h��؂���
	inline std::optional<Node> RemoveTopLevelNode(PrimitiveData* primitiveData, const std::string& nodeName);

	//node.mChildren�̃m�[�h�̂���name����v����m�[�h�̎Q�Ƃ̎擾
	inline std::vector<const Node*> GetChildrenNodes(const Node* modelMesh, const std::string& name);

	//�P��̎q�m�[�h�̎擾
	//��������ꍇ��nullopt
	inline std::optional<const Node*> GetSingleChildrenNode(const Node* modelMesh, const std::string& name);

	//Node.mProperties��index�Ԗڂ�T�^�̏����擾
	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index);

	//property�̃C���f�b�N�X3���w�肵Vector�RD�𐶐�
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	//Propeties70�̎q�m�[�h��0�Ԗڂ̖��O����v����node�̎擾
	inline std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	//Propeties70�̎q�m�[�h��0�Ԗڂ̖��O����v����node��index�Ԗڂ̌^T�̃f�[�^���擾
	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	//Propeties70�̎q�m�[�h��0�Ԗڂ̖��O����v����node��Vector3D�̃f�[�^���擾
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);



	//
	//�ȉ��A����
	//

	inline std::optional<Node> RemoveTopLevelNode(PrimitiveData* primitiveData, const std::string& nodeName)
	{
		for (auto iter = primitiveData->nodes.begin(); iter != primitiveData->nodes.end(); iter++)
		{
			if (iter->name == nodeName) {
				auto result = std::move(*iter);
				primitiveData->nodes.erase(iter);
				return result;
			}
		}

		return std::nullopt;
	}

	inline std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(node->children.begin(), node->children.end(),
			[&name, &object](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
	}

	std::optional<const Node*> GetSingleChildrenNode(const Node* node, const std::string& name)
	{
		std::optional<const Node*> resultIndex = std::nullopt;
		for (std::size_t i = 0; i < node->children.size(); i++)
		{
			if (!resultIndex && node->children[i].name == name)
				resultIndex = &node->children[i];
			else if (resultIndex && node->children[i].name == name) {
				resultIndex = std::nullopt;
				break;
			}
		}
		return resultIndex;
	}

	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index)
	{
		if (std::holds_alternative<T>(modelMesh->properties[index]))
			return std::get<T>(modelMesh->properties[index]);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		auto x = GetProperty<double>(modelMesh, xIndex);
		auto y = GetProperty<double>(modelMesh, yIndex);
		auto z = GetProperty<double>(modelMesh, zIndex);

		if (x && y && z)
			return CreateVector3DPolicy::Create(x.value(), y.value(), z.value());
		else
			return std::nullopt;
	}

	inline std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name)
	{
		assert(prop70->name == "Properties70");

		auto ps = GetChildrenNodes(prop70, "P");

		for (auto p : ps)
		{
			if (GetProperty<std::string>(p, 0).value() == name)
				return p;
		}

		return std::nullopt;
	}

	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetProperty<T>(p.value(), index);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), xIndex, yIndex, zIndex);
		else
			return std::nullopt;
	}

}
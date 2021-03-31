#pragma once
#include"../../include/DataStruct.hpp"
#include"PrimitiveDataStruct.hpp"

#ifndef ZLIB_WINAPI
#	define ZLIB_WINAPI
#endif

#include<zlib.h>
#include<zconf.h>
#include<iostream>
#include<fstream>
#include<optional>
#include<stdexcept>

#pragma comment(lib,"zlibstat.lib")

namespace FBXL
{
	namespace {

		//�𓀂���Ƃ��̒P��
		constexpr std::size_t CHUNK = 16384;


		template<typename T>
		T ReadPrimitiveType(std::istream& is)
		{
			T result{};
			is.read(reinterpret_cast<char*>(std::addressof(result)), sizeof(T));
			return result;
		}


		template<typename T>
		inline void ReadZipCompressedBuffer(std::vector<T>& result, std::vector<unsigned char>&& inBuffer)
		{
			z_stream strm;

			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			strm.avail_in = 0;
			strm.next_in = Z_NULL;

			if (inflateInit(&strm) != Z_OK)
				return;

			std::vector<unsigned char> tmp(CHUNK);

			std::size_t offset = 0;
			std::size_t availInISize;
			int ret;
			std::size_t have;
			do {
				availInISize = (CHUNK > inBuffer.size() - offset) ? inBuffer.size() - offset : CHUNK;

				strm.avail_in = availInISize;
				strm.next_in = &inBuffer[offset];

				do {
					//inflate���Ăяo�����ƂɍX�V����Ă��܂��̂�
					//�����[�v������
					strm.avail_out = CHUNK;
					strm.next_out = (Bytef*)tmp.data();

					ret = inflate(&strm, Z_NO_FLUSH);

					have = CHUNK - strm.avail_out;

					auto nowPos = result.size();
					result.resize(result.size() + have / sizeof(T));
					if (have > 0)
						std::memcpy(&result[nowPos], tmp.data(), have);

				} while (strm.avail_out == 0);

				offset += availInISize;

			} while (ret != Z_STREAM_END);

			inflateEnd(&strm);
		}

		template<typename T>
		std::vector<T> ReadArrayType(std::istream& is)
		{
			auto arrayLength = ReadPrimitiveType<std::uint32_t>(is);
			auto encoding = ReadPrimitiveType<std::uint32_t>(is);
			auto compressedLength = ReadPrimitiveType<std::uint32_t>(is);

			std::vector<T> result{};

			if (encoding == 0)
			{
				result.resize(arrayLength);
				is.read(reinterpret_cast<char*>(result.data()), static_cast<std::uint64_t>(arrayLength) * sizeof(T));
			}
			else if (encoding == 1)
			{
				std::vector<unsigned char> inBuffer(compressedLength);
				is.read(reinterpret_cast<char*>(inBuffer.data()), static_cast<std::uint64_t>(compressedLength));
				ReadZipCompressedBuffer(result, std::move(inBuffer));
			}

			return result;
		}

		inline std::string ReadStringType(std::istream& is)
		{
			auto length = ReadPrimitiveType<std::uint32_t>(is);

			std::string result{};
			result.resize(length);

			is.read(result.data(), length * sizeof(char));

			return result;
		}

		inline std::vector<unsigned char> ReadRawBinaryType(std::istream& is)
		{
			auto length = ReadPrimitiveType<std::uint32_t>(is);

			std::vector<unsigned char> result(length);

			is.read(reinterpret_cast<char*>(result.data()), length * sizeof(unsigned char));

			return result;
		}


		template<typename UintType>
		std::optional<Node> LoadNodeImpl(std::istream& is)
		{
			Node result{};

			//
			std::streampos endPos{ static_cast<long long>(ReadPrimitiveType<UintType>(is)) };

			auto numProperties = ReadPrimitiveType<UintType>(is);

			result.properties.resize(static_cast<std::size_t>(numProperties));

			//�g��Ȃ��H
			auto propertyListLen = ReadPrimitiveType<UintType>(is);

			auto nameLen = ReadPrimitiveType<std::uint8_t>(is);

			result.name.resize(nameLen);

			is.read(result.name.data(), nameLen * sizeof(char));

			char typeCode;
			for (std::size_t i = 0; i < numProperties; i++)
			{
				typeCode = ReadPrimitiveType<std::uint8_t>(is);

				switch (typeCode)
				{
				case 'Y':
					result.properties[i] = ReadPrimitiveType<std::int16_t>(is);
					break;

				case 'C':
					result.properties[i] = ReadPrimitiveType<bool>(is);
					break;

				case 'I':
					result.properties[i] = ReadPrimitiveType<std::int32_t>(is);
					break;

				case 'F':
					result.properties[i] = ReadPrimitiveType<float>(is);
					break;

				case 'D':
					result.properties[i] = ReadPrimitiveType<double>(is);
					break;

				case 'L':
					result.properties[i] = ReadPrimitiveType<std::int64_t>(is);
					break;

				case 'f':
					result.properties[i] = ReadArrayType<float>(is);
					break;

				case 'd':
					result.properties[i] = ReadArrayType<double>(is);
					break;

				case 'l':
					result.properties[i] = ReadArrayType<std::int64_t>(is);
					break;

				case 'i':
					result.properties[i] = ReadArrayType<std::int32_t>(is);
					break;

				case 'b':
					result.properties[i] = ReadArrayType<unsigned char>(is);
					break;

				case 'S':
					result.properties[i] = ReadStringType(is);
					break;

				case 'R':
					result.properties[i] = ReadRawBinaryType(is);
					break;

				default:
					//std::cout << "\n default \n";
					break;
				}
			}

			if (is.tellg() < endPos)
			{
				std::optional<Node> nestedNode;

				//�q�m�[�h�������_�ŏI�[�����͏ȗ�����Ȃ�
				while (is.tellg() + std::streampos{ 13 } < endPos) {
					nestedNode = LoadNodeImpl<UintType>(is);
					if (nestedNode)
						result.children.push_back(nestedNode.value());
				}
			}

			is.seekg(endPos, std::ios::beg);

			return result;
		}

		template<int N>
		bool CheckNullRecord(std::istream& is)
		{
			std::uint8_t zeroCheck[N];
			is.read(reinterpret_cast<char*>(zeroCheck), N);

			is.seekg(-N, std::ios::cur);

			return std::count(std::begin(zeroCheck), std::end(zeroCheck), 0) == N;
		}

		template<typename UintType, int N>
		std::vector<Node> LoadNode(std::istream& is)
		{
			std::vector<Node> result{};
			std::optional<Node> tmp;

			while (!CheckNullRecord<N>(is))
			{
				tmp = LoadNodeImpl<UintType>(is);
				if (tmp)
					result.push_back(tmp.value());
			}

			return result;
		}


	}


	inline PrimitiveData LoadPrimitiveData(const std::string& filePath)
	{
		PrimitiveData result{};
		result.filePath = filePath;

		std::ifstream is(result.filePath, std::ios::in | std::ios::binary);

		if (!is)
			throw std::invalid_argument("FBXL::LoadFBX   file is not found : " + filePath);

		char magic[21];
		is.read(magic, 21);

		if (strcmp(magic, "Kaydara FBX Binary  \x00\x1a\x00") != 0)
			throw std::invalid_argument("FBXL::LoadFBX   file is not FBX : " + filePath);

		//0xa1�܂���0x00�̕������΂�
		is.seekg(2, std::ios::cur);

		result.version = ReadPrimitiveType<std::uint32_t>(is);

		if (result.version <= 7400)
			result.nodes = LoadNode<std::uint32_t, 13>(is);
		else
			result.nodes = LoadNode<std::uint64_t, 25>(is);

		return result;
	}

}
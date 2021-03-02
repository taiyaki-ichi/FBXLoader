#include"Loader.hpp"

#ifndef ZLIB_WINAPI
#	define ZLIB_WINAPI
#endif

#include<zlib.h>
#include<zconf.h>
#include<iostream>
#include<fstream>
#include<array>

#pragma comment(lib,"zlibstat.lib")




namespace FBXL
{


	namespace {

		template<typename T>
		T ReadPrimitiveType(std::istream& is) 
		{
			T result{};
			is.read(reinterpret_cast<char*>(std::addressof(result)), sizeof(T));
			return result;
		}


		template<typename T>
		void ReadZipCompressedBuffer(std::vector<T>& result, std::vector<unsigned char>&& inBuffer)
		{
			z_stream strm;

			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			strm.avail_in = 0;
			strm.next_in = Z_NULL;

			if (inflateInit(&strm) != Z_OK)
				return;

			constexpr std::size_t CHUNK = 16384;

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
					//inflateを呼び出すごとに更新されてしまうので
					//毎ループ初期化
					strm.avail_out = CHUNK;
					strm.next_out = (Bytef*)tmp.data();

					ret = inflate(&strm, Z_NO_FLUSH);

					have = CHUNK - strm.avail_out;
					
					auto nowPos = result.size();
					result.resize(result.size() + have / sizeof(T));
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
				is.read(reinterpret_cast<char*>(result.data()), arrayLength * sizeof(T));
			}
			else if (encoding == 1)
			{
				std::vector<unsigned char> inBuffer(compressedLength);
				is.read(reinterpret_cast<char*>(inBuffer.data()), compressedLength * sizeof(unsigned char));
				ReadZipCompressedBuffer(result, std::move(inBuffer));
			}

			return result;
		}

		std::string ReadStringType(std::istream& is)
		{
			auto length = ReadPrimitiveType<std::uint32_t>(is);

			std::string result{};
			result.resize(length);

			is.read(result.data(), length * sizeof(char));

			return result;
		}

		std::vector<unsigned char> ReadRawBinaryType(std::istream& is)
		{
			auto length = ReadPrimitiveType<std::uint32_t>(is);

			std::vector<unsigned char> result(length);

			is.read(reinterpret_cast<char*>(result.data()), length * sizeof(unsigned char));

			return result;
		}



		std::optional<Node> ReadNode(std::istream& is)
		{
			Node result{};

			//std::cout << "nowPos : " << is.tellg() << "\n";
			//std::streampos endPos = is.tellg();
			//endPos += ReadPrimitiveType<std::uint32_t>(is);
			std::streampos endPos{ ReadPrimitiveType<std::uint32_t>(is) };
			//std::cout << "endPos : " << endPos << "\n";

			auto numProperties = ReadPrimitiveType<std::uint32_t>(is);
			//std::cout << "numProperties : " << numProperties << " pos : " << is.tellg() << "\n";

			result.mDatas.resize(numProperties);

			auto propertyListLen = ReadPrimitiveType<std::uint32_t>(is);
			//std::cout << "propertyListLen : " << propertyListLen << " pos : " << is.tellg() << "\n";

			

			auto nameLen = ReadPrimitiveType<std::uint8_t>(is);
			//std::cout << "nameLen : " << static_cast<unsigned int>(nameLen) << " pos : " << is.tellg() << "\n";

			result.mName.resize(nameLen);

			is.read(result.mName.data(), nameLen * sizeof(char));
			//std::cout << "name : " << result.mName << " pos : " << is.tellg() << "\n";

			char typeCode;
			for (std::size_t i = 0; i < numProperties; i++)
			{
				auto pos = is.tellg();
				typeCode = ReadPrimitiveType<std::uint8_t>(is);
				//std::cout << "typecode : " << typeCode << " pos " << is.tellg() << " ";

				//is.seekg(pos + std::streampos{ 1 }, std::ios::beg);
				//std::cout << "typecode : " << typeCode << " pos " << is.tellg() << " ";

				/*
				if (is.tellg() < 0 || endPos <= is.tellg()) {
					is.seekg(endPos, std::ios::beg);
					return std::nullopt;
				}
				*/

				std::string str;
				switch (typeCode)
				{
				case 'Y' :
					result.mDatas[i] = ReadPrimitiveType<std::int16_t>(is);
					break;

				case 'C' :
					result.mDatas[i] = ReadPrimitiveType<bool>(is);
					break;

				case 'I' :
					result.mDatas[i] = ReadPrimitiveType<std::int32_t>(is);
					break;

				case 'F' : 
					result.mDatas[i] = ReadPrimitiveType<float>(is);
					break;

				case 'D' :
					result.mDatas[i] = ReadPrimitiveType<double>(is);
					break;

				case 'L' :
					result.mDatas[i] = ReadPrimitiveType<std::int64_t>(is);
					break;

				case 'f' :
					result.mDatas[i] = ReadArrayType<float>(is);
					break;

				case 'd' :
					result.mDatas[i] = ReadArrayType<double>(is);
					break;

				case 'l' :
					result.mDatas[i] = ReadArrayType<std::int64_t>(is);
					break;

				case 'i' :
					result.mDatas[i] = ReadArrayType<std::int32_t>(is);
					break;

				case 'b' :
					//result.mDatas[i] = ReadArrayType<bool>(is);
					result.mDatas[i] = ReadArrayType<unsigned char>(is);
					break;

				case 'S' :
					str = ReadStringType(is);
					//std::cout << str << " ";
					result.mDatas[i] = std::move(str);// ReadStringType(is);
					break;

				case 'R' : 
					result.mDatas[i] = ReadRawBinaryType(is);
					break;

				default:
					//std::cout << "\n default \n";
					break;
				}
				//std::cout << "pos : " << is.tellg() << " ";
			}
			//std::cout << "\n";

			//std::cout << "nest check (" << is.tellg() << " + 13 < " << endPos << ")\n";
			//std::cout << "{\n";

			//バージョンによっては13ではない可能性が？？
			if (is.tellg()  /*+ std::streampos{ 13 }*/ < endPos)
			{

				std::optional<Node> nestedNode;

				//子ノードを持つ時点で終端文字は省略されない
				while (is.tellg()  + std::streampos{ 13 } < endPos) {
					//std::cout << "nest check (" << is.tellg() << " < " << endPos << ")\n";
					nestedNode = ReadNode(is);
					if (nestedNode)
						result.mNestedNode.push_back(nestedNode.value());
				}


				//is.seekg(13, std::ios::cur);
				//is.seekg(endPos, std::ios::beg);
			}
			else if (numProperties < 1)
			{
				//is.seekg(13, std::ios::cur);
				//is.seekg(endPos, std::ios::beg);
			}
			//std::cout << "}\n";

			is.seekg(endPos, std::ios::beg);

			
			return result;
		}
	}


	std::vector<Node> Load(const char* fileName)
	{
		std::ifstream is(fileName, std::ios::in | std::ios::binary);

		if (!is) {
			std::cout << "FBXL::load is failed (filename : " << fileName << " )\n";
			//return std::nullopt;
			return {};
		}

		//std::cout << "pos : " << is.tellg() << "\n";
		char magic[21];
		is.read(magic, 21);

		//0xa1または0x00の部分を飛ばす
		is.seekg(2, std::ios::cur);

		auto version = ReadPrimitiveType<std::uint32_t>(is);
		//std::cout << "version : " << version << "\n\n";


		std::vector<Node> result{};
		std::optional<Node> tmp;

		std::uint8_t zeroCheck[13];

		while (true)
		{	

			tmp = ReadNode(is);
			if (tmp)
				result.push_back(tmp.value());


			is.read(reinterpret_cast<char*>(zeroCheck), 13);
			bool isZero = true;
			for (int i = 0; i < 13; i++)
				if (zeroCheck[i] != 0x00)
					isZero = false;

			if (isZero)
				break;
			else
				is.seekg(-13, std::ios::cur);
		}

		return result;
	}
}
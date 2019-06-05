#include "pch.h"
#include "Memory.h"
#include "JsonReflector.h"
#include <fstream>

JsonStructs JsonReflector::StructsList;
nlohmann::json JsonReflector::JsonObj;

#pragma region JsonReflector
bool JsonReflector::ReadJsonFile(const std::string& fileName, void* jsonObj)
{
	// read a JSON file
	std::ifstream i(fileName.c_str());
	if (!i.good()) return false;
	i >> *reinterpret_cast<nlohmann::json*>(jsonObj);
	return true;
}

bool JsonReflector::ReadJsonFile(const std::string& fileName)
{
	return ReadJsonFile(fileName, &JsonObj);
}

JsonStruct JsonReflector::GetStruct(const std::string& structName)
{
	auto s = StructsList.find(structName);
	if (s != StructsList.end())
		return s->second;

	throw std::exception(("Can't find " + structName + " in loaded structs.").c_str());
}

bool JsonReflector::LoadStruct(const std::string& structName, const bool overrideOld)
{
	// Check is already here
	if (StructsList.find(structName) != StructsList.end() && !overrideOld) return true;
	auto j = JsonObj; // Don't make it reference var

	for (const auto& parent : j.at("structs").items())
	{
		const std::string eName = parent.value().at("name");
		if (eName == structName)
		{
			JsonStruct tempToSave;
			JsonVariables vars;
			int structSize = 0;
			int offset = 0;

			// Get Super
			const std::string eSuper = parent.value().at("super");
			if (!eSuper.empty())
			{
				tempToSave.StructSuper = eSuper;

				// Add super struct Variables to struct first
				if (LoadStruct(eSuper, overrideOld))
				{
					auto s = StructsList.find(eSuper);
					if (s != StructsList.end())
					{
						for (const auto& var : s->second.Vars)
						{
							JsonVar variable = var.second;

							const std::string& name = variable.Name;
							std::string type = variable.Type;

							auto jVar = JsonVar(name, type, offset, IsStructType(type));
							vars.push_back({ name , jVar });

							offset += jVar.Size;
						}

						structSize += s->second.StructSize;
					}
				}
				else
				{
					throw std::exception(("Can't find `" + eSuper + "` Struct.").c_str());
				}
			}

			// Init vars
			auto element = parent.value().at("vars");
			for (const auto& var : element.items())
			{
				nlohmann::json::iterator it = var.value().begin();

				if (it.value().is_number_unsigned())
					structSize += static_cast<int>(var.value());
				else
					structSize += VarSizeFromJson(it.value(), overrideOld);

				const std::string& name = it.key();
				std::string type = it.value();

				JsonVar jVar(name, type, offset, IsStructType(type));
				vars.push_back({ name , jVar });


				offset += jVar.Size;
			}

			// Init struct
			tempToSave.StructName = eName;
			tempToSave.Vars = vars;
			tempToSave.StructSize = structSize;

			// [Copy] the struct to structs list
			{
				// Override old struct
				if (overrideOld)
				{
					// check if it in the list
					auto foundIt = StructsList.find(eName);
					if (foundIt != StructsList.end())
					{
						*foundIt = { eName, tempToSave };
					}
				}

				// check if it not in the list
				if (StructsList.find(eName) == StructsList.end())
					StructsList.push_back({ eName, tempToSave });
			}

			return true;
		}
	}

	// if code hit here then there is no override in the `override engine` file
	// so by return true i just give it the struct from `EngineBase` file, because it already loaded !!
	return overrideOld;
}

bool JsonReflector::Load(void* jsonObj, const bool overrideOld)
{
	auto jObj = reinterpret_cast<nlohmann::json*>(jsonObj);
	auto j = *jObj;

	for (const auto& parent : j.at("structs").items())
	{
		const std::string eName = parent.value().at("name");
		// Check is already here
		if (StructsList.find(eName) != StructsList.end() && !overrideOld) continue;

		JsonStruct tempToSave;
		JsonVariables vars;
		int structSize = 0;
		int offset = 0;

		// Get Super
		const std::string eSuper = parent.value().at("super");
		if (!eSuper.empty())
		{
			tempToSave.StructSuper = eSuper;

			// Add super struct Variables to struct first
			if (LoadStruct(eSuper, overrideOld))
			{
				auto s = StructsList.find(eSuper);
				if (s != StructsList.end())
				{
					for (const auto& var : s->second.Vars)
					{
						JsonVar variable = var.second;

						const std::string& name = variable.Name;
						std::string type = variable.Type;

						auto jVar = JsonVar(name, type, offset, IsStructType(type));
						vars.push_back({ name , jVar });

						offset += jVar.Size;
					}

					structSize += s->second.StructSize;
				}
			}
			else
			{
				throw std::exception(("Can't find `" + eSuper + "` Struct.").c_str());
			}
		}

		// Init vars
		auto element = parent.value().at("vars");
		for (const auto& var : element.items())
		{
			nlohmann::json::iterator it = var.value().begin();

			if (it.value().is_number_unsigned())
				structSize += static_cast<int>(var.value());
			else
				structSize += VarSizeFromJson(it.value(), overrideOld); // VarSizeFromJson Load other struct if not loaded

			const std::string& name = it.key();
			std::string type = it.value();

			auto jVar = JsonVar(name, type, offset, IsStructType(type));
			vars.push_back({ name , jVar });

			offset += jVar.Size;
		}

		// Init struct
		tempToSave.StructName = eName;
		tempToSave.Vars = vars;
		tempToSave.StructSize = structSize;

		// [Copy] the struct to structs list
		{
			// Override old struct
			if (overrideOld)
			{
				// check if it in the list
				auto foundIt = StructsList.find(eName);
				if (foundIt != StructsList.end())
				{
					*foundIt = { eName, tempToSave };
				}
			}

			// check if it not in the list
			if (StructsList.find(eName) == StructsList.end())
				StructsList.push_back({ eName, tempToSave });
		}
	}
	return true;
}

bool JsonReflector::Load(const bool overrideOld)
{
	return Load(&JsonObj, overrideOld);
}

bool JsonReflector::ReadAndLoadFile(const std::string& fileName, void* jsonObj, const bool overrideOld)
{
	return ReadJsonFile(fileName, jsonObj) && Load(jsonObj, overrideOld);
}

bool JsonReflector::ReadAndLoadFile(const std::string& fileName, const bool overrideOld)
{
	return ReadAndLoadFile(fileName, reinterpret_cast<void*>(&JsonObj), overrideOld);
}

int JsonReflector::VarSizeFromJson(const std::string& typeName, const bool overrideOld)
{
	if (typeName == "int8")
		return sizeof(int8_t);
	if (typeName == "int16")
		return sizeof(int16_t);
	if (typeName == "int" || typeName == "int32")
		return sizeof(int32_t);
	if (typeName == "int64")
		return sizeof(int64_t);

	if (typeName == "uint8")
		return sizeof(uint8_t);
	if (typeName == "uint16")
		return sizeof(uint16_t);
	if (typeName == "uint" || typeName == "uint32")
		return sizeof(uint32_t);
	if (typeName == "uint64")
		return sizeof(uint64_t);

	if (Utils::EndsWith(typeName, "*")) // pointer
		return sizeof(uintptr_t);
	if (typeName == "DWORD")
		return sizeof(DWORD);
	if (typeName == "DWORD64")
		return sizeof(DWORD64);
	if (typeName == "string")
		return sizeof(uintptr_t);

	// Other type (usually) structs
	if (Utils::IsNumber(typeName))
		return std::stoi(typeName);

	if (IsStructType(typeName))
	{
		if (LoadStruct(typeName, overrideOld))
		{
			auto s = StructsList.find(typeName);
			if (s != StructsList.end())
				return s->second.StructSize;
		}
		throw std::exception(("Cant find struct `" + typeName + "`.").c_str());
	}
	throw std::exception(("Cant detect size of `" + typeName + "`.").c_str());
}

bool JsonReflector::IsStructType(const std::string& typeName)
{
	const bool isStruct =
		typeName == "int8" ||
		typeName == "int16" ||
		typeName == "int" ||
		typeName == "int32" ||
		typeName == "int64" ||

		typeName == "uint8" ||
		typeName == "uint16" ||
		typeName == "uint" ||
		typeName == "uint32" ||
		typeName == "uint64" ||

		Utils::EndsWith(typeName, "*") || // pointer
		typeName == "DWORD" ||
		typeName == "DWORD64" ||
		typeName == "string" ||

		Utils::IsNumber(typeName);
	return !isStruct;
}
#pragma endregion

#pragma region JsonStruct
int JsonStruct::SubUnNeededSize()
{
	int sSub = 0;
	if (Utils::ProgramIs64() && !Utils::MemoryObj->Is64Bit)
	{
		// if it's 32bit game (4byte pointer) sub 4byte for every pointer
		for (auto& var : Vars)
		{
			if (Utils::EndsWith(var.second.Type, "*"))
				sSub += 0x4;
			else if (var.second.IsStruct)
			{
				if (var.second.Struct == nullptr)
					var.second.ReadAsStruct();

				if (var.second.Struct != nullptr)
					sSub += var.second.Struct->SubUnNeededSize();
			}
		}
	}

	return sSub;
}

JsonVar& JsonStruct::operator[](const std::string& varName)
{
	return GetVar(varName);
}

JsonVar& JsonStruct::GetVar(const std::string& varName)
{
	if (Vars.find(varName) != Vars.end())
		return Vars.find(varName)->second;

	throw std::exception(("Not found " + varName + " in JsonVariables").c_str());
}
#pragma endregion

#pragma region JsonVar
JsonVar::JsonVar(const std::string& name, const std::string& type, const int offset, const bool isStruct) : Struct(nullptr)
{
	Name = name;
	Type = type;
	Size = JsonReflector::VarSizeFromJson(Type, false);
	Offset = offset;
	IsStruct = isStruct;
}

JsonVar::~JsonVar()
{
	if (IsStruct && Struct != nullptr)
		delete Struct;
}

JsonVar& JsonVar::operator[](const std::string& varName)
{
	if (!IsStruct)
		throw std::exception((Name + " not a struct.").c_str());

	return GetVar(varName);
}

JsonVar& JsonVar::GetVar(const std::string& varName)
{
	if (!IsStruct)
		throw std::exception((Name + " not a struct.").c_str());

	if (Struct == nullptr)
		ReadAsStruct();

	if (Struct != nullptr)
	{
		if (Struct->Vars.find(varName) != Struct->Vars.end())
			return Struct->Vars.find(varName)->second;
	}

	throw std::exception(("Not found " + varName + " in JsonVariables").c_str());
}

JsonStruct* JsonVar::ReadAsStruct()
{
	if (!IsStruct)
		throw std::exception((Name + " Not a struct.").c_str());

	if (Struct != nullptr)
		return Struct;

	auto sStructIt = JsonReflector::StructsList.find(Type);
	if (sStructIt == JsonReflector::StructsList.end())
		throw std::exception(("Can't find struct When try read as " + Type).c_str());

	Struct = new JsonStruct(sStructIt->second);
	return Struct;
}
#pragma endregion

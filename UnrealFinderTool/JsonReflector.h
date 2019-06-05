#pragma once
#include "json.hpp"
#include "UnsortedMap.h"

class JsonVar;
class JsonStruct;
class Memory;

using JsonStructs = UnsortedMap<std::string, JsonStruct>;
using JsonVariables = UnsortedMap<std::string, JsonVar>;

class JsonReflector
{
public:
	// Main json reader for structs
	static nlohmann::json JsonObj;
	// Contains all loaded structs
	static JsonStructs StructsList;
	// Read Json file into memory to be ready to load structs inside them
	static bool ReadJsonFile(const std::string& fileName, void* jsonObj);
	// Read Json file into memory to be ready to load structs inside them, [Using main `JsonObj`]
	static bool ReadJsonFile(const std::string& fileName);
	// Check variable type is struct
	static bool IsStructType(const std::string& typeName);
	// Read struct form loaded json structs
	static JsonStruct GetStruct(const std::string& structName);
	// Load all json structs inside `StructsList`
	static bool Load(void* jsonObj, bool overrideOld = false);
	// Load all json structs inside `StructsList`, [Using main `JsonObj`]
	static bool Load(bool overrideOld = false);
	// Read Json file into memory AND read all structs inside json file, then store structs inside `StructsList`
	static bool ReadAndLoadFile(const std::string& fileName, void* jsonObj, bool overrideOld = false);
	// Read Json file into memory AND read all structs inside json file, then store structs inside `StructsList`, [Using main `JsonObj`]
	static bool ReadAndLoadFile(const std::string& fileName, bool overrideOld = false);
	// Load json struct by name
	static bool LoadStruct(const std::string& structName, bool overrideOld = false);
	// Get json struct variable size
	static int VarSizeFromJson(const std::string& typeName, bool overrideOld);
};

class JsonVar
{
public:
	// Variable Name
	std::string Name;
	// Variable Type
	std::string Type;
	// Variable Size
	int Size = 0;
	// Variable offset of his parent
	int Offset = 0;
	// Variable is struct
	bool IsStruct = false;
	// If this variable is struct this is pointer to struct contains variables
	JsonStruct* Struct;

	JsonVar(const std::string& name, const std::string& type, int offset, bool isStruct);
	~JsonVar();

	// Access variable inside this variable, ONLY work if this variable is struct
	JsonVar& operator[](const std::string& varName);
	// Read variable as struct, ONLY work if this variable is struct [NOT POINTER TO STRUCT]
	JsonStruct* ReadAsStruct();
	// Read variable as [derived] struct, ONLY work if this variable is struct [NOT POINTER TO STRUCT]
	template <typename T>
	T ReadAsStruct()
	{
		T tmp = *reinterpret_cast<T*>(ReadAsStruct());
		tmp.FixStructData();
		tmp.InitData();
		return tmp;
	}
private:
	// Access variable in this variable if this variable is struct
	JsonVar& GetVar(const std::string& varName);
};

class JsonStruct
{
	bool _init = false;
public:
	// Struct Name
	std::string StructName;
	// Super Name
	std::string StructSuper;
	// Variables inside this struct
	JsonVariables Vars;
	// Size of this struct
	int StructSize = 0;

	// Get size must sub from struct size, useful for 32bit games in 64bit version of this tool
	int SubUnNeededSize();
	// Access to variable inside this struct
	JsonVar& operator[](const std::string& varName);
	// Access to variable inside this struct
	JsonVar& GetVar(const std::string& varName);
};
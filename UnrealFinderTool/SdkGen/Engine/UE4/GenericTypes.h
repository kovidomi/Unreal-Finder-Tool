#pragma once
#include <string>
#include <vector>
#include <array>

#include "PropertyFlags.h"
#include "FunctionFlags.h"
#include "IGenerator.h"
#include "SdkGen/EngineClasses.h"
#include "Utils.h"

class UEClass;

class UEObject
{
protected:
	mutable UClass objClass;
	mutable uintptr_t packageAddress = NULL;
	mutable std::string objName, fullName, nameCpp;

public:
	UObject Object;

	UEObject() = default;
	UEObject(const UObject object) : Object(object) { }

	uintptr_t GetAddress() const;
	bool IsValid() const;
	size_t GetIndex() const;

	std::string GetName() const;
	std::string GetInstanceClassName() const;
	std::string GetFullName() const;
	std::string GetNameCPP() const;

	UEClass GetClass() const;
	UEObject& GetOuter() const;
	UEObject& GetPackageObject() const;

	template<typename Base>
	Base Cast() const
	{
		return Base(Object);
	}

	template<typename T>
	bool IsA() const;

	// Check type in target process (Remote check type)
	bool IsA(const std::string& typeName) const;


	static std::string TypeName();

	static UEClass StaticClass();
};

namespace std
{
	template<>
	struct hash<UEObject>
	{
		size_t operator()(const UEObject& obj) const noexcept
		{
			return std::hash<uintptr_t>()(obj.GetAddress());
		}
	};
}

inline UEObject UEObjectEmpty; // IsValid Will Return False for it
inline bool operator==(const UEObject& lhs, const UEObject& rhs) { return rhs.GetAddress() == lhs.GetAddress(); }
inline bool operator!=(const UEObject& lhs, const UEObject& rhs) { return !(lhs.GetAddress() == rhs.GetAddress()); }

class UEField : public UEObject
{
	mutable UField objField;
	mutable UField next;

public:
	using UEObject::UEObject;

	UEField GetNext() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEEnum : public UEField
{
	mutable UEnum objEnum;

public:
	using UEField::UEField;

	std::vector<std::string> GetNames() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEConst : public UEField
{
public:
	using UEField::UEField;

	std::string GetValue() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEStruct : public UEField
{
	mutable UStruct objStruct;
	mutable UStruct superField;
	mutable UField children;

public:
	using UEField::UEField;

	UEStruct GetSuper() const;

	UEField GetChildren() const;

	size_t GetPropertySize() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEScriptStruct : public UEStruct
{
public:
	using UEStruct::UEStruct;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEFunction : public UEStruct
{
	mutable UFunction objFunction;

public:
	using UEStruct::UEStruct;

	UEFunctionFlags GetFunctionFlags() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEClass : public UEStruct
{
public:
	using UEStruct::UEStruct;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEProperty : public UEField
{
public:
	enum class PropertyType
	{
		Unknown,
		Primitive,
		PredefinedStruct,
		CustomStruct,
		Container
	};

	struct Info
	{
		PropertyType Type = PropertyType::Unknown;
		size_t Size = 0;
		bool CanBeReference;
		std::string CppType = "";

		static Info Create(PropertyType type, size_t size, bool reference, std::string&& cppType)
		{
			extern IGenerator* generator;

			return { type, size, reference, generator->GetOverrideType(cppType) };
		}
	};

private:
	mutable UProperty objProperty;
	mutable bool infoChanged = false;
	mutable Info curInfo;

public:
	using UEField::UEField;

	size_t GetArrayDim() const;

	size_t GetElementSize() const;

	UEPropertyFlags GetPropertyFlags() const;

	size_t GetOffset() const;

	Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UENumericProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEByteProperty : public UENumericProperty
{
	mutable UByteProperty objByteProperty;
	mutable UEnum enumProperty;

public:
	using UENumericProperty::UENumericProperty;

	bool IsEnum() const;

	UEEnum GetEnum() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEUInt16Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEUInt32Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEUInt64Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEInt8Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEInt16Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEIntProperty : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEInt64Property : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEFloatProperty : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	static std::string TypeName();

	UEProperty::Info GetInfo() const;

	static UEClass StaticClass();
};

class UEDoubleProperty : public UENumericProperty
{
public:
	using UENumericProperty::UENumericProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEBoolProperty : public UEProperty
{
	mutable UBoolProperty objBoolProperty;

public:
	using UEProperty::UEProperty;

	bool IsNativeBool() const { return GetFieldMask() == 0xFF; }

	bool IsBitfield() const { return !IsNativeBool(); }

	uint8_t GetFieldSize() const;

	uint8_t GetByteOffset() const;

	uint8_t GetByteMask() const;

	uint8_t GetFieldMask() const;

	std::array<int, 2> GetMissingBitsCount(const UEBoolProperty & other) const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

inline bool operator<(const UEBoolProperty& lhs, const UEBoolProperty& rhs)
{
	if (lhs.GetByteOffset() == rhs.GetByteOffset())
	{
		return lhs.GetByteMask() < rhs.GetByteMask();
	}
	return lhs.GetByteOffset() < rhs.GetByteOffset();
}

class UEObjectPropertyBase : public UEProperty
{
	mutable UObjectPropertyBase objObjectPropertyBase;
	mutable UObjectPropertyBase propertyClass;

public:
	using UEProperty::UEProperty;

	UEClass GetPropertyClass() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEObjectProperty : public UEObjectPropertyBase
{
public:
	using UEObjectPropertyBase::UEObjectPropertyBase;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEClassProperty : public UEObjectProperty
{
	mutable UClassProperty objClassProperty;
	mutable UClassProperty metaClass;

public:
	using UEObjectProperty::UEObjectProperty;

	UEClass GetMetaClass() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEInterfaceProperty : public UEProperty
{
	mutable UInterfaceProperty objInterfaceProperty;
	mutable UInterfaceProperty interfaceClass;

public:
	using UEProperty::UEProperty;

	UEClass GetInterfaceClass() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEWeakObjectProperty : public UEObjectPropertyBase
{
public:
	using UEObjectPropertyBase::UEObjectPropertyBase;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UELazyObjectProperty : public UEObjectPropertyBase
{
public:
	using UEObjectPropertyBase::UEObjectPropertyBase;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEAssetObjectProperty : public UEObjectPropertyBase
{
public:
	using UEObjectPropertyBase::UEObjectPropertyBase;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEAssetClassProperty : public UEAssetObjectProperty
{
	mutable UAssetClassProperty objAssetClassProperty;
	mutable UClass metaClass;

public:
	using UEAssetObjectProperty::UEAssetObjectProperty;

	UEClass GetMetaClass() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UENameProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEStructProperty : public UEProperty
{
	mutable UStructProperty objStructProperty;
	mutable UStructProperty objStruct;

public:
	using UEProperty::UEProperty;

	UEScriptStruct GetStruct() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEStrProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UETextProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEArrayProperty : public UEProperty
{
	mutable UArrayProperty objArrayProperty;
	mutable UArrayProperty inner;

public:
	using UEProperty::UEProperty;

	UEProperty GetInner() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEMapProperty : public UEProperty
{
	mutable UMapProperty objMapProperty;
	mutable UMapProperty keyProp, valueProp;

public:
	using UEProperty::UEProperty;

	UEProperty GetKeyProperty() const;
	UEProperty GetValueProperty() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEDelegateProperty : public UEProperty
{
	mutable UDelegateProperty objDelegateProperty;
	mutable UDelegateProperty signatureFunction;

public:
	using UEProperty::UEProperty;

	UEFunction GetSignatureFunction() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEMulticastDelegateProperty : public UEProperty
{
	mutable UDelegateProperty objDelegateProperty;
	mutable UDelegateProperty signatureFunction;

public:
	using UEProperty::UEProperty;

	UEFunction GetSignatureFunction() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

class UEEnumProperty : public UEProperty
{
	mutable UEnumProperty objEnumProperty;
	mutable UEnumProperty underlyingProp;
	mutable UEnumProperty Enum;

public:
	using UEProperty::UEProperty;

	UENumericProperty GetUnderlyingProperty() const;
	UEEnum GetEnum() const;

	UEProperty::Info GetInfo() const;

	static std::string TypeName();

	static UEClass StaticClass();
};

template<typename T>
bool UEObject::IsA() const
{
	if (!IsValid()) return false;

	std::string cmpTypeName = T::TypeName();
	for (UEClass super = GetClass(); super.IsValid(); super = super.GetSuper().Cast<UEClass>())
	{
		if (super.GetName() == cmpTypeName)
			return true;
	}
	return false;
}
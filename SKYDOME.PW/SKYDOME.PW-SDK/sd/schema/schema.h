#pragma once
#include "../utilities/fnv1a.h"


struct SchemaData_t
{
	FNV1A_t uHashedFieldName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};


class SchemaManager
{
public:
	bool init(const char* module);
	[[nodiscard]] std::uint32_t GetOffset(const std::string name);
	
private:
	std::vector<SchemaData_t> vecSchemaData;
	std::unordered_map<std::string, std::uint32_t> offsets;
};




inline SchemaManager* g_SchemaManager = new SchemaManager;


#define SCHEMA_ADD_OFFSET(TYPE, NAME, OFFSET)                                                                 \
	[[nodiscard]] __forceinline std::add_lvalue_reference_t<TYPE> NAME()                                          \
	{                                                                                                         \
		static const std::uint32_t uOffset = OFFSET;                                                          \
		return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

//÷∏’Î
#define SCHEMA_ADD_POFFSET(TYPE, NAME, OFFSET)                                                               \
	[[nodiscard]] __forceinline std::add_pointer_t<TYPE> NAME()                                                  \
	{                                                                                                        \
		const static std::uint32_t uOffset = OFFSET;                                                         \
		return reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

#define SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_OFFSET(TYPE, NAME, g_SchemaManager->GetOffset(FIELD) + ADDITIONAL)

#define SCHEMA_ADD_FIELD(TYPE, NAME, FIELD) SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, 0U)

#define SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_POFFSET(TYPE, NAME, g_SchemaManager->GetOffset(FIELD) + ADDITIONAL)

#define SCHEMA_ADD_PFIELD(TYPE, NAME, FIELD) SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, 0U)
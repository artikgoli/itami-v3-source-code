#pragma once
#define ADD_SCHEMA(TYPE, NAME, VARIABLE, ADDITIONAL) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
	static std::uintptr_t uOffset = schema_system->map_offsets[FNV1A::HashConst(VARIABLE)]; \
	return memory->read<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset + ADDITIONAL); \
}

#define ADD_OFFSET(TYPE, NAME, OFFSET) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
	static std::uintptr_t uOffset = OFFSET; \
	return memory->read<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset); \
}

#define SCHEMA_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) ADD_SCHEMA(TYPE, NAME, FIELD, ADDITIONAL)
#define SCHEMA(TYPE, NAME, FIELD) ADD_SCHEMA(TYPE, NAME, FIELD, 0U)
#define OFFSET(TYPE, NAME, OFFSET) ADD_OFFSET(TYPE, NAME, OFFSET)

#define OFFSET_FROM(NAME, VARIABLE) \
	static std::uintptr_t NAME = schema_system->map_offsets[FNV1A::HashConst(VARIABLE)]; \

class c_schema_system 
{
public:
	bool init();

	std::unordered_map<FNV1A_t, std::uint32_t> map_offsets;
private:
	struct sdk_field
	{
		std::string name;
		std::size_t offset;
	};

	struct sdk_class
	{
		std::string name;
		std::vector<sdk_field> fields;
	};

	std::unordered_map<std::string, sdk_class> client_classes;
};

inline auto schema_system = std::make_shared<c_schema_system>( );
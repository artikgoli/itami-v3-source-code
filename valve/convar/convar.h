#pragma once

class c_convar_manager {
public:
	template <typename T>
	T get( const std::string& name );
	uintptr_t find(const std::string& name);

	void set_monitored_cvars( );

	void init();     
	void update();

private:
	std::unordered_map<FNV1A_t, uintptr_t> cvar_map;
	std::vector<std::string> monitored_cvars;

};

template<typename T>
inline T c_convar_manager::get( const std::string& name )
{
	uintptr_t cvar_address = find( name );
	if ( !cvar_address )
		return false;

	return memory->read<T>( cvar_address + 0x58 );
}

inline auto convar = std::make_shared<c_convar_manager>();

enum e_cvar_flags : int
{
	FCVAR_NONE = 0,
	FCVAR_DEVELOPMENTONLY = (1 << 1),
	FCVAR_HIDDEN = (1 << 4),
};

union cvar_value_t
{
	bool i1;
	short i16;
	uint16_t u16;
	int i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	float fl;
	double db;
};


struct cvar_t
{
public:
	std::string get_name()
	{
		if (m_name_address)
		{
			std::string out = "";

			for (int i = 0; i < 256; i++)
			{
				char character = memory->read<char>(m_name_address + i);

				out += character;

				if (character == '\0')
					break;
			}

			return out;
		}

		return ("<invalid>");
	}

	cvar_value_t get_default_value()
	{
		if (m_default_value_address)
		{
			return memory->read<cvar_value_t>(m_default_value_address);
		}

		return cvar_value_t(0);
	}

	cvar_value_t get_min_value()
	{
		if (m_min_value_address)
		{
			return memory->read<cvar_value_t>(m_min_value_address);
		}

		return cvar_value_t(0);
	}

	cvar_value_t get_max_value()
	{
		if (m_max_value_address)
		{
			return memory->read<cvar_value_t>(m_max_value_address);
		}

		return cvar_value_t(0);
	}

	uintptr_t m_name_address; 
	uintptr_t m_default_value_address; 

	uintptr_t m_min_value_address; 
	uintptr_t m_max_value_address; 

	uintptr_t m_description_address; 

	int16_t m_type; 
	short m_version;
	unsigned int m_times_changed;
	uint64_t m_flags; 

	unsigned int m_callback_index; 

	int m_game_info_flags; 
	int m_user_info_byte_index; 

	alignas(cvar_value_t) cvar_value_t m_value; 
};

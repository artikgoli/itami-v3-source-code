#pragma once
#define m_game_build 14141

struct module_t
{
	uintptr_t base{};
	module_info_t mod{};
	const char* name{};

	explicit module_t(const char* n) : name(n) {}

	void resolve()
	{
		mod = memory->get_module(name);
		base = mod.m_uBaseAddress;
	}

	uintptr_t add(uintptr_t offset) const
	{
		return base + offset;
	}
};

struct signature_t
{
	uintptr_t address = {};
	signature_t( uintptr_t address ) : address( address )
	{
	}
	signature_t( ) = default;
};

class c_sdk
{
public:

	signature_t schema_system {};
	signature_t convars {};
	signature_t dw_global_vars {};
	signature_t local_controller {};
	signature_t view_matrix {};

	void init();

	module_t& client_dll() { return this->m_client; }
	module_t& engine2_dll() { return this->m_engine2; }
	module_t& tier0_dll() { return this->m_tier0; }
	module_t& schemasystem_dll() { return this->m_schema; }

private:
	module_t m_client{ "client.dll" };
	module_t m_engine2{ "engine2.dll" };
	module_t m_tier0{ "tier0.dll" };
	module_t m_schema{ "schemasystem.dll" };
};

// shared bo to jest, no kurwa shared
// a nie unique
inline auto sdk = std::make_shared<c_sdk>();
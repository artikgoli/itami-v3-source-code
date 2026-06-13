
bool c_schema_system::init()
{
    std::uintptr_t schema_interface_address = sdk->schema_system.address;

    std::uintptr_t schema_system_scope_array_ptr = 0U;
    if (!memory->read_raw(schema_interface_address + CS_OFFSETOF(CSchemaSystem, m_pScopeArray), &schema_system_scope_array_ptr, sizeof(std::uintptr_t)))
    {
        log_dbg("failed to read scope array ptr");
        return false;
    }

    int scope_size = memory->read<int>(schema_interface_address + CS_OFFSETOF(CSchemaSystem, m_nScopeSize));
    void** scope_array = new void* [scope_size];

    if (!memory->read_raw(schema_system_scope_array_ptr,scope_array,scope_size * sizeof(void*)))
    {
        log_dbg("failed to read scope array");
        return false;
    }

    for (std::uint16_t i = 0U; i <= scope_size; ++i)
    {
        CSchemaSystemTypeScope schema_scope{};

        if (!memory->read_raw(scope_array[i], &schema_scope, sizeof(CSchemaSystemTypeScope)) || !schema_scope.m_pDeclaredClasses)
        {
            continue;
        }

        CSchemaDeclaredClassEntry* declared_class_entries = new CSchemaDeclaredClassEntry[schema_scope.m_uNumDeclaredClasses + 1U];

        if (!memory->read_raw(schema_scope.m_pDeclaredClasses, declared_class_entries, (schema_scope.m_uNumDeclaredClasses + 1U) * sizeof(CSchemaDeclaredClassEntry)))
        {
            continue;
        }

        if (strcmp(schema_scope.m_szName, X("client.dll")) == 0)
        {
            for (std::uint16_t j = 0U; j < schema_scope.m_uNumDeclaredClasses; ++j)
            {
                CSchemaDeclaredClass declared_class{};
                if (!memory->read_raw(declared_class_entries[j].m_pDeclaredClass, &declared_class, sizeof(CSchemaDeclaredClass)))
                {
                    continue;
                }

                CSchemaClass schema_class{};
                if (!memory->read_raw(declared_class.m_Class, &schema_class, sizeof(CSchemaClass)))
                {
                    continue;
                }

                char class_name[128]{};
                if (!memory->read_raw((void*)declared_class.m_szName, class_name, sizeof(class_name)))
                {
                    continue;
                }

                std::string class_name_str = class_name;

                std::uintptr_t class_fields_ptr = reinterpret_cast<std::uintptr_t>(schema_class.m_pFields);

                if (class_fields_ptr)
                {
                    for (std::uint16_t k = 0; k < schema_class.m_uNumFields; ++k)
                    {
                        CSchemaField schema_field = memory->read<CSchemaField>(class_fields_ptr + (sizeof(CSchemaField) * k));

                        if (!schema_field.m_pType)
                            continue;

                        char field_name[128]{};
                        if (!memory->read_raw((void*)schema_field.m_szName,field_name,sizeof(field_name)))
                        {
                            continue;
                        }

                        const std::string schema_field_name = std::vformat(X("{}->{}"),std::make_format_args(class_name, field_name));

                        map_offsets[FNV1A::Hash(schema_field_name.c_str())] = schema_field.m_uOffset;

                        std::string field_name_str = field_name;

                        //std::cout << class_name << "->" << field_name << std::endl;
                        // turns out my great solution wasn't so great after all T_T
                        std::replace(field_name_str.begin(), field_name_str.end(), '-', '_');

                        if (field_name_str.empty() || field_name_str[0] == '_')
                            continue;
                    }
                }
            }
        }
    }

    if (map_offsets.empty())
    {
        MessageBoxA(NULL, "offset capture failed - wait for cheat update", "error", MB_OK | MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
        return false;
    }
    
	log_dbg("captured %zu schema offsets", map_offsets.size());

    delete[] scope_array;

    return map_offsets.size() > 0;
}


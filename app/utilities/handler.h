#pragma once
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

class handler
{
public:
	static long __stdcall on_exception( struct _EXCEPTION_POINTERS* );
};

inline std::string format( const char* msg, ... )
{
	char fmt[ 1024 ] = { 0 };
	va_list va;
	va_start( va, msg );
	vsnprintf( fmt, 1024, msg, va );
	va_end( va );
	return std::string( fmt );
}

inline long __stdcall handler::on_exception( _EXCEPTION_POINTERS* ExceptionInfo )
{
	SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME );
	SymInitialize( GetCurrentProcess( ), NULL, TRUE );

	const auto code = ExceptionInfo->ExceptionRecord->ExceptionCode;
	const auto address = ExceptionInfo->ExceptionRecord->ExceptionAddress;

	const char* s_error = "UNKNOWN";
	switch (code)
	{
	case STATUS_ACCESS_VIOLATION: s_error = "ACCESS VIOLATION"; break;
	case STATUS_STACK_OVERFLOW:   s_error = "STACK OVERFLOW"; break;
	case STATUS_HEAP_CORRUPTION:  s_error = "HEAP CORRUPTION"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: s_error = "DIVIDE BY ZERO"; break;
	}

	std::stringstream ss;
	ss << std::format( "Error: {} (0x{:X})\n", s_error, code );
	ss << std::format( "Address: {:#x}\n\n", reinterpret_cast< uintptr_t >( address ) );

	ss << std::format( "RAX: {:#x}\n", ExceptionInfo->ContextRecord->Rax );
	ss << std::format( "RBX: {:#x}\n", ExceptionInfo->ContextRecord->Rbx );
	ss << std::format( "RCX: {:#x}\n", ExceptionInfo->ContextRecord->Rcx );
	ss << std::format( "RDX: {:#x}\n", ExceptionInfo->ContextRecord->Rdx );
	ss << std::format( "RSI: {:#x}\n", ExceptionInfo->ContextRecord->Rsi );
	ss << std::format( "RDI: {:#x}\n", ExceptionInfo->ContextRecord->Rdi );
	ss << std::format( "RBP: {:#x}\n", ExceptionInfo->ContextRecord->Rbp );
	ss << std::format( "RSP: {:#x}\n\n", ExceptionInfo->ContextRecord->Rsp );

	HANDLE process = GetCurrentProcess( );
	IMAGEHLP_LINE64 line {};
	line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );
	DWORD displacement = 0;

	if (SymGetLineFromAddr64( process, ( DWORD64 ) address, &displacement, &line ))
	{
		const char* file = line.FileName ? line.FileName : "<unknown>";
		const char* name = strrchr( file, '\\' );
		if (!name) name = strrchr( file, '/' );
		if (name) file = name + 1;
		ss << std::format( "File: {} (line {})\n\n", file, line.LineNumber );
	}
	else
	{
		ss << std::format( "Crash signature: 0x{:X}\n\n",
			code ^ reinterpret_cast< uintptr_t >( address ) );
	}

	ss << "Built @ " __DATE__ ", " __TIME__ "\n";
	ss << "Ctrl + C to copy this message.\n";

	MessageBoxA(
		NULL,
		ss.str( ).c_str( ),
		"Unhandled Exception",
		MB_SYSTEMMODAL | MB_ICONERROR | MB_OK
	);

	DWORD o_prot {};
	char* p_base = ( char* ) GetModuleHandleA( NULL );
	VirtualProtect( p_base, 4096, PAGE_READWRITE, &o_prot );
	RtlSecureZeroMemory( p_base, 4096 );

	_exit( 123 );
	return EXCEPTION_EXECUTE_HANDLER;
}

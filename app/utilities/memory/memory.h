#pragma once
#include "mem_def.h"
#include "fnv1a.h"
#include "xor.h"

#ifndef MEMORY_USERMODE
#include <WinIoCtl.h>
#endif

#define MEM_PAD(SIZE) \
private: \
	char _MEM_CONCATENATE(pad_0, __COUNTER__)[SIZE]; \
public:

#define _MEM_INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define _MEM_CONCATENATE(LEFT, RIGHT) _MEM_INTERNAL_CONCATENATE(LEFT, RIGHT)

#ifdef __has_builtin
#define CS_HAS_BUILTIN(BUILTIN) __has_builtin(BUILTIN)
#else
#define CS_HAS_BUILTIN(BUILTIN) 0
#endif

#if defined(_CRT_USE_BUILTIN_OFFSETOF) || CS_HAS_BUILTIN(__builtin_offsetof)
#define CS_OFFSETOF(STRUCT, MEMBER) __builtin_offsetof(STRUCT, MEMBER)
#else
#define CS_OFFSETOF(STRUCT, MEMBER) reinterpret_cast<std::size_t>(std::addressof(static_cast<STRUCT*>(nullptr)->MEMBER))
#endif

#define BYTE_InRange(x, a, b)	(x >= a && x <= b)
#define BYTE_GetBits(x)			(BYTE_InRange((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (BYTE_InRange(x,'0','9') ? x - '0' : 0))
#define BYTE_Get(x)				(BYTE_GetBits(x[0]) << 4 | BYTE_GetBits(x[1]))

namespace memory_constants
{
	constexpr uint64_t MIN_VALID_ADDRESS = 0x10000;
	constexpr uint64_t MAX_VALID_ADDRESS = 0x7FFFFFFFFFFF;

#ifndef MEMORY_USERMODE
	constexpr ULONG attach =
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

	constexpr ULONG read =
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

	constexpr ULONG get_module_code =
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

	constexpr ULONG get_pid =
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

	constexpr ULONG batch_read =
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
#endif
}

struct module_info_t
{
	module_info_t() = default;
	module_info_t(std::uintptr_t uBaseAddress, std::string strModulePath, std::uintptr_t uModuleSize)
	{
		m_uBaseAddress = uBaseAddress;
		m_strPath = strModulePath;
		m_uModuleSize = uModuleSize;
	};

	std::uintptr_t m_uBaseAddress = 0;
	std::string m_strPath{};

	std::uintptr_t m_uModuleSize = 0U;
};

enum EPatternScanFlags : std::uint16_t
{
	NO_FLAGS = 0U,
	SCAN_RESOLVE_RIP = (1 << 0),
	SCAN_RESOLVE_ABS = (1 << 1)
};

class c_read_batch;

// todo: rewrite with indirect syscalls
// no..
// (required inline assembly)
class c_memory
{
public:
#ifndef MEMORY_USERMODE
	typedef struct Request_t {
		HANDLE process_id;
		PVOID target;
		PVOID buffer;
		SIZE_T size;
	};

	typedef struct BatchReadRequest_t {
		DWORD64 address;
		SIZE_T size;
		SIZE_T offset_in_buffer;
	};

	typedef struct BatchReadHeader_t {
		HANDLE process_id;
		UINT32 num_requests;
		SIZE_T total_buffer_size;
	};

	typedef struct ModuleRequest_t {
		UINT32 pid;
		UINT64 baseAddress;
		SIZE_T size;
		WCHAR moduleName[1024];
	};

	typedef struct AttachRequest_t {
		HANDLE process_id;
		PVOID target;
		PVOID buffer;
		SIZE_T size;
	};

	typedef struct PIDRequest_t {
		UINT32 pid;
		WCHAR name[1024];
	};
#endif

	c_memory() = default;

	~c_memory()
	{
#ifndef MEMORY_USERMODE
		if (m_pDeviceHandle && m_pDeviceHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m_pDeviceHandle);
#endif
		if (m_pProcessHandle)
			CloseHandle(m_pProcessHandle);
	}

	bool is_window_in_fg(const char* szWindowName)
	{
		static HWND hWnd = FindWindowA(nullptr, szWindowName);
		if (!hWnd)
			return false;

		HWND hForegroundWnd = GetForegroundWindow();
		if (!hForegroundWnd)
			return false;

		return (hWnd == hForegroundWnd);
	}

	bool is_game_running()
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_pProcessID);
		if (hProcess != NULL) {
			CloseHandle(hProcess);
			return true;
		}

		return false;
	}

	bool init(const char* szProcessName)
	{
#ifdef MEMORY_USERMODE
		while (!m_pProcessID)
		{
			PROCESSENTRY32 entry = {};
			entry.dwSize = sizeof(PROCESSENTRY32);

			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			while (Process32Next(hSnapShot, &entry))
			{
				if (!strcmp(szProcessName, entry.szExeFile))
				{
					m_pProcessID = entry.th32ProcessID;
					m_pProcessHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, m_pProcessID);
					break;
				}
			}
			if (hSnapShot)
				CloseHandle(hSnapShot);
		}
		return m_pProcessID != 0;
#else
		m_pDeviceHandle = CreateFileW(L"\\\\.\\Spotify",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr, OPEN_EXISTING, 0, nullptr);

		if (m_pDeviceHandle == nullptr || m_pDeviceHandle == INVALID_HANDLE_VALUE)
			return false;

		m_pProcessID = find_process(szProcessName);
		if (!m_pProcessID)
			return false;

		if (!attach())
			return false;

		m_pProcessHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, m_pProcessID);

		return true;
#endif
	}

#ifndef MEMORY_USERMODE
	bool attach()
	{
		Request_t request = { (HANDLE)(uintptr_t)m_pProcessID };
		DWORD bytesReturned = 0;
		return DeviceIoControl(m_pDeviceHandle, memory_constants::attach,
			&request, sizeof(request),
			&request, sizeof(request), &bytesReturned, nullptr);
	}

	uint32_t find_process(const char* szProcessName)
	{
		PIDRequest_t request = {};
		memset(request.name, 0, sizeof(request.name));
		MultiByteToWideChar(CP_UTF8, 0, szProcessName, -1, request.name, sizeof(request.name) / sizeof(WCHAR));

		DWORD bytesReturned = 0;
		bool success = DeviceIoControl(m_pDeviceHandle, memory_constants::get_pid,
			&request, sizeof(request),
			&request, sizeof(request), &bytesReturned, nullptr);

		return (success && request.pid != 0) ? request.pid : 0;
	}
#endif

	bool read_raw(const std::uintptr_t& uAddress, void* pBuffer, std::size_t uSize)
	{
#ifdef MEMORY_USERMODE
		static tNtReadVirtualMemory NtReadVirtualMemory = reinterpret_cast<tNtReadVirtualMemory>(get_import("ntdll.dll", X("NtReadVirtualMemory")));
		return NtReadVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), pBuffer, static_cast<ULONG>(uSize), 0) == 0;
#else
		if (!isValid(uAddress))
			return false;

		if (uSize > 0x1000)
			return read_raw_vm(uAddress, pBuffer, uSize);

		Request_t request = {
			(HANDLE)(uintptr_t)m_pProcessID,
			(PVOID)uAddress,
			pBuffer,
			uSize
		};

		DWORD bytesReturned = 0;
		return DeviceIoControl(m_pDeviceHandle, memory_constants::read,
			&request, sizeof(request),
			&request, sizeof(request), &bytesReturned, nullptr);
#endif
	}

	bool read_raw(void* pAddress, void* pBuffer, std::size_t uSize)
	{
#ifdef MEMORY_USERMODE
		static tNtReadVirtualMemory NtReadVirtualMemory = reinterpret_cast<tNtReadVirtualMemory>(get_import("ntdll.dll", X("NtReadVirtualMemory")));
		return NtReadVirtualMemory(m_pProcessHandle, pAddress, pBuffer, static_cast<ULONG>(uSize), 0) == 0;
#else
		return read_raw(reinterpret_cast<std::uintptr_t>(pAddress), pBuffer, uSize);
#endif
	}

	static bool isValid(const uint64_t address)
	{
		return address >= memory_constants::MIN_VALID_ADDRESS &&
			address < memory_constants::MAX_VALID_ADDRESS &&
			(address + sizeof(uintptr_t) > address);
	}

	template<typename T = std::uintptr_t>
	T read(const std::uintptr_t& uAddress)
	{
		T buffer{};
		read_raw(uAddress, &buffer, sizeof(T));
		return buffer;
	}

	template<typename T = std::uintptr_t>
	T read(const std::uintptr_t& uBaseAddress, std::vector<std::uintptr_t> vecOffsets)
	{
		std::uintptr_t uAddress = read<std::uintptr_t>(uBaseAddress);
		if (vecOffsets.empty())
			return uAddress;

		for (std::uintptr_t uOffset : vecOffsets)
			uAddress = read<std::uintptr_t>(uAddress + uOffset);

		return uAddress;
	}

	std::string read_string(const std::uintptr_t& uAddress)
	{
		char buffer[MAX_PATH]{};
		read_raw(uAddress, &buffer, sizeof(buffer));
		return std::string(buffer);
	}

	template<typename T = std::uintptr_t>
	void write(const std::uintptr_t& uAddress, T value)
	{
		static tNtWriteVirtualMemory NtWriteVirtualMemory = reinterpret_cast<tNtWriteVirtualMemory>(get_import("ntdll.dll", X("NtWriteVirtualMemory")));
		NtWriteVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), &value, sizeof(T), nullptr);
	}

	std::uintptr_t get_import(const char* szModule, const char* szImport)
	{
		FNV1A_t uHash = FNV1A::Hash(szImport);
		if (m_mapImports.find(uHash) == m_mapImports.end())
		{
			HMODULE hModule = GetModuleHandleA(szModule);
			if (hModule != nullptr)
			{
				m_mapImports[uHash] = reinterpret_cast<std::uintptr_t>(GetProcAddress(hModule, szImport));
			}
		}
		return m_mapImports[uHash];
	}

	module_info_t get_module(const char* szModuleName)
	{
		FNV1A_t uHash = FNV1A::Hash(szModuleName);
		if (m_mapModules.find(uHash) == m_mapModules.end())
		{
			MODULEENTRY32 entry = {};
			entry.dwSize = sizeof(MODULEENTRY32);
			const HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pProcessID);

			while (Module32Next(hSnapShot, &entry))
			{
				if (!strcmp(szModuleName, entry.szModule))
				{
					if (hSnapShot)
						CloseHandle(hSnapShot);

					m_mapModules[uHash] = { reinterpret_cast<std::uintptr_t>(entry.modBaseAddr), std::string(entry.szExePath), static_cast<std::uintptr_t>(entry.modBaseSize) };
					return m_mapModules[uHash];
				}
			}

			if (hSnapShot)
				CloseHandle(hSnapShot);
		}

		return m_mapModules[uHash];
	}

	std::uintptr_t pattern_scan(const char* szModuleName, const char* szSignature, uint16_t uFlags = NO_FLAGS, std::uint32_t uOption1 = 0U, std::uint32_t uOption2 = 0U)
	{
		module_info_t moduleInformation = get_module(szModuleName);

		uint8_t* pModuleBytes = new uint8_t[moduleInformation.m_uModuleSize];
		if (!read_raw(moduleInformation.m_uBaseAddress, pModuleBytes, moduleInformation.m_uModuleSize))
		{
			log_dbg("failed to read module bytes");
			return 0U;
		}

		std::uintptr_t uAddress = find_signature(reinterpret_cast<std::uintptr_t>(pModuleBytes), moduleInformation.m_uBaseAddress, szSignature);
		if (uAddress == 0U)
		{
			std::cout << ("Failed to find signature in ") << szModuleName << std::endl;
			return false;
		}

		if (uFlags & SCAN_RESOLVE_RIP)
			uAddress = resolve_relative(uAddress, uOption1, uOption2);

		if (uFlags & SCAN_RESOLVE_ABS)
			uAddress = get_abs_address(uAddress, uOption1, uOption2);

		uAddress -= reinterpret_cast<std::uintptr_t>(pModuleBytes);
		std::uintptr_t uFinalAddress = moduleInformation.m_uBaseAddress + uAddress;

		delete[] pModuleBytes;
		return uFinalAddress;
	}

	bool flush_batch(const c_read_batch& batch) { return flush_batch_impl(batch); }

	// :troll:

	void clear_modules( )
	{
		if ( !m_mapModules.empty( ) )
			m_mapModules.clear( );
	}

private:
	bool sig_is_match(std::uint8_t* pAddress, std::uint8_t* pPattern, std::uint8_t* pMask)
	{
		std::size_t uIndex = 0U;
		while (pAddress[uIndex] == pPattern[uIndex] || pMask[uIndex] == (uint8_t)('?'))
		{
			if (!pMask[++uIndex])
				return true;
		}

		return false;
	}

#ifndef MEMORY_USERMODE
	bool read_raw_vm(const std::uintptr_t& uAddress, void* pBuffer, std::size_t uSize)
	{
		constexpr std::size_t CHUNK = 0x10000;

		const uint32_t num_requests = (uint32_t)((uSize + CHUNK - 1) / CHUNK);
		const std::size_t header_and_reqs = sizeof(BatchReadHeader_t) + num_requests * sizeof(BatchReadRequest_t);
		const std::size_t total_size = header_and_reqs + uSize;

		std::vector<uint8_t> buf(total_size, 0);

		auto* header = reinterpret_cast<BatchReadHeader_t*>(buf.data());
		header->process_id = (HANDLE)(uintptr_t)m_pProcessID;
		header->num_requests = num_requests;
		header->total_buffer_size = uSize;

		auto* requests = reinterpret_cast<BatchReadRequest_t*>(header + 1);
		for (uint32_t i = 0; i < num_requests; ++i)
		{
			std::size_t offset = (std::size_t)i * CHUNK;
			requests[i].address = uAddress + offset;
			requests[i].size    = min(CHUNK, uSize - offset);
			requests[i].offset_in_buffer = offset;
		}

		DWORD bytesReturned = 0;
		if (!DeviceIoControl(m_pDeviceHandle, memory_constants::batch_read,
			buf.data(), (DWORD)total_size,
			buf.data(), (DWORD)total_size,
			&bytesReturned, nullptr))
			return false;

		memcpy(pBuffer, buf.data() + header_and_reqs, uSize);
		return true;
	}
#endif

	std::uintptr_t find_signature(uintptr_t uAddress, uintptr_t uSize, const char* szSignature)
	{
		std::size_t uSignatureLength = strlen(szSignature);

		std::uint8_t* pPatternAlloc = new std::uint8_t[uSignatureLength >> 1];
		std::uint8_t* pMaskAlloc = new std::uint8_t[uSignatureLength >> 1];
		std::uint8_t* pPattern = pPatternAlloc;
		std::uint8_t* pMask = pMaskAlloc;

		std::size_t uPatternLength = 0;
		while (*szSignature)
		{
			if (*szSignature == ' ') szSignature++;
			if (!*szSignature) break;

			if (*(std::uint8_t*)(szSignature) == (std::uint8_t)('\?'))
			{
				*pPattern++ = 0;
				*pMask++ = '?';
				szSignature += ((*(std::uint16_t*)(szSignature) == (std::uint16_t)('\?\?')) ? 2 : 1);
			}
			else
			{
				*pPattern++ = BYTE_Get(szSignature);
				*pMask++ = 'x';
				szSignature += 2;
			}

			++uPatternLength;
		}

		*pMask = 0;

		std::uintptr_t uReturnValue = 0;
		for (std::uintptr_t i = 0; uSize > i; ++i)
		{
			if (sig_is_match(reinterpret_cast<std::uint8_t*>(uAddress + i), pPatternAlloc, pMaskAlloc))
			{
				uReturnValue = (uAddress + i);
				break;
			}
		}

		delete[] pPatternAlloc;
		delete[] pMaskAlloc;
		return uReturnValue;
	}

	std::uintptr_t get_abs_address(std::uintptr_t uRelativeAddr, std::uint32_t uPreOffset = 0U, std::uint32_t uPostOffset = 0U)
	{
		uRelativeAddr += uPreOffset;
		uRelativeAddr += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(uRelativeAddr);
		uRelativeAddr += uPostOffset;
		return uRelativeAddr;
	}

	std::uintptr_t resolve_relative(std::uintptr_t uAddressBytes, std::uint32_t uRVAOffset, std::uint32_t uRIPOffset, std::uint32_t uAdditionalOffset = 0U)
	{
		const std::uintptr_t uRVA = *reinterpret_cast<PLONG>(uAddressBytes + uRVAOffset);
		const std::uintptr_t uRIP = uAddressBytes + uRIPOffset;

		if (uAdditionalOffset)
			return read<std::uintptr_t>(uRVA + uRIP) + uAdditionalOffset;

		return uRVA + uRIP;
	}

private:
#ifndef MEMORY_USERMODE
	HANDLE m_pDeviceHandle = nullptr;
#endif
	HANDLE m_pProcessHandle = nullptr;
	DWORD m_pProcessID = 0;

	bool flush_batch_impl(const c_read_batch& batch);

	std::unordered_map<FNV1A_t, std::uintptr_t> m_mapImports;
	std::unordered_map<FNV1A_t, module_info_t> m_mapModules;
};

// -- batch read helper --
// queue multiple reads then flush them all in a single batch_read IOCTL call.
// use for hot paths where many independent addresses are read per frame.
//
// example:
//   c_read_batch batch;
//   batch.queue<vector3d>(view_angles_addr, &angles);
//   batch.queue<matrix4x4>(view_matrix_addr, &matrix);
//   memory->flush_batch(batch);
class c_read_batch
{
public:
	template<typename T>
	c_read_batch& queue(uintptr_t address, T* out)
	{
		m_entries.push_back({ address, reinterpret_cast<void*>(out), sizeof(T) });
		return *this;
	}

	void clear() { m_entries.clear(); }
	bool empty() const { return m_entries.empty(); }

	struct entry_t
	{
		uintptr_t address;
		void* out;
		size_t size;
	};

	const std::vector<entry_t>& entries() const { return m_entries; }

private:
	std::vector<entry_t> m_entries;
};

inline bool c_memory::flush_batch_impl(const c_read_batch& batch)
{
	const auto& entries = batch.entries();
	if (entries.empty())
		return true;

#ifdef MEMORY_USERMODE
	for (const auto& e : entries)
		read_raw(e.address, e.out, e.size);
	return true;
#else
	// compute per-entry offsets into the output buffer
	std::vector<size_t> offsets(entries.size());
	size_t total_output = 0;
	for (size_t i = 0; i < entries.size(); ++i)
	{
		offsets[i] = total_output;
		total_output += entries[i].size;
	}

	const size_t header_and_reqs = sizeof(BatchReadHeader_t) + entries.size() * sizeof(BatchReadRequest_t);
	const size_t total_size = header_and_reqs + total_output;

	std::vector<uint8_t> buf(total_size, 0);

	auto* header = reinterpret_cast<BatchReadHeader_t*>(buf.data());
	header->process_id     = (HANDLE)(uintptr_t)m_pProcessID;
	header->num_requests   = (UINT32)entries.size();
	header->total_buffer_size = total_output;

	auto* reqs = reinterpret_cast<BatchReadRequest_t*>(header + 1);
	for (size_t i = 0; i < entries.size(); ++i)
	{
		reqs[i].address          = entries[i].address;
		reqs[i].size             = entries[i].size;
		reqs[i].offset_in_buffer = offsets[i];
	}

	DWORD bytesReturned = 0;
	if (!DeviceIoControl(m_pDeviceHandle, memory_constants::batch_read,
		buf.data(), (DWORD)total_size,
		buf.data(), (DWORD)total_size,
		&bytesReturned, nullptr))
		return false;

	const uint8_t* output = buf.data() + header_and_reqs;
	for (size_t i = 0; i < entries.size(); ++i)
		memcpy(entries[i].out, output + offsets[i], entries[i].size);

	return true;
#endif
}

inline auto memory = std::make_shared<c_memory>( );

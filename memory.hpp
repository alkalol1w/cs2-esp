#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>

class Memory {
public:
    HANDLE hProcess = nullptr;
    uintptr_t clientBase = 0;
    uintptr_t engineBase = 0;

    Memory() = default;
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    bool Attach(const std::wstring& processName) {
        DWORD pid = GetPID(processName);
        if (!pid) return false;

        hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
            hProcess = nullptr;
            return false;
        }

        clientBase = GetModuleBase(pid, L"client.dll");
        engineBase = GetModuleBase(pid, L"engine2.dll");
        return clientBase != 0;
    }

    bool IsAttached() const { return hProcess != nullptr; }

    template<typename T>
    T Read(uintptr_t address) const {
        T val{};
        if (!hProcess || !address) return val;
        SIZE_T read = 0;
        ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address),
                          &val, sizeof(T), &read);
        return val;
    }

    std::string ReadString(uintptr_t address, size_t maxLen = 128) const {
        if (!hProcess || !address) return {};
        std::string buf(maxLen, '\0');
        SIZE_T read = 0;
        if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address),
                               buf.data(), maxLen, &read) || read == 0) {
            return {};
        }
        size_t nul = buf.find('\0');
        if (nul != std::string::npos) buf.resize(nul);
        else buf.resize(read);
        return buf;
    }

    ~Memory() {
        if (hProcess) CloseHandle(hProcess);
    }

private:
    DWORD GetPID(const std::wstring& name) {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32W pe{};
        pe.dwSize = sizeof(pe);

        DWORD pid = 0;
        if (Process32FirstW(snap, &pe)) {
            do {
                if (name == pe.szExeFile) { pid = pe.th32ProcessID; break; }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
        return pid;
    }

    uintptr_t GetModuleBase(DWORD pid, const std::wstring& modName) {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32W me{};
        me.dwSize = sizeof(me);

        uintptr_t base = 0;
        if (Module32FirstW(snap, &me)) {
            do {
                if (modName == me.szModule) {
                    base = reinterpret_cast<uintptr_t>(me.modBaseAddr);
                    break;
                }
            } while (Module32NextW(snap, &me));
        }
        CloseHandle(snap);
        return base;
    }
};
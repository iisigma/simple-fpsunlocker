#include <windows.h>
#include <iostream>
#include <thread>
#include <string>

static const auto current_module = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(NULL));
template<typename T>
T rebase(const std::uintptr_t& address) {
	return reinterpret_cast<T>(current_module + address);
}

bool is_numeric(const std::string& str) {
	for (char c : str)
		if (c > '9' || c < '0')
			return false;

	return true;
}

void init() {
	DWORD old{};
	FILE* stream{};

	VirtualProtect(reinterpret_cast<void*>(&FreeConsole), 1, PAGE_EXECUTE_READWRITE, &old);
	*reinterpret_cast<std::uint8_t*>(&FreeConsole) = 0xC3;
	VirtualProtect(reinterpret_cast<void*>(&FreeConsole), 1, old, &old);

	AllocConsole();
	SetConsoleTitleA("simple fps unlocker");
	freopen_s(&stream, "conin$", "r", stdin);
	freopen_s(&stream, "conout$", "w", stdout);

	static const auto getscheduler = rebase<std::uintptr_t(__cdecl*)()>(0xDCD950);
	std::cout << getscheduler() << std::endl;

	for (;;) {
		reentry:
		std::string val{};
		std::getline(std::cin, val);

		if (!is_numeric(val))
			goto reentry;

		*reinterpret_cast<double*>(getscheduler() + 0x118) = 1 / std::stod(val);
		std::cout << std::stod(val) << std::endl;
	}
}

std::uint8_t WINAPI DllMain(int, int type, int) {
	if (type == DLL_PROCESS_ATTACH)
		std::thread{ init }.detach();
	return TRUE;
}

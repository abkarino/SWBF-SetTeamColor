// SetTeamColor.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "SetTeamColor.h"

#pragma comment(lib, "BasePlugin")

struct Team {
	BYTE gap0[0x6C];
	DWORD colors[8];
};

struct SpawnManager {
	BYTE gap0[0x18];
	Team* teams[8];
};

typedef float (*luaL__check_number_TYPE)(LPVOID, int);
typedef BOOL (*luaL__isnumber_TYPE)(LPVOID, int);
typedef SpawnManager* (*PSpawnManager)[2];

PSpawnManager pgSpawnManager = 0;
LPVOID continueAddress = 0;
LPVOID skipAddress = 0;
LPVOID gSpawnManager__get = 0;

_declspec(naked) void zeroColor() {
	__asm {
		xor     eax, eax;
		mov     [ecx], eax;
		ret     ;
	}
}

_declspec(naked) void zeroColor_gog() {
	__asm {
		xor     esi, esi;
		mov     [eax+0x6C], esi;
		mov     [eax+0x70], esi;
		mov     [eax+0x74], esi;
		mov     [eax+0x78], esi;
		mov     [eax+0x7C], esi;
		mov     [eax+0x80], esi;
		mov     [eax+0x84], esi;
		mov     [eax+0x88], esi;
		pop     esi;
		pop     ebp;
		ret     0x10;
	}
}

_declspec(naked) void trampoline_sptest() {
	__asm {
		push    esi;
		mov     esi, [eax];
		test    esi, esi;
		pop     esi;
		je      og;
		jmp     skipAddress;
		og:
		lea     ecx, [edx + eax];
		mov     ecx, [ecx + ebp];
		jmp     continueAddress;
	}
}

_declspec(naked) void trampoline_steam() {
	__asm {
		push    esi;
		mov     esi, [edi];
		test    esi, esi;
		pop     esi;
		je      og;
		jmp     skipAddress;
		og:
		push    ebx;
		call    gSpawnManager__get;
		jmp     continueAddress;
	}
}

_declspec(naked) void trampoline_gog() {
	__asm {
		push    esi;
		mov     esi, [edx];
		test    esi, esi;
		pop     esi;
		je      og;
		jmp     skipAddress;
		og:
		lea     ecx, [ebx + edx];
		mov     eax, [ecx + eax];
		jmp     continueAddress;
	}
}

bool SetTeamColor_handler(LPVOID luaState) {
	luaL__check_number_TYPE _luaL__check_number = reinterpret_cast<luaL__check_number_TYPE>(BasePlugin::luaL__check_number);
	luaL__isnumber_TYPE _luaL__isnumber = reinterpret_cast<luaL__isnumber_TYPE>(BasePlugin::luaL__isnumber);
	int teamIndex = _luaL__check_number(luaState, 1);
	int teamColorIndex = _luaL__check_number(luaState, 2);

	Team* team = (*pgSpawnManager)[0]->teams[teamIndex];
	BYTE* color = (BYTE*)&team->colors[teamColorIndex];

	color[0] = (int)_luaL__check_number(luaState, 5);
	color[1] = (int)_luaL__check_number(luaState, 4);
	color[2] = (int)_luaL__check_number(luaState, 3);
	color[3] = 255;
	return 0;
}

BasePlugin::LuaCFunction fns[2] = {
	{"SetTeamColor", &SetTeamColor_handler},
	{0, 0},
};

bool SetTeamColor::init() {
	while (!BasePlugin::Ready)
		Sleep(5);
	BasePlugin::GameVersion version = BasePlugin::DetectGameVersion();
	BYTE* base = (BYTE*)GetModuleHandle(NULL);
	BYTE* target = NULL;
	if (version == BasePlugin::SPTEST) {
		pgSpawnManager = reinterpret_cast<PSpawnManager>(base + 0x29a610);
		continueAddress = reinterpret_cast<LPVOID>(base + 0xbebc4);
		skipAddress = reinterpret_cast<LPVOID>(base + 0xbebf5);

		//trampoline
		target = base + 0xbebbe;
		DWORD  oldProtect;
		VirtualProtect(target, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
		target[0] = 0xE9;
		*(DWORD*)(target + 1) = reinterpret_cast<BYTE*>(&trampoline_sptest) - target - 5;
		target[5] = 0x90;
		VirtualProtect(target, 6, oldProtect, &oldProtect);
		//set color to zero initially to know if we set it in lua or not
		target = base + 0xbf31b;
		VirtualProtect(target, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(DWORD*)target = reinterpret_cast<DWORD>(&zeroColor);
		VirtualProtect(target, 4, oldProtect, &oldProtect);
	}
	else if (version == BasePlugin::STEAM) {
		pgSpawnManager = reinterpret_cast<PSpawnManager>(base + 0x62ea50);
		continueAddress = reinterpret_cast<LPVOID>(base + 0x224046);
		skipAddress = reinterpret_cast<LPVOID>(base + 0x224082);
		gSpawnManager__get = reinterpret_cast<LPVOID>(base + 0x1979a);

		//trampoline
		target = base + 0x224040;
		DWORD  oldProtect;
		VirtualProtect(target, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
		target[0] = 0xE9;
		*(DWORD*)(target + 1) = reinterpret_cast<BYTE*>(&trampoline_steam) - target - 5;
		target[5] = 0x90;
		VirtualProtect(target, 6, oldProtect, &oldProtect);
		//set color to zero initially to know if we set it in lua or not
		target = base + 0x2234E4 + 1;
		VirtualProtect(target, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(DWORD*)target = reinterpret_cast<DWORD>(&zeroColor);
		VirtualProtect(target, 4, oldProtect, &oldProtect);
	}
	else if (version == BasePlugin::GOG) {
		pgSpawnManager = reinterpret_cast<PSpawnManager>(base + 0x5deecc);
		continueAddress = reinterpret_cast<LPVOID>(base + 0x20fdf0);
		skipAddress = reinterpret_cast<LPVOID>(base + 0x20fe1f);

		//trampoline
		target = base + 0x20fdea;
		DWORD  oldProtect;
		VirtualProtect(target, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
		target[0] = 0xE9;
		*(DWORD*)(target + 1) = reinterpret_cast<BYTE*>(&trampoline_gog) - target - 5;
		target[5] = 0x90;
		VirtualProtect(target, 6, oldProtect, &oldProtect);
		//trampoline for zero
		target = base + 0x20f3cd;
		VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		target[0] = 0xE9;
		*(DWORD*)(target + 1) = reinterpret_cast<BYTE*>(&zeroColor_gog) - target - 5;
		VirtualProtect(target, 5, oldProtect, &oldProtect);
	}
	else return false;

	BasePlugin::RegisterLUAFunctions(fns);

	return true;
}

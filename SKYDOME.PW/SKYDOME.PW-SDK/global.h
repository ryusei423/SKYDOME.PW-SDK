#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <sstream>
#include <iostream>
#include "external/SD.h"
#include "external/easylogging/easylogging++.h"

//标准库对于char8_t缺乏支持 我认为它存在的意义并不大
#define U8ST(s) ((const char*)u8 ## s)

//目前真正的作弊并不是第一目标
#define XorStr(s) s

#define SD_DEBUG_BREAK __debugbreak()
#define SD_ASSERT(EXPRESSION) static_cast<void>(!!(EXPRESSION) || (SD_DEBUG_BREAK, 0))

#define CLIENT_DLL XorStr(L"client.dll")
#define ENGINE2_DLL XorStr(L"engine2.dll")
#define SCHEMASYSTEM_DLL XorStr(L"schemasystem.dll")
#define INPUTSYSTEM_DLL XorStr(L"inputsystem.dll")
#define SDL3_DLL XorStr(L"SDL3.dll")
#define TIER0_DLL XorStr(L"tier0.dll")
#define NAVSYSTEM_DLL XorStr(L"navsystem.dll")
#define RENDERSYSTEM_DLL XorStr(L"rendersystemdx11.dll")
#define LOCALIZE_DLL XorStr(L"localize.dll")
#define DBGHELP_DLL XorStr(L"dbghelp.dll")
#define GAMEOVERLAYRENDERER_DLL XorStr(L"GameOverlayRenderer64.dll")
#define PARTICLES_DLL XorStr(L"particles.dll")
#define SCENESYSTEM_DLL XorStr(L"scenesystem.dll")
#define MATERIAL_SYSTEM2_DLL XorStr(L"materialsystem2.dll")
#define MATCHMAKING_DLL XorStr(L"matchmaking.dll")
#define RESOURCESYSTEM_DLL XorStr(L"resourcesystem.dll")

//计数器宏用于内存占位
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define UNIQUE_NAME1(prefix) CONCAT(prefix, __COUNTER__)

#define MEM_PAD(SIZE) \
private: \
    char UNIQUE_NAME1(pad_)[SIZE]; \
public:

#define TICK_INTERVAL			/*(g_interfaces->GlobalVars->flIntervalPerTick)*/1.f / 64.f
#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

//它使用上面的一些东西，在define之后编译
#include "sd/localization/localization.h"
//#include "sd/interfaces/interfaces.h"


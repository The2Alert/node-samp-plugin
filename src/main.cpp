// #define NODESAMP_DEBUG
#include <malloc.h>
#include <memory>
#include "amx/amx.h"
#include "plugincommon.h"
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "logger.h"
#include "config.h"
#include "platform.h"
#include "exception.h"
#include "natives.h"

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

extern void* pAMXFunctions;

std::unique_ptr<nodesamp::Platform> platform;

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData) {
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    sampgdk::Load(ppData);
    try {
        nodesamp::Logger::Init();
        std::unique_ptr<nodesamp::Config> config = nodesamp::GetConfig();
        platform = nodesamp::InitPlatform(config->nodeOptions, config->path, config->nodeArgs);
        nodesamp::Log("Initialized.");
    } catch(const nodesamp::Exception& exception) {
        nodesamp::Logger::Error(exception);
    } catch(const nodesamp::ConfigException& exception) {
        nodesamp::Logger::Config::Error(exception);
    }
    #ifdef NODESAMP_DEBUG
    nodesamp::Logger::Debug("Load()");
    #endif
    return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx) {
    return nodesamp::RegisterNatives(amx);
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX* amx, const char* name, cell* params, cell* retval) {
    if(platform != nullptr)
        platform->callAmxPublic(amx, name, params, retval);
    #ifdef NODESAMP_DEBUG
    nodesamp::Logger::Debug("OnPublicCall(\"" + std::string(name) + "\")");
    #endif
    return true;
} 

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
    sampgdk::ProcessTick();
    if(platform != nullptr)
        platform->tick();
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx) {
    return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    if(platform != nullptr)
        platform->uninit();
    sampgdk::Unload();
    #ifdef NODESAMP_DEBUG
    nodesamp::Logger::Debug("Unload()");
    #endif
}
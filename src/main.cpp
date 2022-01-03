#include <memory>
#include <exception>
#include "amx/amx.h"
#include "plugincommon.h"
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "config.h"
#include "platform.h"
#include "logger.h"
#ifdef _WIN32
#include <direct.h>
#define cd _chdir
#endif

std::unique_ptr<nodesamp::Config> config;
std::unique_ptr<nodesamp::Platform> platform;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

extern void* pAMXFunctions;

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	sampgdk::Load(ppData);
	try {
		config = nodesamp::GetConfig();
		platform = nodesamp::CreatePlatform(config->packagePath, config->nodeOptions);
		platform->init();
		nodesamp::log("Initialized.");
	} catch(const std::exception& error) {
		nodesamp::error(error.what());
	}
	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx) {
	if(platform != nullptr) {
		cd(config->packagePath.c_str());
		platform->initModule();
	}
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX* amx, const char* name, cell* params, cell* retval) {
	if(platform != nullptr)
		platform->getModule()->getAmx()->callPublic(amx, name, params, retval);
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	sampgdk::ProcessTick();
	if(platform != nullptr)
		platform->tick();
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx) {
	if(platform != nullptr)
		platform->uninitModule();
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	if(platform != nullptr) {
		platform->uninit();
		platform.reset();
	}
	sampgdk::Unload();
	if(config != nullptr)
		config.reset();
}
#include "natives.h"
#include <memory>
#include "platform.h"
#include "env.h"

extern std::unique_ptr<nodesamp::Platform> platform;

namespace nodesamp {
    int RegisterNatives(AMX* amx) {
        static const AMX_NATIVE_INFO natives[] = {
            {"nodesamp_LoadDefaultEnvironment", Natives::LoadDefaultEnvironment},
            {0, 0}
        };
        return amx_Register(amx, natives, -1);
    }

    namespace Natives {
        cell LoadDefaultEnvironment(AMX* amx, cell* params) {
            if(platform == nullptr)
                return 0;
            platform->loadDefaultEnv();
            return 1;
        }
    }
}
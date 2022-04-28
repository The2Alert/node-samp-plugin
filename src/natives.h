#ifndef NODESAMP_NATIVES_H
#define NODESAMP_NATIVES_H

#include <malloc.h>
#include "amx/amx.h"

namespace nodesamp {
    int RegisterNatives(AMX* amx);

    namespace Natives {
        cell LoadDefaultEnvironment(AMX* amx, cell* params);
    }
}

#endif
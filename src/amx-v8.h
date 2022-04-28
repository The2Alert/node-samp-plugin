#ifndef NODESAMP_AMX_V8_H
#define NODESAMP_AMX_V8_H

#include <vector>
#include "v8.h"
#include <string>
#include <malloc.h>
#include "amx/amx.h"

namespace nodesamp {
    std::vector<v8::Local<v8::Value>> ToV8Args(AMX* amx, v8::Isolate* isolate, std::string format, cell* params);
}

#endif
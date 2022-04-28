#ifndef NODESAMP_API_H
#define NODESAMP_API_H

#include "v8.h"

namespace nodesamp {
    void CreateAPI(v8::Isolate* isolate);

    namespace API {
        void Logprint(const v8::FunctionCallbackInfo<v8::Value>& info);
    }
}

#endif
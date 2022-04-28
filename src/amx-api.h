#ifndef NODESAMP_AMX_API_H
#define NODESAMP_AMX_API_H

#include "v8.h"

namespace nodesamp {
    namespace API {
        namespace AMX {
            v8::Local<v8::Array> CallNative(const v8::FunctionCallbackInfo<v8::Value>& info, bool retFloat);

            void CallNative(const v8::FunctionCallbackInfo<v8::Value>& info);

            void CallNativeInFloat(const v8::FunctionCallbackInfo<v8::Value>& info);

            void OnPublicCall(const v8::FunctionCallbackInfo<v8::Value>& info);
            
            void RemovePublic(const v8::FunctionCallbackInfo<v8::Value>& info);
        }
    }
}

#endif
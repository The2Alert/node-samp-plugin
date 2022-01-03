#ifndef NODESAMP_AMX_H
#define NODESAMP_AMX_H

#include <utility>
#include <string>
#include <vector>
#include "amx/amx.h"
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "v8.h"
#include "node.h"
#include "utils.h"

namespace nodesamp {
    class Amx {
        private:
            static void callNative(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void callNativeInFloat(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void onPublicCall(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void removePublic(const v8::FunctionCallbackInfo<v8::Value>& info);

            node::CommonEnvironmentSetup* setup;
        public:
            Amx(node::CommonEnvironmentSetup* setup);

            void init();
            node::CommonEnvironmentSetup* getSetup();
            void callPublic(AMX* amx, std::string eventName, cell* params, cell* retval);
    };
}

#endif
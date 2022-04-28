#include "api.h"
#include <string>
#include <utility>
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "iconvlite.h"
#include "amx-api.h"

namespace nodesamp {
    void CreateAPI(v8::Isolate* isolate) {
        using namespace v8;
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> samp = Object::New(isolate);
        static const std::pair<std::string, FunctionCallback> sampFuncs[] = {
            {"logprint", API::Logprint}
        };
        for(auto pair: sampFuncs) {
            Local<String> name = String::NewFromUtf8(isolate, pair.first.c_str()).ToLocalChecked();
            Local<Function> func = FunctionTemplate::New(isolate, pair.second)->GetFunction(context).ToLocalChecked();
            func->SetName(name);
            samp->Set(context, name, func).FromMaybe(false);
        }
        static const std::pair<std::string, FunctionCallback> amxFuncs[] = {
            {"callNative", API::AMX::CallNative},
            {"callNativeInFloat", API::AMX::CallNativeInFloat},
            {"onPublicCall", API::AMX::OnPublicCall},
            {"removePublic", API::AMX::RemovePublic}
        };
        Local<Object> amx = Object::New(isolate);
        for(auto pair: amxFuncs) {
            Local<String> name = String::NewFromUtf8(isolate, pair.first.c_str()).ToLocalChecked();
            Local<Function> func = FunctionTemplate::New(isolate, pair.second)->GetFunction(context).ToLocalChecked();
            func->SetName(name);
            amx->Set(context, name, func).FromMaybe(false);
        }
        Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        amx->Set(context, key, Object::New(isolate)).FromMaybe(false);
        key = String::NewFromUtf8(isolate, "amx").ToLocalChecked();
        samp->Set(context, key, amx).FromMaybe(false);
        key = String::NewFromUtf8(isolate, "samp").ToLocalChecked();
        context->Global()->Set(context, key, samp).FromMaybe(false);
    }

    namespace API {
        void Logprint(const v8::FunctionCallbackInfo<v8::Value>& info) {
            using namespace v8;
            Isolate* isolate = info.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();
            std::string log = *String::Utf8Value(isolate, info[0]);
            sampgdk::logprintf("%s", utf2cp(log).c_str());
        }
    }
}
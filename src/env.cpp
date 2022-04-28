#include "env.h"
#include <vector>
#include "platform.h"
#include "v8.h"
#include "uv.h"
#include "api.h"
#include "amx-v8.h"
#include <stdlib.h>
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"

namespace nodesamp {
    Environment::Environment(Platform* platform, std::string source): platform(platform), source(source) {}

    void Environment::load() {
        using namespace v8;
        node::MultiIsolatePlatform* platform = this->platform->platform.get();
        std::vector<std::string> errors;
        std::vector<std::string>& args = this->platform->args;
        std::vector<std::string> execArgs;
        this->setup = node::CommonEnvironmentSetup::Create(platform, &errors, args, execArgs);
        Isolate* isolate = this->setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        CreateAPI(isolate);
        node::Environment* env = this->setup->env();
        node::SetProcessExitHandler(env, [&](node::Environment* env, int code) {
            this->exited = true;
            SendRconCommand("exit");
        });
        if(this->source != "")
            node::LoadEnvironment(env, this->source.c_str());
        else node::LoadEnvironment(env, node::StartExecutionCallback{});
    }

    void Environment::tick() {
        using namespace v8;
        node::CommonEnvironmentSetup* setup = this->setup.get();
        Isolate* isolate = setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        uv_run(setup->event_loop(), UV_RUN_NOWAIT);
    }

    void Environment::callAmxPublic(AMX* amx, std::string name, cell* params, cell* retval) {
        using namespace v8;
        Isolate* isolate = this->setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        Local<String> key = String::NewFromUtf8(isolate, "samp").ToLocalChecked();
        Local<Value> value = context->Global()->Get(context, key).ToLocalChecked();
        if(!value->IsObject())
            return;
        Local<Object> samp = Local<Object>::Cast(value);
        key = String::NewFromUtf8(isolate, "amx").ToLocalChecked();
        value = samp->Get(context, key).ToLocalChecked();
        if(!value->IsObject())
            return;
        Local<Object> amxObject = Local<Object>::Cast(value);
        key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        value = amxObject->Get(context, key).ToLocalChecked();
        if(!value->IsObject())
            return;
        Local<Object> events = Local<Object>::Cast(value);
        key = String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked();
        value = events->Get(context, key).ToLocalChecked();
        if(!value->IsArray())
            return;
        Local<Array> listeners = Local<Array>::Cast(value);
        for(uint32_t index = 0; index < listeners->Length(); index++) {
            value = listeners->Get(context, index).ToLocalChecked();
            if(!value->IsObject())
                return;
            Local<Object> listener = Local<Object>::Cast(value);
            key = String::NewFromUtf8(isolate, "paramTypes").ToLocalChecked();
            std::string paramTypes = *String::Utf8Value(isolate, listener->Get(context, key).ToLocalChecked());
            key = String::NewFromUtf8(isolate, "callback").ToLocalChecked();
            value = listener->Get(context, key).ToLocalChecked();
            if(!value->IsFunction())
                return;
            Local<Function> callback = Local<Function>::Cast(value);
            std::vector<Local<Value>> args = ToV8Args(amx, isolate, paramTypes, params);
            MaybeLocal<Value> returnValueMaybe = callback->Call(context, amxObject, args.size(), args.data());
            if(!returnValueMaybe.IsEmpty()) {
                Local<Value> returnValue = returnValueMaybe.ToLocalChecked();
                if(returnValue->IsInt32() && retval != nullptr)
                    *retval = (cell)returnValue->Int32Value(context).ToChecked();
            }
        }
    }

    void Environment::exit() {
        if(this->exited)
            return;
        using namespace v8;
        Isolate* isolate = this->setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        node::EmitProcessBeforeExit(this->setup->env());
        node::EmitProcessExit(this->setup->env());
    }

    void Environment::stop() {
        this->exit();
        node::Stop(this->setup->env());
        this->setup.reset();
    }
}
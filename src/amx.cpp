#include "amx.h"

namespace nodesamp {
    std::unordered_map<std::string, AMX_NATIVE> Amx::nativeCache = std::unordered_map<std::string, AMX_NATIVE>();

    AMX_NATIVE Amx::getNative(std::string& name) {
        AMX_NATIVE native;
        auto it = Amx::nativeCache.find(name);
        if(it != Amx::nativeCache.end())
            native = it->second;
        else {
            native = sampgdk::FindNative(name.c_str());
            if(native)
                Amx::nativeCache[name] = native;
        }
        return native;
    }

    v8::Local<v8::Value> Amx::createCallNativeResultEmpty(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Array> result = Array::New(isolate);
        Local<String> key = String::NewFromUtf8(isolate, "retval").ToLocalChecked();
        result->Set(context, key, Integer::New(isolate, 0)).FromMaybe(false);
        return result;
    }

    void Amx::callNative(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        HandleScope handleScope(isolate);
        if(!info[0]->IsString() || !info[0]->IsString())
            return info.GetReturnValue().Set(Amx::createCallNativeResultEmpty(info));
        std::string name = *String::Utf8Value(isolate, info[0]);
        AMX_NATIVE native = Amx::getNative(name);
        if(!native)
            return info.GetReturnValue().Set(Amx::createCallNativeResultEmpty(info));
        std::string paramTypes = *String::Utf8Value(isolate, info[1]);
        size_t paramCount = paramTypes.length();
        std::vector<void*> params(paramCount);
        std::vector<cell> paramsStorage(paramCount);
        Local<Context> context = isolate->GetCurrentContext();
        int argIndex = 2;
        std::string nativeFormat;
        for(size_t index = 0; index < paramCount; index++) {
            char type = paramTypes[index];
            switch(type) {
                case 'i':
                    paramsStorage[index] = info[argIndex]->Int32Value(context).ToChecked();
                    params[index] = (void*)&paramsStorage[index];
                    argIndex++;
                    nativeFormat += "i";
                break;
                case 'f': 
                {
                    float value = (float)info[argIndex]->NumberValue(context).ToChecked();
                    paramsStorage[index] = amx_ftoc(value);
                    params[index] = (void*)&paramsStorage[index];
                    argIndex++;
                    nativeFormat += "f";
                }
                break;
                case 's':
                {
                    std::string value = *String::Utf8Value(isolate, info[argIndex]);
                    value = Utils::ConvertUTF8ToCP1251(value);
                    size_t length = value.length();
                    char* pValue = new char[length + 1];
                    for(size_t vIndex = 0; vIndex < length; vIndex++)
                        pValue[vIndex] = value[vIndex];
                    pValue[length] = '\0';
                    params[index] = (void*)pValue;
                    argIndex++;
                    nativeFormat += "s";
                }
                break;
                case 'a': 
                {
                    Local<Array> array = Local<Array>::Cast(info[argIndex]);
                    uint32_t size = array->Length();
                    cell* value = new cell[size];
                    for(uint32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = array->Get(context, vIndex).ToLocalChecked()->Int32Value(context).ToChecked();
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "a[" + std::to_string(size) + "]";
                }
                break;
                case 'v': 
                {
                    Local<Array> array = Local<Array>::Cast(info[argIndex]);
                    uint32_t size = array->Length();
                    cell* value = new cell[size];
                    for(uint32_t vIndex = 0; vIndex < size; vIndex++) {
                        float fValue = (float)array->Get(context, vIndex).ToLocalChecked()->NumberValue(context).ToChecked();
                        value[vIndex] = amx_ftoc(fValue);
                    }
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "a[" + std::to_string(size) + "]";
                }
                break;
                case 'I':
                case 'F':
                    params[index] = (void*)&paramsStorage[index];
                    nativeFormat += "R";
                break;
                case 'S':
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    char* value = new char[size];
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "S[" + std::to_string(size) + "]";
                }
                break;
                case 'A':
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    cell* value = new cell[size];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = 0;
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "A[" + std::to_string(size) + "]";
                }
                break;
                case 'V': 
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    cell* value = new cell[size];
                    float fValue = 0.0;
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = amx_ftoc(vIndex);
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "A[" + std::to_string(size) + "]";
                }
                break;
            }
        }
        int32_t retval = sampgdk::InvokeNativeArray(native, nativeFormat.c_str(), params.data());
        Local<Array> result = Array::New(isolate);
        Local<String> key = String::NewFromUtf8(isolate, "retval").ToLocalChecked();
        result->Set(context, key, Integer::New(isolate, retval)).FromMaybe(false);
        for(size_t index = 0; index < paramCount; index++) {
            char type = paramTypes[index];
            switch(type) {
                case 's':
                    delete[] (char*)params[index];
                break;
                case 'a':
                case 'v':
                    delete[] (cell*)params[index];
                break;
                case 'I': 
                {
                    int32_t value = *(cell*)params[index];
                    result->Set(context, result->Length(), Integer::New(isolate, value)).FromMaybe(false);
                }
                break;
                case 'F': 
                {
                    float value = amx_ctof(*(cell*)params[index]);
                    result->Set(context, result->Length(), Number::New(isolate, value)).FromMaybe(false);
                } 
                break;
                case 'S': 
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    char* value = (char*)params[index];
                    value[size - 1] = '\0';
                    result->Set(context, result->Length(), String::NewFromUtf8(isolate, Utils::ConvertCP1251ToUTF8(value).c_str()).ToLocalChecked()).FromMaybe(false);
                    delete[] (char*)params[index];
                }
                break;
                case 'A':
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    Local<Array> array = Array::New(isolate, size);
                    cell* value = (cell*)params[index];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        array->Set(context, vIndex, Integer::New(isolate, value[index])).FromMaybe(false);
                    result->Set(context, result->Length(), array).FromMaybe(false);
                    delete[] (cell*)params[index];
                }
                break;
                case 'V':
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    Local<Array> array = Array::New(isolate, size);
                    cell* value = (cell*)params[index];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        array->Set(context, vIndex, Number::New(isolate, (double)amx_ctof(value[index]))).FromMaybe(false);
                    result->Set(context, result->Length(), array).FromMaybe(false);
                    delete[] (cell*)params[index];
                } 
                break;
            }
        }
        return info.GetReturnValue().Set(result);
    }

    void Amx::callNativeInFloat(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        HandleScope handleScope(isolate);
        if(!info[0]->IsString() || !info[0]->IsString())
            return info.GetReturnValue().Set(Amx::createCallNativeResultEmpty(info));
        std::string name = *String::Utf8Value(isolate, info[0]);
        AMX_NATIVE native = Amx::getNative(name);
        if(!native)
            return info.GetReturnValue().Set(Amx::createCallNativeResultEmpty(info));
        std::string paramTypes = *String::Utf8Value(isolate, info[1]);
        size_t paramCount = paramTypes.length();
        std::vector<void*> params(paramCount);
        std::vector<cell> paramsStorage(paramCount);
        Local<Context> context = isolate->GetCurrentContext();
        int argIndex = 2;
        std::string nativeFormat;
        for(size_t index = 0; index < paramCount; index++) {
            char type = paramTypes[index];
            switch(type) {
                case 'i':
                    paramsStorage[index] = info[argIndex]->Int32Value(context).ToChecked();
                    params[index] = (void*)&paramsStorage[index];
                    argIndex++;
                    nativeFormat += "i";
                break;
                case 'f': 
                {
                    float value = (float)info[argIndex]->NumberValue(context).ToChecked();
                    paramsStorage[index] = amx_ftoc(value);
                    params[index] = (void*)&paramsStorage[index];
                    argIndex++;
                    nativeFormat += "f";
                }
                break;
                case 's':
                {
                    std::string value = *String::Utf8Value(isolate, info[argIndex]);
                    value = Utils::ConvertUTF8ToCP1251(value);
                    size_t length = value.length();
                    char* pValue = new char[length + 1];
                    for(size_t vIndex = 0; vIndex < length; vIndex++)
                        pValue[vIndex] = value[vIndex];
                    pValue[length] = '\0';
                    params[index] = (void*)pValue;
                    argIndex++;
                    nativeFormat += "s";
                }
                break;
                case 'a': 
                {
                    Local<Array> array = Local<Array>::Cast(info[argIndex]);
                    uint32_t size = array->Length();
                    cell* value = new cell[size];
                    for(uint32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = array->Get(context, vIndex).ToLocalChecked()->Int32Value(context).ToChecked();
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "a[" + std::to_string(size) + "]";
                }
                break;
                case 'v': 
                {
                    Local<Array> array = Local<Array>::Cast(info[argIndex]);
                    uint32_t size = array->Length();
                    cell* value = new cell[size];
                    for(uint32_t vIndex = 0; vIndex < size; vIndex++) {
                        float fValue = (float)array->Get(context, vIndex).ToLocalChecked()->NumberValue(context).ToChecked();
                        value[vIndex] = amx_ftoc(fValue);
                    }
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "a[" + std::to_string(size) + "]";
                }
                break;
                case 'I':
                case 'F':
                    params[index] = (void*)&paramsStorage[index];
                    nativeFormat += "R";
                break;
                case 'S':
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    char* value = new char[size];
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "S[" + std::to_string(size) + "]";
                }
                break;
                case 'A':
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    cell* value = new cell[size];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = 0;
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "A[" + std::to_string(size) + "]";
                }
                break;
                case 'V': 
                {
                    int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                    paramsStorage[index] = (cell)size;
                    cell* value = new cell[size];
                    float fValue = 0.0;
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        value[vIndex] = amx_ftoc(vIndex);
                    params[index] = (void*)value;
                    argIndex++;
                    nativeFormat += "A[" + std::to_string(size) + "]";
                }
                break;
            }
        }
        int32_t retval = sampgdk::InvokeNativeArray(native, nativeFormat.c_str(), params.data());
        Local<Array> result = Array::New(isolate);
        Local<String> key = String::NewFromUtf8(isolate, "retval").ToLocalChecked();
        result->Set(context, key, Number::New(isolate, (double)amx_ctof(retval))).FromMaybe(false);
        for(size_t index = 0; index < paramCount; index++) {
            char type = paramTypes[index];
            switch(type) {
                case 's':
                    delete[] (char*)params[index];
                break;
                case 'a':
                case 'v':
                    delete[] (cell*)params[index];
                break;
                case 'I': 
                {
                    int32_t value = *(cell*)params[index];
                    result->Set(context, result->Length(), Integer::New(isolate, value)).FromMaybe(false);
                }
                break;
                case 'F': 
                {
                    float value = amx_ctof(*(cell*)params[index]);
                    result->Set(context, result->Length(), Number::New(isolate, value)).FromMaybe(false);
                } 
                break;
                case 'S': 
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    char* value = (char*)params[index];
                    value[size - 1] = '\0';
                    result->Set(context, result->Length(), String::NewFromUtf8(isolate, Utils::ConvertCP1251ToUTF8(value).c_str()).ToLocalChecked()).FromMaybe(false);
                    delete[] (char*)params[index];
                }
                break;
                case 'A':
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    Local<Array> array = Array::New(isolate, size);
                    cell* value = (cell*)params[index];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        array->Set(context, vIndex, Integer::New(isolate, value[index])).FromMaybe(false);
                    result->Set(context, result->Length(), array).FromMaybe(false);
                    delete[] (cell*)params[index];
                }
                break;
                case 'V':
                {
                    int32_t size = (int32_t)paramsStorage[index];
                    Local<Array> array = Array::New(isolate, size);
                    cell* value = (cell*)params[index];
                    for(int32_t vIndex = 0; vIndex < size; vIndex++)
                        array->Set(context, vIndex, Number::New(isolate, (double)amx_ctof(value[index]))).FromMaybe(false);
                    result->Set(context, result->Length(), array).FromMaybe(false);
                    delete[] (cell*)params[index];
                } 
                break;
            }
        }
        return info.GetReturnValue().Set(result);
    }

    void Amx::onPublicCall(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        HandleScope handleScope(isolate);
        if(!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsFunction())
            return info.GetReturnValue().Set(Null(isolate));
        Local<Context> context = isolate->GetCurrentContext();
        Local<String> eventName = Local<String>::Cast(info[0]);
        Local<String> paramTypes = Local<String>::Cast(info[1]);
        Local<Function> callback = Local<Function>::Cast(info[2]);
        Local<Object> amxObject = info.This();
        Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        Local<Object> events = Local<Object>::Cast(amxObject->Get(context, key).ToLocalChecked());     
        Local<Array> listeners;
        if(events->Get(context, eventName).ToLocalChecked()->IsArray())
            listeners = Local<Array>::Cast(events->Get(context, eventName).ToLocalChecked());
        else {
            listeners = Array::New(isolate);
            events->Set(context, eventName, listeners).FromMaybe(false);
        }
        Local<Object> listener = Object::New(isolate);
        key = String::NewFromUtf8(isolate, "paramTypes").ToLocalChecked();
        listener->Set(context, key, paramTypes).FromMaybe(false);
        key = String::NewFromUtf8(isolate, "callback").ToLocalChecked();
        listener->Set(context, key, callback).FromMaybe(false);
        uint32_t index = listeners->Length();
        listeners->Set(context, index, listener).FromMaybe(false);
        return info.GetReturnValue().Set(listener);
    }

    void Amx::removePublic(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        HandleScope handleScope(isolate);
        if(!info[0]->IsString())
            return info.GetReturnValue().Set(False(isolate));
        Local<Context> context = isolate->GetCurrentContext();
        Local<String> eventName = Local<String>::Cast(info[0]);
        Local<Object> amxObject = info.This();
        Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        Local<Object> events = Local<Object>::Cast(amxObject->Get(context, key).ToLocalChecked());
        if(!events->Get(context, eventName).ToLocalChecked()->IsArray())
            return info.GetReturnValue().Set(False(isolate));
        events->Delete(context, eventName).FromMaybe(false);
        return info.GetReturnValue().Set(True(isolate));
    }

    Amx::Amx(node::CommonEnvironmentSetup* setup): setup(setup) {}

    void Amx::init() {
        using namespace v8;
        node::CommonEnvironmentSetup* setup = this->getSetup();
        Isolate* isolate = setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        Local<Object> amxObject = Object::New(isolate);
        const std::pair<std::string, FunctionCallback> funcs[] = {
            {"callNative", this->callNative},
            {"callNativeInFloat", this->callNativeInFloat},
            {"onPublicCall", this->onPublicCall},
            {"removePublic", this->removePublic}
        };
        for(auto func: funcs) {
            Local<String> key = String::NewFromUtf8(isolate, func.first.c_str()).ToLocalChecked();
            Local<Function> value = FunctionTemplate::New(isolate, func.second)->GetFunction(context).ToLocalChecked();
            value->SetName(key);
            amxObject->Set(context, key, value).FromMaybe(false);
        }
        Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        Local<Object> value = Object::New(isolate);
        amxObject->Set(context, key, value).FromMaybe(false);
        key = String::NewFromUtf8(isolate, "amx").ToLocalChecked();
        context->Global()->Set(context, key, amxObject).FromMaybe(false);
    }

    inline node::CommonEnvironmentSetup* Amx::getSetup() {
        return this->setup;
    }

    void Amx::callPublic(AMX* amx, std::string eventName, cell* params, cell* retval) {
        using namespace v8;
        node::CommonEnvironmentSetup* setup = this->getSetup();
        Isolate* isolate = setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        Local<Object> global = context->Global();
        Local<String> key = String::NewFromUtf8(isolate, "amx").ToLocalChecked();
        Local<Object> amxObject = Local<Object>::Cast(global->Get(context, key).ToLocalChecked());
        key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
        Local<Object> events = Local<Object>::Cast(amxObject->Get(context, key).ToLocalChecked());
        key = String::NewFromUtf8(isolate, eventName.c_str()).ToLocalChecked();
        if(!events->Get(context, key).ToLocalChecked()->IsArray())
            return;
        Local<Array> listeners = Local<Array>::Cast(events->Get(context, key).ToLocalChecked());
        for(uint32_t index = 0; index < listeners->Length(); index++) {
            Local<Object> listener = Local<Object>::Cast(listeners->Get(context, index).ToLocalChecked());
            key = String::NewFromUtf8(isolate, "paramTypes").ToLocalChecked();
            String::Utf8Value string(isolate, listener->Get(context, key).ToLocalChecked());
            std::string paramTypes = *string;
            std::vector<Local<Value>> args(paramTypes.length());
            for(size_t index = 0; index < paramTypes.length(); index++) {
                char type = paramTypes[index];
                switch(type) {
                    case 'i':
                        args[index] = Integer::New(isolate, (int32_t)params[index + 1]);
                    break;
                    case 'f':
                        args[index] = Number::New(isolate, (double)amx_ctof(params[index + 1]));
                    break;
                    case 's': 
                    {
                        cell* addr = nullptr;
                        amx_GetAddr(amx, params[index + 1], &addr);
                        int length = 0;
                        amx_StrLen(addr, &length);
                        char* value = new char[length + 1];
                        amx_GetString(value, addr, 0, length + 1);
                        args[index] = String::NewFromUtf8(isolate, Utils::ConvertCP1251ToUTF8(value).c_str()).ToLocalChecked();
                        delete[] value;
                    } 
                    break;
                    case 'a': 
                    {
                        cell* addr = nullptr;
                        amx_GetAddr(amx, params[index + 1], &addr);
                        int32_t size = (int32_t)params[index + 2];
                        Local<Array> array = Array::New(isolate, size);
                        for(int32_t vIndex = 0; vIndex < size; vIndex++)
                            array->Set(context, vIndex, Integer::New(isolate, (int32_t)addr[index])).FromMaybe(false);
                        args[index] = array;
                    } 
                    break;
                    case 'v':
                    {
                        cell* addr = nullptr;
                        amx_GetAddr(amx, params[index + 1], &addr);
                        int32_t size = (int32_t)params[index + 2];
                        Local<Array> array = Array::New(isolate, size);
                        for(int32_t vIndex = 0; vIndex < size; vIndex++)
                            array->Set(context, vIndex, Number::New(isolate, (double)amx_ctof(addr[index]))).FromMaybe(false);
                        args[index] = array;
                    }
                    break;
                }
            }
            key = String::NewFromUtf8(isolate, "callback").ToLocalChecked();
            Local<Function> callback = Local<Function>::Cast(listener->Get(context, key).ToLocalChecked());
            Local<Value> returnValue = callback->Call(context, amxObject, args.size(), args.data()).ToLocalChecked();
            if(returnValue->IsInt32() && retval != nullptr)
                *retval = (cell)returnValue->Int32Value(context).ToChecked();
        }
    }
}
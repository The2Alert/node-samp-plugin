#include "amx-v8.h"
#include "iconvlite.h"

namespace nodesamp {
    std::vector<v8::Local<v8::Value>> ToV8Args(AMX* amx, v8::Isolate* isolate, std::string format, cell* params) {
        using namespace v8;
        Local<Context> context = isolate->GetCurrentContext();
        std::vector<Local<Value>> args(format.length());
        for(size_t index = 0; index < format.length(); index++) {
            char type = format[index];
            switch(type) {
                case 'i':
                    args[index] = Integer::New(isolate, (int32_t)params[index + 1]);
                break;
                case 'f':
                    args[index] = Number::New(isolate, (double)amx_ctof(params[index + 1]));
                break;
                case 's': {
                    cell* addr = nullptr;
                    amx_GetAddr(amx, params[index + 1], &addr);
                    int length = 0;
                    amx_StrLen(addr, &length);
                    char* string = new char[length + 1];
                    amx_GetString(string, addr, 0, length + 1);
                    args[index] = String::NewFromUtf8(isolate, cp2utf(string).c_str()).ToLocalChecked();
                    delete[] string;
                } break;
                case 'a': {
                    cell* addr = nullptr;
                    amx_GetAddr(amx, params[index + 1], &addr);
                    int32_t size = (int32_t)params[index + 2];
                    Local<Array> array = Array::New(isolate, size);
                    for(int32_t arrayIndex = 0; arrayIndex < size; arrayIndex++)
                        array->Set(context, arrayIndex, Integer::New(isolate, (int32_t)addr[arrayIndex])).FromMaybe(false);
                    args[index] = array;
                } break;
                case 'v': {
                    cell* addr = nullptr;
                    amx_GetAddr(amx, params[index + 1], &addr);
                    int32_t size = (int32_t)params[index + 2];
                    Local<Array> array = Array::New(isolate, size);
                    for(int32_t arrayIndex = 0; arrayIndex < size; arrayIndex++)
                        array->Set(context, arrayIndex, Number::New(isolate, (double)amx_ctof(addr[arrayIndex]))).FromMaybe(false);
                    args[index] = array;
                } break;
            }
        }
        return args;
    }
}
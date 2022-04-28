#include "amx-api.h"
#include <unordered_map>
#include <malloc.h>
#include "amx/amx.h"
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "iconvlite.h"
#include <string>
#include <vector>

namespace nodesamp {
    namespace API {
        namespace AMX {
            namespace Utils {
                AMX_NATIVE FindAmxNative(std::string name) {
                    static std::unordered_map<std::string, AMX_NATIVE> cache;
                    AMX_NATIVE native;
                    auto it = cache.find(name);
                    if(it != cache.end())
                        native = it->second;
                    else {
                        native = sampgdk::FindNative(name.c_str());
                        if(native)
                            cache[name] = native;
                    }
                    return native;
                }

                v8::Local<v8::Array> CreateCallNativeResultEmpty(v8::Isolate* isolate) {
                    using namespace v8;
                    Local<Context> context = isolate->GetCurrentContext();
                    Local<Array> result = Array::New(isolate);
                    Local<String> key = String::NewFromUtf8(isolate, "retval").ToLocalChecked();
                    result->Set(context, key, Integer::New(isolate, 0)).FromMaybe(false);
                    return result;
                }
            }

            v8::Local<v8::Array> CallNative(const v8::FunctionCallbackInfo<v8::Value>& info, bool retFloat) {
                using namespace v8;
                using namespace Utils;
                Isolate* isolate = info.GetIsolate();
                HandleScope handleScope(isolate);
                Local<Context> context = isolate->GetCurrentContext();
                std::string name = *String::Utf8Value(isolate, info[0]);
                AMX_NATIVE native = FindAmxNative(name);
                if(!native)
                    return CreateCallNativeResultEmpty(isolate);
                std::string paramTypes = info[1]->IsString() ? *String::Utf8Value(isolate, info[1]) : "";
                size_t paramsSize = paramTypes.length();
                std::vector<void*> params(paramsSize != 0 ? paramsSize : 1);
                std::vector<cell> paramsStorage(paramsSize);
                std::string nativeFormat;
                for(size_t index = 0, argIndex = 2; index < paramsSize; index++, argIndex++) {
                    char type = paramTypes[index];
                    switch(type) {
                        case 'i':
                            paramsStorage[index] = (cell)info[argIndex]->Int32Value(context).ToChecked();
                            params[index] = (void*)&paramsStorage[index];
                            nativeFormat += "i";
                        break;
                        case 'f': {
                            float value = (float)info[argIndex]->NumberValue(context).ToChecked();
                            paramsStorage[index] = amx_ftoc(value);
                            params[index] = (void*)&paramsStorage[index];
                            nativeFormat += "f";
                        } break;
                        case 's': {
                            std::string value = utf2cp(*String::Utf8Value(isolate, info[argIndex])); 
                            size_t length = value.length();
                            char* charValue = new char[length + 1];
                            for(size_t valueIndex = 0; valueIndex < length; valueIndex++)
                                charValue[valueIndex] = value[valueIndex];
                            charValue[length] = '\0';
                            params[index] = (void*)charValue;
                            nativeFormat += "s";
                        } break;
                        case 'a': {
                            uint32_t size;
                            cell* value;
                            if(info[argIndex]->IsArray()) {
                                Local<Array> array = Local<Array>::Cast(info[argIndex]);
                                size = array->Length();
                                value = new cell[size];
                                for(uint32_t valueIndex = 0; valueIndex < size; valueIndex++)
                                    value[valueIndex] = array->Get(context, valueIndex).ToLocalChecked()->Int32Value(context).ToChecked();
                            } else {
                                size = 0;
                                value = new cell[size];
                            }
                            params[index] = (void*)value;
                            nativeFormat += "a[" + std::to_string(size) + "]";
                        } break;
                        case 'v': {
                            uint32_t size;
                            cell* value;
                            if(info[argIndex]->IsArray()) {
                                Local<Array> array = Local<Array>::Cast(info[argIndex]);
                                size = array->Length();
                                value = new cell[size];
                                for(uint32_t valueIndex = 0; valueIndex < size; valueIndex++) {
                                    float floatValue = (float)array->Get(context, valueIndex).ToLocalChecked()->NumberValue(context).ToChecked();
                                    value[valueIndex] = amx_ftoc(floatValue);
                                }
                            } else {
                                size = 0;
                                value = new cell[size];
                            }
                            params[index] = (void*)value;
                            nativeFormat += "a[" + std::to_string(size) + "]";
                        } break;
                        case 'I':
                        case 'F':
                            params[index] = (void*)&paramsStorage[index];
                            nativeFormat += "R";
                        break;
                        case 'S': {
                            int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                            paramsStorage[index] = (cell)size;
                            char* value = new char[size];
                            params[index] = (void*)value;
                            nativeFormat += "S[" + std::to_string(size) + "]";
                        } break;
                        case 'A': {
                            int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                            paramsStorage[index] = (cell)size;
                            cell* value = new cell[size];
                            for(int32_t valueIndex = 0; valueIndex < size; valueIndex++)
                                value[valueIndex] = 0;
                            params[index] = (void*)value;
                            nativeFormat += "A[" + std::to_string(size) + "]";
                        } break;
                        case 'V': {
                            int32_t size = info[argIndex]->Int32Value(context).ToChecked();
                            paramsStorage[index] = (cell)size;
                            cell* value = new cell[size];
                            float floatValue = 0.0;
                            for(int32_t valueIndex = 0; valueIndex < size; valueIndex++)
                                value[valueIndex] = amx_ftoc(floatValue);
                            params[index] = (void*)value;
                            nativeFormat += "A[" + std::to_string(size) + "]";
                        } break;
                    }
                }
                Local<Array> result = Array::New(isolate);
                cell retval = sampgdk::InvokeNativeArray(native, nativeFormat.c_str(), params.data());
                Local<String> key = String::NewFromUtf8(isolate, "retval").ToLocalChecked();
                Local<Value> value;
                if(retFloat)
                    value = Number::New(isolate, (double)amx_ctof(retval));
                else value = Integer::New(isolate, (int32_t)retval);
                result->Set(context, key, value).FromMaybe(false);
                for(size_t index = 0; index < paramsSize; index++) {
                    char type = paramTypes[index];
                    switch(type) {
                        case 's':
                            delete[] (char*)params[index];
                        break;
                        case 'a':
                        case 'v':
                            delete[] (cell*)params[index];
                        break;
                        case 'I': {
                            int32_t value = (int32_t)*(cell*)params[index];
                            result->Set(context, result->Length(), Integer::New(isolate, value)).FromMaybe(false);
                        } break;
                        case 'F': {
                            double value = (double)amx_ctof(*(cell*)params[index]);
                            result->Set(context, result->Length(), Number::New(isolate, value)).FromMaybe(false);
                        } break;
                        case 'S': {
                            int32_t size = (int32_t)paramsStorage[index];
                            char* value = (char*)params[index];
                            value[size - 1] = '\0';
                            result->Set(context, result->Length(), String::NewFromUtf8(isolate, cp2utf(value).c_str()).ToLocalChecked()).FromMaybe(false);
                            delete[] (char*)params[index];
                        } break;
                        case 'A': {
                            int32_t size = (int32_t)paramsStorage[index];
                            Local<Array> array = Array::New(isolate, size);
                            cell* value = (cell*)params[index];
                            for(int32_t valueIndex = 0; valueIndex < size; valueIndex++)
                                array->Set(context, valueIndex, Integer::New(isolate, (int32_t)value[valueIndex])).FromMaybe(false);
                            result->Set(context, result->Length(), array).FromMaybe(false);
                            delete[] (cell*)params[index];
                        } break;
                        case 'V': {
                            int32_t size = (int32_t)paramsStorage[index];
                            Local<Array> array = Array::New(isolate, size);
                            cell* value = (cell*)params[index];
                            for(int32_t valueIndex = 0; valueIndex < size; valueIndex++)
                                array->Set(context, valueIndex, Number::New(isolate, (double)amx_ctof(value[valueIndex]))).FromMaybe(false);
                            result->Set(context, result->Length(), array).FromMaybe(false);
                            delete[] (cell*)params[index];
                        } break;
                    }
                }
                return result;
            }

            void CallNative(const v8::FunctionCallbackInfo<v8::Value>& info) {
                return info.GetReturnValue().Set(CallNative(info, false));
            }

            void CallNativeInFloat(const v8::FunctionCallbackInfo<v8::Value>& info) {
                return info.GetReturnValue().Set(CallNative(info, true));
            }

            void OnPublicCall(const v8::FunctionCallbackInfo<v8::Value>& info) {
                using namespace v8;
                Isolate* isolate = info.GetIsolate();
                HandleScope handleScope(isolate);
                Local<Context> context = isolate->GetCurrentContext();
                Local<String> name = info[0]->ToString(context).ToLocalChecked();
                Local<String> paramTypes;
                Local<Function> callback;
                if(info[1]->IsFunction()) {
                    paramTypes = String::NewFromUtf8(isolate, "").ToLocalChecked();
                    callback = Local<Function>::Cast(info[1]);
                } else {
                    paramTypes = info[1]->ToString(context).ToLocalChecked();
                    if(info[2]->IsFunction())
                        callback = Local<Function>::Cast(info[2]);
                    else return info.GetReturnValue().Set(Null(isolate));
                }
                Local<Object> amx = info.This();
                Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
                Local<Value> value = amx->Get(context, key).ToLocalChecked();
                if(!value->IsObject())
                    return info.GetReturnValue().Set(Null(isolate));
                Local<Object> events = Local<Object>::Cast(value);     
                Local<Array> listeners;
                value = events->Get(context, name).ToLocalChecked();
                if(value->IsArray())
                    listeners = Local<Array>::Cast(value);
                else {
                    listeners = Array::New(isolate);
                    events->Set(context, name, listeners).FromMaybe(false);
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

            void RemovePublic(const v8::FunctionCallbackInfo<v8::Value>& info) {
                using namespace v8;
                Isolate* isolate = info.GetIsolate();
                HandleScope handleScope(isolate);
                Local<Context> context = isolate->GetCurrentContext();
                Local<String> name = info[0]->ToString(context).ToLocalChecked();
                Local<Object> amx = info.This();
                Local<String> key = String::NewFromUtf8(isolate, "publicEvents").ToLocalChecked();
                Local<Value> value = amx->Get(context, key).ToLocalChecked();
                if(!value->IsObject())
                    return info.GetReturnValue().Set(Null(isolate));
                Local<Object> events = Local<Object>::Cast(value);
                if(!events->Get(context, name).ToLocalChecked()->IsArray())
                    return info.GetReturnValue().Set(False(isolate));
                events->Delete(context, name).FromMaybe(false);
                return info.GetReturnValue().Set(True(isolate));
            }
        }
    }
}
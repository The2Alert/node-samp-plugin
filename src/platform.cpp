#include "platform.h"
#include "env.h"
#include "json.hpp"

namespace nodesamp {
    Platform::Platform(const std::vector<std::string>& options, std::string path, const std::vector<std::string>& args): path(path) {
        this->args.push_back("");
        for(std::string option: options)
            this->args.push_back(option);
        this->args.push_back(path);
        for(std::string arg: args)
            this->args.push_back(arg);
    }

    void Platform::init() {
        using namespace v8;
        std::vector<std::string> execArgs;
        std::vector<std::string> errors;
        node::InitializeNodeWithArgs(&this->args, &execArgs, &errors);
        this->platform = node::MultiIsolatePlatform::Create(4);
        V8::InitializePlatform(this->platform.get());
        V8::Initialize();
    }

    Environment* Platform::loadDefaultEnv() {
        if(this->defaultEnv != nullptr)
            return this->defaultEnv.get();
        this->defaultEnv = std::unique_ptr<Environment>(new Environment(this, ""));
        this->defaultEnv->load();
        return this->defaultEnv.get();
    }

    Environment* Platform::getDefaultEnv() {
        return this->defaultEnv.get();
    }

    void Platform::stopDefaultEnv() {
        if(this->defaultEnv == nullptr)
            return;
        this->defaultEnv->stop();
        this->defaultEnv.reset();
    }

    void Platform::tick() {
        if(this->defaultEnv == nullptr)
            return;
        this->defaultEnv->tick();
    }

    void Platform::callAmxPublic(AMX* amx, std::string name, cell* params, cell* retval) {
        if(this->defaultEnv == nullptr)
            return;
        this->defaultEnv->callAmxPublic(amx, name, params, retval);
    }

    void Platform::uninit() {
        using namespace v8;
        this->stopDefaultEnv();
        V8::Dispose();
        V8::ShutdownPlatform();
        this->platform.reset();
    }

    std::unique_ptr<Platform> InitPlatform(const std::vector<std::string>& options, std::string path, const std::vector<std::string>& args) {
        std::unique_ptr<Platform> platform(new Platform(options, path, args));
        platform->init();
        return platform;
    }
}
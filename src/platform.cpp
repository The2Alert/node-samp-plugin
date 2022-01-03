#include "platform.h"
#include "amx.h"

namespace nodesamp {
    Platform::Platform(std::string packagePath, const std::vector<std::string>& options) {
        Args args = {""};
        for(std::string arg: options)
            args.push_back(arg);
        args.push_back(packagePath);
        this->args = args;
    }

    void Platform::init() {
        using namespace v8;
        std::vector<std::string> errors;
        node::InitializeNodeWithArgs(&this->getArgs(), &this->getExecArgs(), &errors);
        this->platform = node::MultiIsolatePlatform::Create(4);
        V8::InitializePlatform(this->getPlatform());
        V8::Initialize();
    }

    Platform::Args& Platform::getArgs() {
        return this->args;
    }

    Platform::Args& Platform::getExecArgs() {
        return this->execArgs;
    }

    node::MultiIsolatePlatform* Platform::getPlatform() {
        return this->platform.get();
    }
    
    void Platform::initModule() {
        this->module = std::unique_ptr<Module>(new Module(this));
        this->getModule()->init();
    }

    Module* Platform::getModule() {
        return this->module.get();
    }

    void Platform::uninitModule() {
        this->getModule()->uninit();
        this->module.reset();
    }
    
    void Platform::tick() {
        this->getModule()->tick();
    }
    
    void Platform::uninit() {
        using namespace v8;
        V8::Dispose();
        V8::ShutdownPlatform();
    }

    std::unique_ptr<Platform> CreatePlatform(std::string packagePath, const std::vector<std::string>& options) {
        return std::unique_ptr<Platform>(new Platform(packagePath, options));
    }
}
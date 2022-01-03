#include "module.h"
#include "platform.h"

namespace nodesamp {
    Module::Module(Platform* platform): platform(platform) {}

    void Module::init() {
        using namespace v8;
        node::MultiIsolatePlatform* platform = this->platform->getPlatform();
        std::vector<std::string> errors;
        Platform::Args& args = this->platform->getArgs();
        Platform::Args& execArgs = this->platform->getExecArgs();
        this->setup = node::CommonEnvironmentSetup::Create(platform, &errors, args, execArgs);
        node::CommonEnvironmentSetup* setup = this->getSetup();
        Isolate* isolate = setup->isolate();
        isolate->SetFatalErrorHandler([](const char* location, const char* message) {
            exit(0);
        });
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Local<Context> context = setup->context();
        Context::Scope contextScope(context);
        this->amx = std::unique_ptr<Amx>(new Amx(setup));
        Amx* amx = this->getAmx();
        amx->init();
        node::LoadEnvironment(
            setup->env(),
            "globalThis.require = require(\"module\").createRequire(process.execPath);"
            "globalThis.require(process.argv[process.argv.length - 1]);"
        );
    }

    inline node::CommonEnvironmentSetup* Module::getSetup() {
        return this->setup.get();
    }

    Amx* Module::getAmx() {
        return this->amx.get();
    }

    void Module::tick() {
        using namespace v8;
        node::CommonEnvironmentSetup* setup = this->getSetup();
        Isolate* isolate = setup->isolate();
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Context::Scope contextScope(setup->context());
        uv_run(setup->event_loop(), UV_RUN_NOWAIT);
    }
    
    void Module::uninit() {
        node::Stop(this->getSetup()->env());
        this->setup.reset();
    }
}
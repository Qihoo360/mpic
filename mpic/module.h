#pragma once

#include <memory>

namespace mpic {

class Option;

class Resource {
public:
    // The resource object is created in master process
    // and it will not be changed when this mpic framework reloads
    virtual bool Init(const Option* op) = 0;
    virtual ~Resource() {}
};

class Module {
public:
    // This will be called in master process
    // It will be reloaded immediately when this mpic framework reloads
    virtual bool InitInMaster(const Option* op) = 0;

    // This will be called in worker process
    virtual bool InitInWorker(const Option* op) {
        return true;
    }

    // This will be called in master process
    virtual void Uninit() {}

    virtual int Run() = 0; // Run in worker process

    virtual ~Module() {}

    void SetResource(Resource* r) {
        resource_ = r;
    }

protected:
    Resource* resource_;
};

}


#ifdef _WIN32
#define MPIC_EXPORT_DLL __declspec(dllexport)
#else
#define MPIC_EXPORT_DLL
#endif

#define MPIC_CREATE_MODULE(M) \
    extern "C" MPIC_EXPORT_DLL void* MPIC_NewModule() \
    {return new M;}

#define MPIC_CREATE_RESOURCE(R) \
    extern "C" MPIC_EXPORT_DLL void* MPIC_NewResource() \
    {return new R;}

#define EXPORT_MPIC_MODULE(Module, Resource) \
    MPIC_CREATE_MODULE(Module); \
    MPIC_CREATE_RESOURCE(Resource);

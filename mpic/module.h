#pragma once

#include <memory>

namespace mpic {

class Option;

class Resource {
public:
    // The resource object is created in master process
    // and it will not be reloaded when this mpic framework reloads
    virtual bool Init(const std::shared_ptr<Option>& op) = 0;
    virtual ~Resource() {}
};

class Module {
public:
    virtual bool Init(const std::shared_ptr<Option>& op) = 0;
    virtual void Uninit() {}
    virtual int Run() = 0; // run in worker process
    virtual ~Module() {}

    void SetResource(Resource* r) {
        resource_ = r;
    }
protected:
    Resource* resource_;
};

}

#define MPIC_CREATE_MODULE(M) \
    extern "C" ::mpic::Module* MPIC_NewModule() \
    {return new M;}

#define MPIC_CREATE_RESOURCE(R) \
    extern "C" ::mpic::Resource* MPIC_NewResource() \
    {return new R;}


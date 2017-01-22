
#include "mpic/exp.h"

#include "nfmpic_resource.h"

namespace nfmpic {
class EchoModule : public mpic::Module {
public:
    virtual bool InitInMaster(const mpic::Option* op);
    virtual bool InitInWorker(const mpic::Option* op);
    virtual int Run();

    Resource* GetResource() const {
        return static_cast<nfmpic::Resource*>(resource_);
    }

};
}

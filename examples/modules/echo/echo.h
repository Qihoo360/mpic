
#include "mpic/exp.h"

namespace echo {
class EchoResource : public mpic::Resource {
public:
    virtual bool Init(const mpic::Option* op) {
        return true;
    }
};

class EchoModule : public mpic::Module {
public:
    virtual bool InitInMaster(const mpic::Option* op);

    virtual int Run();
private:
    const mpic::Option* option_;
};
}

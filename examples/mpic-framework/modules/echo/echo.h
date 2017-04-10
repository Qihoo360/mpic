
#include "mpic/module.h"

namespace echo {
class EchoModule : public mpic::Module {
public:
    virtual bool InitInMaster(const mpic::Option* op);

    virtual int Run();
private:
    const mpic::Option* option_;
};
}

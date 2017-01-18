
#include "mpic/exp.h"

namespace echo {
class EchoResource : public mpic::Resource {
public:
    virtual bool Init(const std::shared_ptr<mpic::Option>& op) {
        return true;
    }
};

class EchoModule : public mpic::Module {
public:
    virtual bool Init(const std::shared_ptr<mpic::Option>& op);

    virtual int Run();
private:
    std::shared_ptr<mpic::Option> option_;
};
}

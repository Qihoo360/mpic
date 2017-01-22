#include <mpic/option.h>
#include <vector>

namespace nfmpic {

class Option : public mpic::Option {
public:
    Option();
    virtual bool Init(int argc, char** argv);

    const std::string& http_ports() const {
        return http_ports_;
    }

    int tcp_port() const {
        return tcp_port_;
    }

    const std::string& udp_ports() const {
        return udp_ports_;
    }

    int tcp_thread_pool_size() const {
        return tcp_thread_pool_size_;
    }

    int http_thread_pool_size() const {
        return http_thread_pool_size_;
    }
private:
    std::string http_ports_;
    std::string udp_ports_;
    int tcp_port_;
    int tcp_thread_pool_size_;
    int http_thread_pool_size_;
};

}


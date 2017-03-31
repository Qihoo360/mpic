
#include <mpic/option.h>
#include <boost/program_options.hpp>

namespace nfmpic {

class Option : public mpic::Option {
public:
    bool Init(int argc, char** argv) {
        mpic::po::options_description opts("network service options");
        opts.add_options()
            ("tcp-thread-pool-size",
             mpic::po::value<int>(&tcp_thread_pool_size_)->default_value(tcp_thread_pool_size_),
             "the tcp server thread pool size")
            ("http-thread-pool-size",
             mpic::po::value<int>(&http_thread_pool_size_)->default_value(http_thread_pool_size_),
             "the http server thread pool size")
            ("tcp-port",
             mpic::po::value<int>(&tcp_port_)->default_value(tcp_port_),
             "the tcp port")
            ("http-port",
             mpic::po::value<std::string>(&http_ports_)->default_value(http_ports_),
             "the http port")
            ("udp-port",
             mpic::po::value<std::string>(&udp_ports_)->default_value(udp_ports_),
             "the udp port");
        AddOption(opts);

        return mpic::Option::Init(argc, argv);
    }

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
    int tcp_thread_pool_size_ = 4;
    int http_thread_pool_size_ = 4;
    int tcp_port_ = 8070;
    std::string http_ports_ = "8080,8081";
    std::string udp_ports_ = "1053,1054";
};

}

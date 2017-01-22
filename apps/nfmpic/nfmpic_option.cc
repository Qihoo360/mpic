#include "nfmpic_option.h"

#include <gflags/gflags.h>

#include <mpic/option.h>
#include <vector>

DEFINE_string(http_port, "8080,8090", "The listening ports of the http server. We can give more than 1 port using comma to separate them.");
DEFINE_int32(tcp_port, 8081, "The listening port of the tcp server.");
DEFINE_string(udp_port, "5353", "The listening ports of the udp server. We can give more than 1 port using comma to separate them.");
DEFINE_int32(tcp_thread_pool_size, 12, "The thread number in the tcp server's working thread pool");
DEFINE_int32(http_thread_pool_size, 12, "The thread number in the http server's working thread pool");
// The udp server thread pool is shared with http server

namespace nfmpic {

Option::Option() {}

bool Option::Init(int argc, char** argv) {
    bool rc = mpic::Option::Init(argc, argv);
    if (!rc) {
        LOG(ERROR) << "mpic::Option::Init failed.";
        return false;
    }

    http_ports_ = FLAGS_http_port;
    tcp_port_ = FLAGS_tcp_port;
    udp_ports_ = FLAGS_udp_port;


    return true;
}

}


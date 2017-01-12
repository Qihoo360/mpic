#pragma once

#include <functional>

namespace mpic {

class Monitor {
public:
    /** return 0 if OK, others failed */
    typedef std::function< int() > WorkerMainRoutine;

    bool Init(int argc, char** argv);

    ~Monitor();

    /**
     * @param worker_main - the child worker process main routine
     * @return 0 if OK, others failed
     */
    int Run(WorkerMainRoutine worker_main);

public:
    WorkerMainRoutine worker_main_routine() {
        return worker_main_routine_;
    }

    static Monitor& instance() {
        return instance_;
    }

private:
    Monitor();
    Monitor(const Monitor& rhs) {}

private:
    WorkerMainRoutine worker_main_routine_;
    static Monitor instance_;
};
}



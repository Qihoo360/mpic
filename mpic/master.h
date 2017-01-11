#pragma once

#include <functional>

namespace mpic {

class Master {
public:
    /** return 0 if OK, others failed */
    typedef std::function< int() > WorkerMainRoutine;

    bool Init(int argc, char** argv);

    ~Master();

    /**
     * @param worker_main - the child worker process main routine
     * @return 0 if OK, others failed
     */
    int Run(WorkerMainRoutine worker_main);

public:
    WorkerMainRoutine worker_main_routine() {
        return worker_main_routine_;
    }

    int worker_processes() const {
        return worker_processes_;
    }

    static Master& instance() {
        return instance_;
    }

private:
    Master();
    Master(const Master& rhs) {}

private:
    WorkerMainRoutine worker_main_routine_;
    int worker_processes_;
    static Master instance_;
};
}



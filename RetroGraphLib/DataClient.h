#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "utils.h"

namespace rg {

class DataClient {
public:
    DataClient();
    DataClient(const DataClient&) { RGERROR("Not Implemented"); }
    DataClient& operator=(const DataClient&) { RGERROR("Not Implemented"); }
    DataClient(DataClient&&);
    DataClient& operator=(DataClient&&);
    ~DataClient();

private:
    void startNetworkThread();

    void destroyListenThread();

    std::condition_variable cv;
    std::atomic<bool> m_threadRunning;
    std::thread m_listenThread;
};

}


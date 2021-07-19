#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "utils.h"

namespace rg {

class DataClient {
public:
    DataClient() noexcept;
    DataClient(const DataClient&) noexcept { RGERROR("Not Implemented"); }
    DataClient& operator=(const DataClient&) noexcept { RGERROR("Not Implemented"); }
    DataClient(DataClient&&) noexcept;
    DataClient& operator=(DataClient&&) noexcept;
    ~DataClient();

private:
    void startNetworkThread();

    void destroyListenThread();

    std::condition_variable cv;
    std::atomic<bool> m_threadRunning;
    std::thread m_listenThread;
};

}


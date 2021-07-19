#include "stdafx.h"
#include "DataClient.h"


namespace rg {

static std::mutex m;

DataClient::DataClient() noexcept {
    startNetworkThread();
}

DataClient::~DataClient() {
    destroyListenThread();
}

DataClient::DataClient(DataClient&& other)  noexcept
    : cv{}
    , m_threadRunning{ other.m_threadRunning.load() }
    , m_listenThread{  }
{

    m_listenThread = std::move(other.m_listenThread);
    // Clean up the moved-from object
    other.m_threadRunning.store(false);
}

DataClient& DataClient::operator=(DataClient&& other) noexcept {
    if (this != &other) {
        m_threadRunning = other.m_threadRunning.load();
        m_listenThread = std::move(other.m_listenThread);
        other.m_threadRunning.store(false);
    }
    return *this;
}

void DataClient::startNetworkThread() {
    using namespace std::chrono_literals;

    // Start thread that periodically checks connection to internet.
    m_threadRunning.store(true);
    m_listenThread = std::thread{ [this]() {
        SetThreadDescription(GetCurrentThread(), L"DataClientThread");

        bool tmp{ m_threadRunning.load() };
        while (tmp) {
            std::unique_lock<std::mutex> lg{ m };

            std::cout << "Shit\n";

            cv.wait_for(lg, 1000ms, [this]() { return !m_threadRunning.load(); });
            tmp = m_threadRunning.load();
        }
    }};

}

void DataClient::destroyListenThread() {
    // End the background thread
    if (m_threadRunning.load()) {
        m_threadRunning.store(false);
        cv.notify_all();
        m_listenThread.join();
    }
}

}

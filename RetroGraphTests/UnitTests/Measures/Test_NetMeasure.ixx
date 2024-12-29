export module UnitTests.Test_NetMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std;

import "Catch2HeaderUnit.h";

constexpr std::chrono::milliseconds testMeasureUpdateInterval{ 10 };

export class TestNetDataSource : public rg::INetDataSource {
public:
    bool updateBestAdapter() override { return false; }
    void updateNetTraffic() override {}
    bool checkConnectionStatusChanged() override { return false; }

    int64_t getDownBytes() const override { return downBytes; }
    int64_t getUpBytes() const override { return upBytes; }

    const std::string& getDNS() const override { return dnsIP; }
    const std::string& getHostname() const override { return hostname; }
    const std::string& getAdapterName() const override { return mainAdapterName; }
    const std::string& getAdapterMAC() const override { return mainAdapterMAC; }
    const std::string& getAdapterIP() const override { return mainAdapterIP; }
    bool isConnected() const override { return connected; }

    int64_t downBytes;
    int64_t upBytes;
    std::string dnsIP;
    std::string hostname;
    std::string mainAdapterName;
    std::string mainAdapterMAC;
    std::string mainAdapterIP;
    bool connected;
};

TEST_CASE("Measures::NetMeasure. Update", "[measure]") {
    auto netDataSource{ std::make_unique<TestNetDataSource>() };
    auto* netDataSourceRaw{ netDataSource.get() };
    rg::NetMeasure measure{ testMeasureUpdateInterval, std::move(netDataSource) };

    netDataSourceRaw->downBytes = 0;
    netDataSourceRaw->upBytes = 0;
    netDataSourceRaw->dnsIP = "8.8.8.8";
    netDataSourceRaw->hostname = "TEST-PC";
    netDataSourceRaw->mainAdapterName = "Ethernet Adapter";
    netDataSourceRaw->mainAdapterMAC = "AA-AA-AA-AA";
    netDataSourceRaw->mainAdapterIP = "192.168.1.1";
    netDataSourceRaw->connected = false;

    int64_t eventDownBytes{ 0 };
    int64_t eventUpBytes{ 0 };
    bool eventIsConnected{ false };
    bool eventBestAdapterChanged{ false };
    const auto handle{ measure.onDownBytes.attach([&](int64_t usage) { eventDownBytes = usage; }) };
    const auto handle2{ measure.onUpBytes.attach([&](int64_t usage) { eventUpBytes = usage; }) };
    const auto handle3{ measure.onConnectionStatusChanged.attach(
        [&](bool isConnected) { eventIsConnected = isConnected; }) };
    const auto handle4{ measure.onBestAdapterChanged.attach([&]() { eventBestAdapterChanged = true; }) };

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        REQUIRE(measure.getAdapterName() == "Ethernet Adapter");
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

        SECTION("Update when no change since last update") {
            measure.update();

            REQUIRE(measure.getAdapterName() == "Ethernet Adapter");
        }

        SECTION("Update when change since last update") {
            netDataSourceRaw->mainAdapterName = "VPN Adapter";
            measure.update();

            REQUIRE(measure.getAdapterName() == "VPN Adapter");
        }
    }

    measure.onDownBytes.detach(handle);
    measure.onUpBytes.detach(handle2);
    measure.onConnectionStatusChanged.detach(handle3);
    measure.onBestAdapterChanged.detach(handle4);
}

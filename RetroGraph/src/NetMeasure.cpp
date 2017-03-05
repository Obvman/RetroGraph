#include "../headers/NetMeasure.h"

#include <Windows.h>
#include <Iphlpapi.h>

#pragma comment (lib, "Iphlpapi.lib")

namespace rg {

NetMeasure::NetMeasure() {

    GetAdaptersInfo(nullptr, nullptr);
}


NetMeasure::~NetMeasure() {

}

}
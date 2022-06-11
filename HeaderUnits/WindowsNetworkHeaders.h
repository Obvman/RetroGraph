#pragma once

// Must be included in this order due to nasty conflicts with the networking libraries.

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <Iphlpapi.h>
#include <wininet.h>
#include <icmpapi.h>
#include <Ws2tcpip.h>

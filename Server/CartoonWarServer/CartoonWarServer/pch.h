#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_set>
#include <atomic>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
using namespace chrono;

#include "protocol.h"
#include "Struct.h"
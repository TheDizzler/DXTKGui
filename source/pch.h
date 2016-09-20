#pragma once

/* Global variables and includes */		
#include <Windows.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <comdef.h>
#include <algorithm> 
#include <cctype>
#include <sstream>
#include <WinSDKVer.h>		// these are necessary for XAudio2.8
#define _WIN32_WINNT 0x0602	// ^
#include <SDKDDKVer.h>		// ^

#include "Audio.h"
#include "globals.h"


using namespace Microsoft::WRL;

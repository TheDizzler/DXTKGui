#pragma once

#include <comdef.h>
#include <algorithm> 
#include <cctype>
#include <sstream>
#include <functional>



namespace GUIAssets {

	const static char* assetManifestFile = "assets/AssetManifest.xml";
	const static char* defaultFontFile = "assets/Fonts/Arial.spritefont";

	/** Print error to log file?? */
	static inline bool reportError(HRESULT hr,
		std::wstring failMessage = L"This is SRS Error",
		std::wstring failTitle = L"Fatal Error") {

		if (FAILED(hr)) {

			_com_error err(hr);
			std::wostringstream wss;
			wss << failMessage;
			wss << "\nHRESULT: " << err.ErrorMessage();
			/*if (!Globals::FULL_SCREEN)
				MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);
			else*/
			OutputDebugString(wss.str().c_str());
			return true;
		}

		return false;
	}


};

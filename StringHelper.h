#pragma once

#include <algorithm>
#include <cctype>
#include <functional>

// for error reporting
#include <sstream>
#include <comdef.h>


namespace StringHelper {

	inline wchar_t* convertCharStarToWCharT(const char* text) {

		const size_t cSize = strlen(text) + 1;
		wchar_t* wc = new wchar_t[cSize];
		size_t outsize;
		mbstowcs_s(&outsize, wc, cSize, text, cSize - 1);

		return wc;
	}

	// trim from start (in place)
	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	/** WARNING: if game is fullscreen, showing a message box will cause bad times!
		Freezes! Forced Reboots! Demon Summons! You have been warned!!! */
	static inline void reportError(
		wstring failMessage = L"This is SRS Error",
		wstring failTitle = L"Fatal Error", bool showMessageBox = false) {

		wostringstream wss;
		wss << failMessage;
		if (showMessageBox)
			MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);

		OutputDebugString(wss.str().c_str());
	}

	/** WARNING: if game is fullscreen, showing a message box will cause bad times!
		Freezes! Forced Reboots! Demon Summons! You have been warned!!!*/
	static inline bool reportError(HRESULT hr,
		wstring failMessage = L"This is SRS Error",
		wstring failTitle = L"Fatal Error", bool showMessageBox = false) {

		if (FAILED(hr)) {

			_com_error err(hr);
			wostringstream wss;
			wss << failMessage;
			wss << "\nHRESULT: " << err.ErrorMessage() << endl;
			if (showMessageBox)
				MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);

			OutputDebugString(wss.str().c_str());
			return true;
		}

		return false;
	}
};
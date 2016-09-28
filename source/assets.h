#include "pch.h"
#pragma once



namespace Assets {

	const static char* assetManifestFile = "assets/AssetManifest.xml";
	const static char* defaultFontFile = "assets/Fonts/Arial.spritefont";


	inline wchar_t* convertCharStarToWCharT(const char* text) {

		const size_t cSize = strlen(text) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, text, cSize);

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

	
	static inline bool reportError(HRESULT hr,
		wstring failMessage = L"This is SRS Error",
		wstring failTitle = L"Fatal Error") {

		if (FAILED(hr)) {

			_com_error err(hr);
			wostringstream wss;
			wss << failMessage;
			wss << "\nHRESULT: " << err.ErrorMessage();
			if (!Globals::FULL_SCREEN)
				MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);
			else
				OutputDebugString(wss.str().c_str());
			return true;
		}

		return false;
	}

	//static void getTextureDimensions(ID3D11Resource* res, UINT* width, UINT* height) {

	//	D3D11_RESOURCE_DIMENSION dim;
	//	res->GetType(&dim);

	//	switch (dim) {
	//		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	//		{

	//			auto texture = reinterpret_cast<ID3D11Texture2D*>(res);
	//			D3D11_TEXTURE2D_DESC desc;
	//			texture->GetDesc(&desc);
	//			if (width)
	//				*width = desc.Width; // width in pixels
	//			if (height)
	//				*height = desc.Height; // height in pixels

	//			break;

	//		}
	//		default:

	//			if (width)
	//				*width = 0; // width in pixels
	//			if (height)
	//				*height = 0; // height in pixels


	//	}
	//}

};

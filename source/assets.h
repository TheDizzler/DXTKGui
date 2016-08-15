#pragma once


/* Global variables and includes */
#include <wrl.h>
#include <comdef.h>
#include <algorithm> 
#include <cctype>


using namespace Microsoft::WRL;


namespace Assets {

	const static char* assetManifestFile = "assets/AssetManifest.xml";
	//const static wchar_t* arialFontFile = L"assets/Fonts/Arial.spritefont";

	//const static wchar_t* mouseIconFile = L"assets/GUI/mouse icon.dds";
	//const static wchar_t* mouseReticleFile = L"assets/GUI/reticle (16x16).dds";
	//const static wchar_t* buttonUpFile = L"assets/GUI/button up (256x64).dds";
	//const static wchar_t* buttonDownFile = L"assets/GUI/button down (256x64).dds";

	//const static wchar_t* whitePixelFile = L"assets/GUI/WhitePixel.dds";

	///** Excessive buttons are excessive. */
	//const static wchar_t* comboButtonClosedFile
	//	= L"assets/GUI/ComboButtonClosed.dds";
	//const static wchar_t* comboButtonPressedClosedFile
	//	= L"assets/GUI/ComboButtonPressedClosed.dds";
	//const static wchar_t* comboButtonOpenFile = L"assets/GUI/ComboButtonOpen.dds";
	//const static wchar_t* comboButtonPressedOpenFile
	//	= L"assets/GUI/ComboButtonPressedOpen.dds";
	//const static wchar_t* scrollBarUpFile = L"assets/GUI/ScrollBarUp.dds";
	//const static wchar_t* scrollBarUpPressedFile
	//	= L"assets/GUI/ScrollBarUpPressed.dds";
	//const static wchar_t* scrollBarDownFile = L"assets/GUI/ScrollBarDown.dds";
	//const static wchar_t* scrollBarDownPressedFile
	//	= L"assets/GUI/ScrollBarDownPressed.dds";

	///** This needs to be rejiggered. */
	//const static wchar_t* pauseOverlayFile = L"assets/GUI/pinkblock(16x16).dds";
	//const static wchar_t* uglyDialogBox = L"assets/GUI/dialog pink (200x200).dds";



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

	static void getTextureDimensions(ID3D11Resource* res, UINT* width, UINT* height) {

		D3D11_RESOURCE_DIMENSION dim;
		res->GetType(&dim);

		switch (dim) {
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			{

				auto texture = reinterpret_cast<ID3D11Texture2D*>(res);
				D3D11_TEXTURE2D_DESC desc;
				texture->GetDesc(&desc);
				if (width)
					*width = desc.Width; // width in pixels
				if (height)
					*height = desc.Height; // height in pixels

				break;

			}
			default:

				if (width)
					*width = 0; // width in pixels
				if (height)
					*height = 0; // height in pixels


		}
	}

};

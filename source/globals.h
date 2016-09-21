#pragma once


namespace Globals {

	extern int WINDOW_WIDTH;	// in pixels
	extern int WINDOW_HEIGHT;	// in pixels


	/**	0 = Present as fast as possible.
		1 = Lock to screen refresh rate. */
	extern int vsync_enabled;
	extern bool FULL_SCREEN;

	/* NOTE: using MessageBox forces a reboot when in full screen mode!
		Only use this before graphics assets have been loaded and before going
		full screen. After graphics assets have been loaded use GameEngine::reportError(). */
	/*inline bool reportError(HRESULT hr) {

		if (FAILED(hr)) {
			_com_error err(hr);
			MessageBox(NULL, err.ErrorMessage(), TEXT("This is SRS Error"), MB_OK | MB_ICONERROR);
			return true;
		}

		return false;
	}*/

	/* For reference purposes only. */
	/*wstring convertIntToWString(float number) {

		wostringstream ws;
		ws << "The number: " << number;

		return ws.str();
	}*/


	/*wostringstream ws;
	ws << "some writing here" << someVariable;
	OutputDebugString(ws.str().c_str());*/
};

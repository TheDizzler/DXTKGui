#pragma once


/* Global variables and includes */
#include <comdef.h>


namespace Globals {

	static int WINDOW_WIDTH = 800;	// in pixels
	static int WINDOW_HEIGHT = 600;	// in pixels

	/**	0 = Present as fast as possible.
		1 = Lock to screen refresh rate. */
	static int vsync_enabled = 1;
	static bool FULL_SCREEN = false;


	inline bool reportError(HRESULT hr) {

		if (FAILED(hr)) {
			_com_error err(hr);
			MessageBox(NULL, err.ErrorMessage(), TEXT("This is SRS Error"), MB_OK | MB_ICONERROR);
			return true;
		}

		return false;
	}

	/* For reference purposes only. */
	/*wstring convertIntToWString(float number) {

		wostringstream ws;
		ws << "The number: " << number;

		return ws.str();
	}*/
};

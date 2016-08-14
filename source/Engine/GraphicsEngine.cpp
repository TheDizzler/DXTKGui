#include "GraphicsEngine.h"

#include <DirectXColors.h>


GraphicsEngine::GraphicsEngine() {

}


GraphicsEngine::~GraphicsEngine() {

	//delete[] displayModeList;

	//if (swapChain)
	swapChain->SetFullscreenState(false, NULL);
}



bool GraphicsEngine::initD3D(HWND hwnd) {

	if (!getDisplayAdapters()) {
		MessageBox(NULL, L"Error gathering display info", L"ERROR", MB_OK);
		return false;
	}


	if (!initializeAdapter(hwnd, selectedAdapterIndex)) {
		MessageBox(NULL, L"Error initializing Adapter", L"ERROR", MB_OK);
		return false;
	}


	if (!initializeRenderTarget())
		return false;

	initializeViewport();

	return true;

}


bool GraphicsEngine::getDisplayAdapters() {

	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIOutput> adapterOutput;

	// Create a DirectX graphics interface factory.
	if (Globals::reportError(CreateDXGIFactory(__uuidof(IDXGIFactory2),
		(void**) factory.GetAddressOf()))) {
		MessageBox(NULL, L"Cannot create DXGI factory.", L"ERROR", MB_OK);
		return false;
	}


	int i = 0;
	// get all adapters (gfx cards)
	while (factory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND) {
		adapters.push_back(adapter);
		/*DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adapter->GetDesc(&desc);*/
		//MessageBox(0, desc.Description, L"Adapter detected", MB_OK);
		/* send adapter info to ConfigScreen */
		//SendMessage(adapterCombo, CB_ADDSTRING, 0, (LPARAM) desc.Description);

	}


	int size = i - 1;
	i = 0;
	adapter = adapters[0];

	for (int j = 0; j < size; ++j) {
		while (adapter->EnumOutputs(i++, &adapterOutput) != DXGI_ERROR_NOT_FOUND) {

			adapterOutputs.push_back(adapterOutput);

			/*DXGI_OUTPUT_DESC desc;
			ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));
			adapterOutput->GetDesc(&desc);*/
			//MessageBox(0, desc.DeviceName, L"Device detected", MB_OK);
			/* send adapter output info to ConfigScreen */
			//SendMessage(outputCombo, CB_ADDSTRING, 0, (LPARAM) desc.DeviceName);
		}
		i = 0;
		adapter = adapters[j];
	}

	// set defaults
	adapter = adapters[selectedAdapterIndex];
	selectedOutput = adapterOutputs[0];

	return true;
}


bool GraphicsEngine::initializeAdapter(HWND hwnd, int adapterIndex) {

	selectedAdapterIndex = adapterIndex;

	/*unsigned int numerator = 0, denominator = 1;*/
	DXGI_RATIONAL refreshRate = {0, 1};
	selectedAdapter = adapters[adapterIndex].Get();

	if (!getDisplayModeList(selectedOutput.Get()))
		return false;

	/*if (selectedDisplayMode.Width == (unsigned int) Globals::WINDOW_WIDTH
		&& selectedDisplayMode.Height == (unsigned int) Globals::WINDOW_HEIGHT) {

		refreshRate = selectedDisplayMode.RefreshRate;
	}*/

	/** **** Create SWAP CHAIN **** **/
	setDisplayMode(selectedDisplayModeIndex);

	//DXGI_MODE_DESC bufferDesc;

	//ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	//bufferDesc.Width = Globals::WINDOW_WIDTH;
	//bufferDesc.Height = Globals::WINDOW_HEIGHT;
	//// Set the refresh rate of the back buffer.
	//if (Globals::vsync_enabled == 1) {
	//	bufferDesc.RefreshRate = refreshRate;
	//} else {
	//	bufferDesc.RefreshRate.Numerator = 60;
	//	bufferDesc.RefreshRate.Denominator = 1;
	//}
	//bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1; // Back buffer count
	swapChainDesc.BufferDesc = selectedDisplayMode;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = !Globals::FULL_SCREEN;
	// Use Alt-Enter to switch between full screen and windowed mode.;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept.
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	if (Globals::reportError(D3D11CreateDeviceAndSwapChain(selectedAdapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, featureLevels,
		_countof(featureLevels), D3D11_SDK_VERSION, &swapChainDesc,
		swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel,
		deviceContext.GetAddressOf()))) {

		MessageBox(hwnd, L"Error creating Device and Swap Chain.", L"ERROR", MB_OK);
		return false;
	}

	ID3D11Texture2D* backBufferPtr;
	if (Globals::reportError(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*) &backBufferPtr))) {
		MessageBox(hwnd, L"Could not get pointer to back buffer.", L"ERROR", MB_OK);
		return false;
	}

	if (Globals::reportError(device->CreateRenderTargetView(backBufferPtr,
		NULL, &renderTargetView))) {
		MessageBox(hwnd, L"Could not create render target view.", L"ERROR", MB_OK);
		return false;
	}
	backBufferPtr->Release();



	return true;
}

bool GraphicsEngine::initializeRenderTarget() {

	/** **** Create our Render Target **** **/
	ID3D11Texture2D* backBufferTexture = 0;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**) &backBufferTexture)))

		return false;


	if (FAILED(device->CreateRenderTargetView(backBufferTexture,
		nullptr, renderTargetView.GetAddressOf())))

		return false;

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	backBufferTexture->Release();

	return true;
}

void GraphicsEngine::initializeViewport() {

	/** **** Create Viewport **** **/
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(Globals::WINDOW_WIDTH);
	viewport.Height = static_cast<float>(Globals::WINDOW_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewport);

	batch.reset(new SpriteBatch(deviceContext.Get()));
}


bool GraphicsEngine::getDisplayModeList(ComPtr<IDXGIOutput> adapterOut) {

	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (Globals::reportError(adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		return false;
	}

	//displayModeList = new DXGI_MODE_DESC[numModes];
	displayModeList.clear();
	displayModeList.resize(numModes);
	if (Globals::reportError(adapterOut->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, &displayModeList[0]))) {
		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
		return false;
	}

	for (int i = 0; i < displayModeList.size(); ++i) {
		if (displayModeList[i].Height == Globals::WINDOW_HEIGHT
			&& displayModeList[i].Width == Globals::WINDOW_WIDTH) {
			selectedDisplayModeIndex = i;
			selectedDisplayMode = displayModeList[i];
			break;
		}
	}

	/*DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Globals::WINDOW_WIDTH;
	bufferDesc.Height = Globals::WINDOW_HEIGHT;*/
	// Set the refresh rate of the back buffer.
	/*if (Globals::vsync_enabled == 1) {
		bufferDesc.RefreshRate = refreshRate;
	} else {
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
	}*/
	/*bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_MODE_DESC closestMatch;
	ZeroMemory(&closestMatch, sizeof(DXGI_MODE_DESC));

	if (Globals::reportError(
		adapterOut->FindClosestMatchingMode(
			&bufferDesc, &closestMatch, device.Get())))
		return false;

	for (int i = 0; i < displayModeList.size(); ++i) {

		if (displayModeList[i].Height == closestMatch.Height
			&& displayModeList[i].Width == closestMatch.Width) {
			displayModeIndex = i;
			break;
		}
	}*/

	//selectedDisplayMode = closestMatch;
	// getDisplayModeDescriptions

/** Send info to ConfigScreen. **/

	return true;
}

void GraphicsEngine::setDisplayMode(size_t selectedIndex) {

	lastDisplayModeIndex = selectedDisplayModeIndex;
		//ZeroMemory(&lastDisplayMode, sizeof(DXGI_MODE_DESC));
		//lastDisplayMode = selectedDisplayMode;
	//ZeroMemory(&selectedDisplayMode, sizeof(DXGI_MODE_DESC));
	selectedDisplayModeIndex = selectedIndex;
	selectedDisplayMode = displayModeList[selectedDisplayModeIndex];
	if (Globals::vsync_enabled != 1) {
		selectedDisplayMode.RefreshRate.Numerator = 60;
		selectedDisplayMode.RefreshRate.Denominator = 1;
	}

}


vector<wstring> GraphicsEngine::getDisplayModeDescriptions() {

	vector<wstring> displayModeDescriptions;

	UINT lastWidth = 0;
	UINT lastHeight = 0;
	int i = 0;
	for (i; i < numModes; i++) {

		UINT width = displayModeList[i].Width;
		UINT height = displayModeList[i].Height;

		if (width == lastWidth && height == lastHeight)
			continue;
		wostringstream mode;
		//mode << "Format: " << displayModeList[i].Format;
		mode << width
			<< " x " << height;
		/*mode << "   Refresh Rate: "
			<< displayModeList[i].RefreshRate.Numerator
			/ displayModeList[i].RefreshRate.Denominator;*/

		lastWidth = width;
		lastHeight = height;

		displayModeDescriptions.push_back(mode.str());
		/** Send info to ConfigScreen. **/
		//SendMessage(displayModesCombo, CB_ADDSTRING, 0, (LPARAM) mode.str().c_str());
	}

	return displayModeDescriptions;
}

vector<wstring> GraphicsEngine::getAdapterListDescriptions() {

	vector<wstring> list;
	for each (ComPtr<IDXGIAdapter> adap in adapters) {

		DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adap->GetDesc(&desc);
		list.push_back(desc.Description);

	}
	return list;
}

vector<ComPtr<IDXGIAdapter>> GraphicsEngine::getAdapterList() {
	return adapters;
}



vector<wstring> GraphicsEngine::getDisplayModeListDescriptions(size_t adapterIndex) {


	ComPtr<IDXGIOutput> adapterOut = adapterOutputs[adapterIndex];
	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (Globals::reportError(adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		vector<wstring> list;
		list.push_back(L"ERROR enumerating display modes");
		return list;
	}

	//displayModeList = new DXGI_MODE_DESC[numModes];
	//vector<DXGI_MODE_DESC> displayModeList;
	displayModeList.clear();
	displayModeList.resize(numModes);
	if (Globals::reportError(adapterOut->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, &displayModeList[0]))) {

		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
		vector<wstring> list;
		list.push_back(L"ERROR getting display mode list");
		return list;
	}
	return getDisplayModeDescriptions();
}

/* Deprecate? */
vector<DXGI_MODE_DESC> GraphicsEngine::getDisplayModeList(size_t adapterIndex) {

	UINT totalModes;
	ComPtr<IDXGIOutput> adapterOut = adapterOutputs[adapterIndex];
	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (Globals::reportError(
		adapterOut->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
			&totalModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		return vector<DXGI_MODE_DESC>();
	}

	vector<DXGI_MODE_DESC> modeList;
	modeList.resize(numModes);
	if (Globals::reportError(
		adapterOut->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
			&totalModes, &modeList[0]))) {

		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
		return vector<DXGI_MODE_DESC>();
	}

	return modeList;
}


vector<wstring> GraphicsEngine::getAdapterOutputList() {

	vector<wstring> list;
	for each (ComPtr<IDXGIOutput> adap in adapterOutputs) {

		DXGI_OUTPUT_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));
		adap->GetDesc(&desc);
		list.push_back(desc.DeviceName);

	}
	return list;
}

size_t GraphicsEngine::getSelectedAdapterIndex() {
	return selectedAdapterIndex;
}

size_t GraphicsEngine::getSelectedDisplayModeIndex() {
	return selectedDisplayModeIndex;
}

#include "GraphicsEngine.h"

#include <DirectXColors.h>


GraphicsEngine::GraphicsEngine() {

}


GraphicsEngine::~GraphicsEngine() {
	adapters.clear();
	displays.clear();
	displayModeList.clear();
	if (swapChain.Get() != NULL)
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



	// create SpriteBatch
	batch.reset(new SpriteBatch(deviceContext.Get()));

	return true;

}


bool GraphicsEngine::getDisplayAdapters() {


	ComPtr<IDXGIFactory1> factory;
	// Create a DirectX graphics interface factory.
	if (Globals::reportError(CreateDXGIFactory1(__uuidof(IDXGIFactory),
		(void**) factory.GetAddressOf()))) {
		MessageBox(NULL, L"Cannot create DXGI factory.", L"ERROR", MB_OK);
		return false;
	}

	ComPtr<IDXGIAdapter1> adapter;
	int i = 0;
	// get all adapters (gfx cards)
	while (factory->EnumAdapters1(i++, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		adapters.push_back(adapter);
		/*DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adapter->GetDesc(&desc);
		MessageBox(0, desc.Description, L"Adapter detected", MB_OK);*/

	}


	int size = i - 1;

	for (int j = 0; j < size; ++j) {
		adapter = adapters[j];
		i = 0;

		DXGI_ADAPTER_DESC1 adapterDesc;
		ZeroMemory(&adapterDesc, sizeof(DXGI_ADAPTER_DESC1));
		adapter->GetDesc1(&adapterDesc);
		ComPtr<IDXGIOutput> adapterOutput;

		while (adapter->EnumOutputs(i++, adapterOutput.GetAddressOf())
			!= DXGI_ERROR_NOT_FOUND) {

			displays.push_back(adapterOutput);

			DXGI_OUTPUT_DESC desc;
			ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));
			adapterOutput->GetDesc(&desc);

			wostringstream ws;
			ws << "Adapter: " << adapterDesc.Description << "\n";
			ws << "Display: " << desc.DeviceName << "\n";
			ws << "AttachedToDesktop: " << desc.AttachedToDesktop << "\n";
			//ws << "Monitor: " << desc.Monitor << "\n" << "\n";
			OutputDebugString(ws.str().c_str());
			//MessageBox(0, desc.DeviceName, L"Device detected", MB_OK);
		}


	}

	// set defaults
	adapter = adapters[selectedAdapterIndex];
	selectedDisplay = displays[selectedDisplayIndex];

	return true;
}


bool GraphicsEngine::initializeAdapter(HWND hwnd, int adapterIndex) {

	selectedAdapterIndex = adapterIndex;

	/*unsigned int numerator = 0, denominator = 1;*/
	//DXGI_RATIONAL refreshRate = {0, 1};

	selectedAdapter = adapters[selectedAdapterIndex];

	if (!populateDisplayModeList(selectedDisplay))
		return false;

	/** **** Create SWAP CHAIN **** **/
	setDisplayMode(selectedDisplayModeIndex);


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

	//featureLevel

	return true;
}

bool GraphicsEngine::initializeRenderTarget() {

	///** **** Create our Render Target **** **/
	ID3D11Texture2D* backBufferPtr;
	if (Globals::reportError(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*) &backBufferPtr))) {
		MessageBox(0, L"Could not get pointer to back buffer.", L"ERROR", MB_OK);
		return false;
	}

	if (Globals::reportError(device->CreateRenderTargetView(backBufferPtr,
		NULL, renderTargetView.GetAddressOf()))) {
		MessageBox(0, L"Could not create render target view.", L"ERROR", MB_OK);
		return false;
	}

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	backBufferPtr->Release();
	return true;
}

void GraphicsEngine::initializeViewport() {

	/** **** Create Viewport **** **/
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Globals::WINDOW_WIDTH;
	viewport.Height = Globals::WINDOW_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewport);

}


bool GraphicsEngine::populateDisplayModeList(ComPtr<IDXGIOutput> display) {

	UINT numModes = 0;
	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (Globals::reportError(display->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		return false;
	}

	displayModeList.clear();
	displayModeList.resize(numModes);
	if (Globals::reportError(display->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, &displayModeList[0]))) {
		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
		return false;
	}

	for (int i = 0; i < displayModeList.size(); ++i) {
		if (displayModeList[i].Height == Globals::WINDOW_HEIGHT
			&& displayModeList[i].Width == Globals::WINDOW_WIDTH) {

			selectedDisplayModeIndex = i;
			selectedDisplayMode = displayModeList[selectedDisplayModeIndex];

			break;
		}
	}

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


vector<ComPtr<IDXGIAdapter1>> GraphicsEngine::getAdapterList() {
	return adapters;
}

vector<ComPtr<IDXGIOutput>> GraphicsEngine::getDisplayListFor(size_t adapterIndex) {
	return getDisplayListFor(adapters[adapterIndex]);
}

vector<ComPtr<IDXGIOutput>> GraphicsEngine::getDisplayListFor(
	ComPtr<IDXGIAdapter1> adapter) {

	vector<ComPtr<IDXGIOutput>> outputs;
	ComPtr<IDXGIOutput> adapterOutput;
	int i = 0;

	while (adapter->EnumOutputs(i++, &adapterOutput) != DXGI_ERROR_NOT_FOUND) {

		outputs.push_back(adapterOutput);
	}

	return outputs;
}


vector<DXGI_MODE_DESC> GraphicsEngine::getDisplayModeList(size_t displayIndex) {
	return getDisplayModeList(displays[displayIndex]);
}


vector<DXGI_MODE_DESC> GraphicsEngine::getDisplayModeList(
	ComPtr<IDXGIOutput> display) {


	UINT totalModes;
	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (Globals::reportError(
		display->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
			&totalModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		return vector<DXGI_MODE_DESC>();
	}

	vector<DXGI_MODE_DESC> modeList;
	modeList.resize(totalModes);

	if (Globals::reportError(
		display->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
			&totalModes, &modeList[0]))) {

		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
		return vector<DXGI_MODE_DESC>();
	}

	return modeList;

}


size_t GraphicsEngine::getSelectedAdapterIndex() {
	return selectedAdapterIndex;
}

size_t GraphicsEngine::getSelectedDisplayIndex() {
	return selectedDisplayIndex;
}

size_t GraphicsEngine::getSelectedDisplayModeIndex() {
	return selectedDisplayModeIndex;
}


//vector<wstring> GraphicsEngine::getDisplayModeDescriptions() {
//
//	vector<wstring> displayModeDescriptions;
//
//	UINT lastWidth = 0;
//	UINT lastHeight = 0;
//	int i = 0;
//	for (i; i < numModes; i++) {
//
//		UINT width = displayModeList[i].Width;
//		UINT height = displayModeList[i].Height;
//
//		if (width == lastWidth && height == lastHeight)
//			continue;
//		wostringstream mode;
//		//mode << "Format: " << displayModeList[i].Format;
//		mode << width
//			<< " x " << height;
//		/*mode << "   Refresh Rate: "
//		<< displayModeList[i].RefreshRate.Numerator
//		/ displayModeList[i].RefreshRate.Denominator;*/
//
//		lastWidth = width;
//		lastHeight = height;
//
//		displayModeDescriptions.push_back(mode.str());
//		/** Send info to ConfigScreen. **/
//		//SendMessage(displayModesCombo, CB_ADDSTRING, 0, (LPARAM) mode.str().c_str());
//	}
//
//	return displayModeDescriptions;
//}
//
//
//vector<wstring> GraphicsEngine::getDisplayList() {
//
//	vector<wstring> list;
//	for each (ComPtr<IDXGIOutput> adap in displays) {
//
//		DXGI_OUTPUT_DESC desc;
//		ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));
//		adap->GetDesc(&desc);
//		list.push_back(desc.DeviceName);
//
//	}
//	return list;
//}

//vector<wstring> GraphicsEngine::getAdapterListDescriptions() {
//
//	vector<wstring> list;
//	for each (ComPtr<IDXGIAdapter> adap in adapters) {
//
//		DXGI_ADAPTER_DESC desc;
//		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
//		adap->GetDesc(&desc);
//		list.push_back(desc.Description);
//
//	}
//	return list;
//}
//
//vector<wstring> GraphicsEngine::getDisplayModeListDescriptions(size_t adapterOutputIndex) {
//
//	ComPtr<IDXGIOutput> display = displays[selectedDisplayIndex];
//	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
//	if (Globals::reportError(
//		display->GetDisplayModeList(
//			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) {
//		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
//		vector<wstring> list;
//		list.push_back(L"ERROR enumerating display modes");
//		return list;
//	}
//
//	//displayModeList = new DXGI_MODE_DESC[numModes];
//	//vector<DXGI_MODE_DESC> displayModeList;
//	displayModeList.clear();
//	displayModeList.resize(numModes);
//	if (Globals::reportError(display->GetDisplayModeList(
//		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
//		&numModes, &displayModeList[0]))) {
//
//		MessageBox(NULL, L"Error getting display mode list.", L"ERROR", MB_OK);
//		vector<wstring> list;
//		list.push_back(L"ERROR getting display mode list");
//		return list;
//	}
//	return getDisplayModeDescriptions();
//}
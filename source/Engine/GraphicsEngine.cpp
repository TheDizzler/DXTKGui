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


bool GraphicsEngine::initD3D(HWND h) {

	hwnd = h;
	if (!getDisplayAdapters()) {
		MessageBox(NULL, L"Error gathering display info", L"ERROR", MB_OK);
		return false;
	}


	if (!initializeAdapter(selectedAdapterIndex)) {
		MessageBox(NULL, L"Error initializing Adapter", L"ERROR", MB_OK);
		return false;
	}


	if (!initializeRenderTarget())
		return false;

	initializeViewport();



	// create SpriteBatch
	batch.reset(new SpriteBatch(deviceContext.Get()));
	//batch = new SpriteBatch(deviceContext.Get());
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

	ComPtr<IDXGIAdapter> adapter;
	int i = 0;
	// get all adapters (gfx cards)
	while (factory->EnumAdapters(i++, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		adapters.push_back(adapter);
		/*DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adapter->GetDesc(&desc);
		MessageBox(0, desc.Description, L"Adapter detected", MB_OK);*/

	}

	deviceContexts.resize(adapters.size());
	swapChains.resize(adapters.size());
	devices.resize(adapters.size());
	renderTargetViews.resize(adapters.size());
	//batches.resize(adapters.size());

	int size = i - 1;

	for (int j = 0; j < size; ++j) {
		adapter = adapters[j];
		i = 0;

		DXGI_ADAPTER_DESC adapterDesc;
		ZeroMemory(&adapterDesc, sizeof(DXGI_ADAPTER_DESC));
		adapter->GetDesc(&adapterDesc);
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
			//ws << "Monitor: " << desc.Monitor << "\n";
			ws << "\n";
			OutputDebugString(ws.str().c_str());
		}


	}

	// set defaults
	selectedDisplay = displays[selectedDisplayIndex];

	return true;
}

bool GraphicsEngine::initializeAdapter(int adapterIndex) {

	selectedAdapterIndex = adapterIndex;
	selectedAdapter = adapters[selectedAdapterIndex];
	device = devices[selectedAdapterIndex];
	deviceContext = deviceContexts[selectedAdapterIndex];
	swapChain = swapChains[selectedAdapterIndex];
	renderTargetView = renderTargetViews[selectedAdapterIndex];
	//batch = batches[selectedAdapterIndex].get();

	if (!populateDisplayModeList(selectedDisplay))
		return false;

	/** **** Create SWAP CHAIN **** **/
	setDisplayMode(selectedDisplayModeIndex);


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = bufferCount; // Back buffer count
	swapChainDesc.BufferDesc = selectedDisplayMode;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 4; // how many multisamples
										// 1 = turn multisampling off.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = !Globals::FULL_SCREEN;
	swapChainDesc.Flags = swapChainFlags;

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

	if (Globals::reportError(
		D3D11CreateDeviceAndSwapChain(selectedAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, featureLevels,
			_countof(featureLevels), D3D11_SDK_VERSION, &swapChainDesc,
			swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel,
			deviceContext.GetAddressOf()))) {

		MessageBox(hwnd, L"Error creating Device and Swap Chain.",
			L"ERROR", MB_OK);
		return false;
	}

	verifyAdapter(device);

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
			wostringstream ws;
			ws << "Format: " << selectedDisplayMode.Format << "\n";
			ws << "Width: " << selectedDisplayMode.Width << "\n";
			ws << "Height: " << selectedDisplayMode.Height << "\n";
			ws << "RefreshRate: " << selectedDisplayMode.RefreshRate.Numerator << "/"
				<< selectedDisplayMode.RefreshRate.Denominator << "\n";
			ws << "Scaling: " << selectedDisplayMode.Scaling << "\n";
			OutputDebugString(ws.str().c_str());
			break;
		}
	}

	return true;
}

void GraphicsEngine::setDisplayMode(size_t selectedIndex) {

	lastDisplayModeIndex = selectedDisplayModeIndex;
	selectedDisplayModeIndex = selectedIndex;
	selectedDisplayMode = displayModeList[selectedDisplayModeIndex];
	if (Globals::vsync_enabled != 1) {
		selectedDisplayMode.RefreshRate.Numerator = 60;
		selectedDisplayMode.RefreshRate.Denominator = 1;
	}

}

vector<ComPtr<IDXGIAdapter>> GraphicsEngine::getAdapterList() {
	return adapters;
}

vector<ComPtr<IDXGIOutput>> GraphicsEngine::getDisplayList() {
	return displays;
}

vector<ComPtr<IDXGIOutput>> GraphicsEngine::getDisplayListFor(size_t adapterIndex) {
	return getDisplayListFor(adapters[adapterIndex]);
}

vector<ComPtr<IDXGIOutput>> GraphicsEngine::getDisplayListFor(
	ComPtr<IDXGIAdapter> adapter) {

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


bool GraphicsEngine::setAdapter(size_t newAdapterIndex) {

	swapChain->SetFullscreenState(false, NULL);
	//renderTargetView.Get()->Release();
	//deviceContext.Get()->Release();
	//device.Get()->Release();
	

	if (!initializeAdapter(newAdapterIndex)) {
		MessageBox(NULL, L"Error initializing Adapter", L"ERROR", MB_OK);
		return false;
	}

	if (!initializeRenderTarget())
		return false;

	initializeViewport();

	// re-create SpriteBatch
	batch.reset(new SpriteBatch(deviceContext.Get()));
	//batch = new SpriteBatch(deviceContext.Get());

	/*ComPtr<ID3D11Device> deviceCheck;
	deviceContext->GetDevice(&deviceCheck);
	verifyAdapter(deviceCheck);*/

	return true;
}

bool GraphicsEngine::changeDisplayMode(size_t newDisplayModeIndex) {

	selectedDisplayModeIndex = newDisplayModeIndex;
	//DXGI_MODE_DESC newDisplayMode = displayModeList[selectedDisplayModeIndex];

	Globals::WINDOW_WIDTH = displayModeList[selectedDisplayModeIndex].Width;
	Globals::WINDOW_HEIGHT = displayModeList[selectedDisplayModeIndex].Height;

	if (!resizeSwapChain())
		return false;


	return true;
}

bool GraphicsEngine::stopFullScreen() {

	if (swapChain.Get() == NULL)
		return false;

	swapChain->SetFullscreenState(false, NULL);
}

bool GraphicsEngine::setFullScreen(bool isFullScreen) {

	if (swapChain.Get() == NULL)
		return false;

	Globals::FULL_SCREEN = isFullScreen;

	if (isFullScreen) {
		swapChain->SetFullscreenState(true, NULL);
	} else {
		swapChain->SetFullscreenState(false, NULL);
	}
	if (!resizeSwapChain())
		return false;

	return true;
}


bool GraphicsEngine::resizeSwapChain() {

	// release all references to back buffers
	renderTargetView.Get()->Release();

	// resize target
	if (Globals::reportError(
		swapChain->ResizeTarget(&displayModeList[selectedDisplayModeIndex]))) {
		MessageBox(0, L"Failed to resize swapchain target",
			L"Display Mode Change Error", MB_OK);
		return false;
	}

	//resize backbuffers
	if (Globals::reportError(swapChain->ResizeBuffers(bufferCount,
		displayModeList[selectedDisplayModeIndex].Width,
		displayModeList[selectedDisplayModeIndex].Height,
		displayModeList[selectedDisplayModeIndex].Format, swapChainFlags))) {

		MessageBox(0, L"Failed to resize swapchain buffers",
			L"Display Mode Change Error", MB_OK);
		return false;
	}

	// destroy and recreate depth/stencil buffer if used (get width&height from backbuffer!)

	// re-construct rendertarget
	initializeRenderTarget();
	// re-construct the viewport
	initializeViewport();

	return true;
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


/** A debug function to make sure we're using the correct graphics adapter.
		Also because for fun.*/
bool GraphicsEngine::verifyAdapter(ComPtr<ID3D11Device> deviceCheck) {

	IDXGIDevice* dxgiDev;
	if (Globals::reportError(
		deviceCheck.Get()->QueryInterface(
			__uuidof(IDXGIDevice), (void **) &dxgiDev))) {

		MessageBox(0, L"Error querying device interface.",
			L"ERROR", MB_OK);
		return false;
	}
	IDXGIAdapter* dxgiAdapter;
	if (Globals::reportError(dxgiDev->GetAdapter(&dxgiAdapter))) {
		MessageBox(0, L"Error getting idxgi adapter from dxgi device.",
			L"ERROR", MB_OK);
		return false;
	}

	IDXGIFactory* factory;
	if (Globals::reportError(
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**) &factory))) {
		MessageBox(0, L"Error getting factory from idxgi adapter.",
			L"ERROR", MB_OK);
		return false;

	}

	DXGI_ADAPTER_DESC adapterDesc;
	ZeroMemory(&adapterDesc, sizeof(DXGI_ADAPTER_DESC));
	dxgiAdapter->GetDesc(&adapterDesc);
	wostringstream ws;
	ws << "Adapter: " << adapterDesc.Description << "\n";
	OutputDebugString(ws.str().c_str());

	dxgiDev->Release();
	dxgiAdapter->Release();
	factory->Release();
}


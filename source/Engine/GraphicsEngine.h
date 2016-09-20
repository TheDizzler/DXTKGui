#include "../pch.h"
#pragma once
#define MyMessageBox(a, b, c, d) \
	if (swapChain.Get() != NULL) \
		swapChain->SetFullscreenState(false, NULL); \
	MessageBox(a, b, c, d);
// ^ doesn't work as intended :/
#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "DXGI.lib")

#include <d3d11_1.h>
#include <dxgi1_2.h>

#include "SpriteBatch.h"

using namespace std;
using namespace DirectX;



class GraphicsEngine {
public:
	GraphicsEngine();
	virtual ~GraphicsEngine();

	bool initD3D(HWND hwnd);


	virtual void render(double time) = 0;


	vector<ComPtr<IDXGIAdapter> > getAdapterList();
	vector<ComPtr<IDXGIOutput> > getDisplayList();
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(size_t adapterIndex);
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(ComPtr<IDXGIAdapter> adapter);
	vector<DXGI_MODE_DESC> getDisplayModeList(size_t displayIndex);
	vector<DXGI_MODE_DESC> getDisplayModeList(ComPtr<IDXGIOutput> display);

	bool setAdapter(size_t adapterIndex);
	bool changeDisplayMode(size_t newDisplayModeIndex);
	bool setFullScreen(bool isFullScreen);
	/* Call when game loses focus. */
	bool stopFullScreen();

	size_t getSelectedAdapterIndex();
	size_t getSelectedDisplayIndex();
	size_t getSelectedDisplayModeIndex();
protected:
	HWND hwnd;
	unique_ptr<SpriteBatch> batch;
	//SpriteBatch* batch;
	/* Adapter currently being used. */
	ComPtr<IDXGIAdapter> selectedAdapter;
	/* Monitor being used. */
	ComPtr<IDXGIOutput> selectedDisplay;
	/* Display Mode being used. */
	DXGI_MODE_DESC selectedDisplayMode;
	size_t selectedAdapterIndex = 0;
	size_t selectedDisplayIndex = 0;
	size_t selectedDisplayModeIndex = 0;
	size_t lastDisplayModeIndex = 0;

	UINT bufferCount = 1;
	// Use Alt-Enter to switch between full screen and windowed mode.;
	UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//unsigned int numModes = 0;
	/* Changes backbuffer to front buffer. */
	ComPtr<IDXGISwapChain> swapChain;
	/* GPU object */
	ComPtr<ID3D11Device> device;
	/* GPU interface */
	ComPtr<ID3D11DeviceContext> deviceContext;
	/* The backbuffer that gets drawn to. */
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	//D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_VIEWPORT viewport;
	/* List of all gfx cards on this machine. */
	vector<ComPtr<IDXGIAdapter> > adapters;
	//vector<ComPtr<ID3D11DeviceContext> > deviceContexts;
	//vector<ComPtr<IDXGISwapChain> > swapChains; 
	//vector<ComPtr<ID3D11Device> > devices;
	//vector<ComPtr<ID3D11RenderTargetView> > renderTargetViews;
	//vector<unique_ptr<SpriteBatch> > batches;
	/* List of all monitors available with this adapter. */
	vector<ComPtr<IDXGIOutput> > displays;
	vector<DXGI_MODE_DESC> displayModeList; // all possible display modes with this monitor/video card 

	bool resizeSwapChain();

	bool getDisplayAdapters();
	bool initializeAdapter(int adapterIndex);
	bool initializeRenderTarget();
	void initializeViewport();
	bool populateDisplayModeList(ComPtr<IDXGIOutput> adapterOut);
	void setDisplayMode(size_t selectedIndex);

	/** A debug function to make sure we're using the correct graphics adapter. */
	bool verifyAdapter(ComPtr<ID3D11Device> deviceCheck);

};


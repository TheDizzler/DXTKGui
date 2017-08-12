#pragma once

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "DXGI.lib")

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <wrl.h>
#include <vector>

#include "SpriteBatch.h"
#include "CommonStates.h"

#include "../globals.h"
#include "Camera.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class GraphicsEngine {
public:
	GraphicsEngine();
	virtual ~GraphicsEngine();

	bool initD3D(HWND hwnd);

	virtual void reloadGraphicsAssets() = 0;
	virtual void render() = 0;

	vector<ComPtr<IDXGIAdapter> > getAdapterList();
	vector<ComPtr<IDXGIOutput> > getDisplayList();
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(size_t adapterIndex);
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(ComPtr<IDXGIAdapter> adapter);
	vector<DXGI_MODE_DESC> getDisplayModeList(size_t displayIndex);
	vector<DXGI_MODE_DESC> getDisplayModeList(ComPtr<IDXGIOutput> display);

	bool setAdapter(size_t adapterIndex);
	bool changeDisplayMode(size_t newDisplayModeIndex);
	bool setFullScreen(bool isFullScreen);
	void setViewport(int xPos, int yPos, int width, int height);
	/* Call when game loses focus. */
	bool stopFullScreen();

	size_t getSelectedAdapterIndex();
	size_t getSelectedDisplayIndex();
	size_t getSelectedDisplayModeIndex();

	ComPtr<ID3D11DeviceContext> getDeviceContext();
	/** Used for creating textures from an area on screen */
	ComPtr<IDXGISwapChain> getSwapChain();
	SpriteBatch* getSpriteBatch();

	shared_ptr<Camera> camera;
protected:
	HWND hwnd;
	unique_ptr<SpriteBatch> batch;

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
	//ComPtr<IDXGISwapChain> textureSwapChain;
	/* GPU object */
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11Debug> debugDevice;
	/* GPU interface */
	ComPtr<ID3D11DeviceContext> deviceContext;
	/* The backbuffer that gets drawn to. */
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	//D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_VIEWPORT d3dViewport;
	//Viewport viewport;

	/* List of all gfx cards on this machine. */
	vector<ComPtr<IDXGIAdapter> > adapters;
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


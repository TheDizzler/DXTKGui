#pragma once
#pragma comment (lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

#include <d3d11.h>
#include <dxgi1_2.h>


#include <wrl.h>
#include <Windows.h>
#include <memory>
#include <vector>
#include <sstream>

#include "SpriteBatch.h"
#include "../globals.h"


using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;


class GraphicsEngine {
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool initD3D(HWND hwnd);


	virtual void render(double time) = 0;


	vector<wstring> getDisplayModeDescriptions();
	vector<wstring> getAdapterListDescriptions();
	vector<ComPtr<IDXGIAdapter> > getAdapterList();
	vector<wstring> getDisplayModeListDescriptions(size_t adapterIndex);
	vector<DXGI_MODE_DESC> getDisplayModeList(size_t adapterIndex);
	vector<wstring> getAdapterOutputList();
	size_t getSelectedAdapterIndex();
	size_t getSelectedDisplayModeIndex();
protected:

	unique_ptr<SpriteBatch> batch;

	/* Adapter currently being used. */
	ComPtr<IDXGIAdapter> selectedAdapter;
	ComPtr<IDXGIOutput> selectedOutput;
	DXGI_MODE_DESC selectedDisplayMode;
	size_t selectedDisplayModeIndex = 0;
	size_t lastDisplayModeIndex = 0;
	size_t selectedAdapterIndex = 0;



	unsigned int numModes = 0;
	/* Changes backbuffer to front buffer. */
	ComPtr<IDXGISwapChain> swapChain;
	/* GPU object */
	ComPtr<ID3D11Device> device;
	/* GPU interface */
	ComPtr<ID3D11DeviceContext> deviceContext;
	/* The backbuffer that gets drawn to. */
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_VIEWPORT viewport;
	vector<ComPtr<IDXGIAdapter> > adapters;
	vector<ComPtr<IDXGIOutput> > adapterOutputs;
	vector<DXGI_MODE_DESC> displayModeList; // all possible display modes with this monitor/video card 
	


	bool getDisplayAdapters();
	bool initializeAdapter(HWND hwnd, int adapterIndex);
	bool initializeRenderTarget();
	void initializeViewport();
	bool getDisplayModeList(ComPtr<IDXGIOutput> adapterOut);
	void setDisplayMode(size_t selectedIndex);

};


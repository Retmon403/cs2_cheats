#include "dx11hk.h"
#include "gui.h"
#include "hacker.h"
#include "other.h"
#include "tool.h"
#include "hook.h"
#include "MyHook.h"
#include <windowsx.h>


DxHook dxhook;

ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
DWORD_PTR* pSwapChainVTable = nullptr;
DWORD_PTR* pDeviceContextVTable = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView;

HMODULE g_dllbase = nullptr;

WNDPROC oWndProc = nullptr;

#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	if (uMsg == WM_KEYUP) {
		if (wParam == VK_HOME) {
			gui.show_menu = !gui.show_menu;
			io.MouseDrawCursor = gui.show_menu;
			goto end;
		}
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}
	
end:
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

namespace Dx11 {
	HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
		ImGui_ImplDX11_InvalidateDeviceObjects();
		if (g_pRenderTargetView) {
			//MessageBoxA(0, 0, 0, 0);
			g_pRenderTargetView->Release();
			g_pRenderTargetView = nullptr;
		}
		HRESULT result = dxhook.o_ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		ImGui_ImplDX11_CreateDeviceObjects();
		return result;
	}

	ID3D11DepthStencilState* DepthStencilState_FALSE = NULL; //depth off
	ID3D11DepthStencilState* DepthStencilState_ORIG = NULL; //depth on

	HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags) {
		if (!gui.imgui) {
			return dxhook.o_Present(pSwapChain, SysInterval, Flags);
		}

		static bool firstcall = true;
		if (firstcall) {
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice))) {
				g_pDevice->GetImmediateContext(&g_pContext);
				ID3D11Texture2D* pBackBuffer;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
				pBackBuffer->Release();

				firstcall = false;

				printf("Loading Menu...\n");

				oWndProc = (WNDPROC)SetWindowLongPtr(game.Hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO(); (void)io;
				ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
				io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
				io.WantSaveIniSettings = false;
				ImFontConfig f_cfg;
				io.Fonts->AddFontFromMemoryTTF((void*)baidu_font_data, baidu_font_size, 16.f, &f_cfg, io.Fonts->GetGlyphRangesChineseFull());

				io.LogFilename = nullptr;
				io.IniFilename = nullptr;
				ImGui::GetIO().ImeWindowHandle = game.Hwnd;
				ImGui_ImplWin32_Init(game.Hwnd);
				ImGui_ImplDX11_Init(g_pDevice, g_pContext);

				// Create depthstencil state
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				depthStencilDesc.DepthEnable = TRUE;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.StencilEnable = FALSE;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				// Stencil operations if pixel is front-facing
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				// Stencil operations if pixel is back-facing
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				g_pDevice->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState_FALSE);


			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		GUI();
		ImGui::EndFrame();
		ImGui::Render();
		g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


		return dxhook.o_Present(pSwapChain, SysInterval, Flags);
	}

	ID3D11Buffer* veBuffer;
	UINT Stride;
	UINT veBufferOffset;
	D3D11_BUFFER_DESC vedesc;

	//index
	ID3D11Buffer* inBuffer;
	DXGI_FORMAT inFormat;
	UINT        inOffset;
	D3D11_BUFFER_DESC indesc;


	//pssetshaderresources
	UINT pssrStartSlot;
	ID3D11Resource* Resource;
	D3D11_SHADER_RESOURCE_VIEW_DESC Descr;
	D3D11_TEXTURE2D_DESC texdesc;

	void hkDrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT  BaseVertexLocation) {

		//get stride & vedesc.ByteWidth
		pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
		if (veBuffer != NULL)
			veBuffer->GetDesc(&vedesc);
		if (veBuffer != NULL) { veBuffer->Release(); veBuffer = NULL; }

		//get indesc.ByteWidth (comment out if not used)
		pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
		if (inBuffer != NULL)
			inBuffer->GetDesc(&indesc);
		if (inBuffer != NULL) { inBuffer->Release(); inBuffer = NULL; }

		return dxhook.o_DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	void hkDrawIndexedInstanced(ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) {

		//get stride & vedesc.ByteWidth
		pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
		if (veBuffer != NULL)
			veBuffer->GetDesc(&vedesc);
		if (veBuffer != NULL) { veBuffer->Release(); veBuffer = NULL; }

		//get indesc.ByteWidth (comment out if not used)
		pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
		if (inBuffer != NULL)
			inBuffer->GetDesc(&indesc);
		if (inBuffer != NULL) { inBuffer->Release(); inBuffer = NULL; }

		//这是cs2中角色的衣服的模型id
		if (Stride == 32 || IndexCountPerInstance == 3312 || indesc.ByteWidth == 6624 || vedesc.ByteWidth == 19904) {
			pContext->OMGetDepthStencilState(&DepthStencilState_ORIG, 0); //get original
			pContext->OMSetDepthStencilState(DepthStencilState_FALSE, 0); //depthstencil off
			dxhook.o_DrawIndexedInstanced(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation); //redraw
			pContext->OMSetDepthStencilState(DepthStencilState_ORIG, 0); //depthstencil on
			SAFE_RELEASE(DepthStencilState_ORIG); //release
		}

		return dxhook.o_DrawIndexedInstanced(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);

	}


}


void SetD3dEsp(bool hook) {
	//通过hook dx11的函数 来过滤模型禁用z轴缓冲 实现人物透视 代码是抄的国外一个大佬的开源项目 
	//https://github.com/DrNseven/D3D11-Wallhack
	if (hook) {
		minihook.SetHook(dxhook.DrawIndexed, Dx11::hkDrawIndexed, (LPVOID*)&dxhook.o_DrawIndexed);
		minihook.SetHook(dxhook.DrawIndexedInstanced, Dx11::hkDrawIndexedInstanced, (LPVOID*)&dxhook.o_DrawIndexedInstanced);
	}
	else {
		minihook.UnHook(dxhook.DrawIndexed);
		minihook.UnHook(dxhook.DrawIndexedInstanced);
	}
}

bool SetDxHook(bool hook) {
	//设置dx11的hook
	if (hook) {
		dxhook.Present = (_Present)getjump_addr(getjump_addr((PVOID)dxhook.Present));
		printf("gameoverlayrenderer64 Present:%p\n", dxhook.Present);

		dxhook.ResizeBuffers = (_ResizeBuffers)getjump_addr(getjump_addr((PVOID)dxhook.ResizeBuffers));
		printf("gameoverlayrenderer64 ResizeBuffers:%p\n", dxhook.Present);
		if (!dxhook.ResizeBuffers || !dxhook.Present) {
			printf("oResizeBuffers || oPresent failed...\n");
			return false;
		}
		minihook.SetHook(dxhook.Present, Dx11::hkPresent, (LPVOID*)&dxhook.o_Present);
	}
	else {
		if (gui.dx_hk) {
			SetD3dEsp(false);
		}
	}
	return true;
}

void InitImGui() {
	//初始化imgui 以及拿到dx虚表里的函数
	game.Hwnd = GetHwndByPid(GetCurrentProcessId());
	if (!game.Hwnd) {
		printf("find game window failed!\n");
		return;
	}
	D3D_FEATURE_LEVEL obtainedLevel;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = game.Hwnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		feature_levels,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pDevice,
		&obtainedLevel,
		&g_pContext) != S_OK)
	{
		printf("create device failed!\n");
		return;
	}
	pSwapChainVTable = (DWORD_PTR*)g_pSwapChain;
	pSwapChainVTable = (DWORD_PTR*)pSwapChainVTable[0];
	printf("pSwapChainVTable:%p\n", pSwapChainVTable);
	dxhook.Present = (_Present)pSwapChainVTable[8];
	dxhook.ResizeBuffers = (_ResizeBuffers)pSwapChainVTable[13];

	pDeviceContextVTable = (DWORD_PTR*)g_pContext;
	pDeviceContextVTable = (DWORD_PTR*)pDeviceContextVTable[0];


	dxhook.DrawIndexed = (_DrawIndexed)pDeviceContextVTable[12];
	dxhook.DrawIndexedInstanced = (_DrawIndexedInstanced)pDeviceContextVTable[20];
	printf("oPresent:%p oResizeBuffers:%p\n", dxhook.Present, dxhook.ResizeBuffers);
	printf("oDrawIndexed:%p oDrawIndexedInstanced:%p\n", dxhook.DrawIndexed, dxhook.DrawIndexedInstanced);
	g_pSwapChain->Release();
	g_pDevice->Release();
	g_pContext->Release();
}

DWORD MainThread(LPVOID lp) {
	initgui();//init gui config...
	SetConsoleStatus(gui.show_debug);
	printf("ModuleBase:%p\n", g_dllbase);

	//等待client被装载到进程
	while (!game.InitGameBase()) {
		Sleep(100);
	}
	
	printf("Init...\n");
	if (!interfaces.cinput.Init()) {
		printf("PatternScan failed...\n");
		return 0;
	}

	InitImGui();
	if (!SetDxHook(true)) {
		//hook dx11
		printf("SetHook failed...\n");
		return 0;
	}
	
	//开两线程
	NewThread(AutoJumpThread);
	NewThread(AutoFireThread);
	

	return 0;
}

void FreeMenu() {
	g_jump_thread_status = STATUS_STOP;
	g_autofire_thread_status = STATUS_STOP;
	gui.imgui = false;
	while (!CheckAllThreadIsExit()) {
		Sleep(100);
	}
	Sleep(1000);
	SetDxHook(false);
	SetWindowLongPtr(game.Hwnd, GWLP_WNDPROC, (ULONG_PTR)oWndProc);
	SetConsoleStatus(false);
	minihook.FreeHook();
	MessageBeep(MB_ICONINFORMATION);
	FreeLibraryAndExitThread(g_dllbase, 0);
}
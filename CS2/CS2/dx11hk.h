#pragma once
#include "define.h"
#include "gui.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(WINAPI* _Present)(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags);
typedef HRESULT(WINAPI* _ResizeBuffers) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef void (WINAPI* _DrawIndexed)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT  BaseVertexLocation);
typedef void(WINAPI* _DrawIndexedInstanced) (ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);

class DxHook
{
public:
	_Present Present;
	_Present o_Present;

	_ResizeBuffers ResizeBuffers;
	_ResizeBuffers o_ResizeBuffers;

	_DrawIndexed DrawIndexed;
	_DrawIndexed o_DrawIndexed;

	_DrawIndexedInstanced DrawIndexedInstanced;
	_DrawIndexedInstanced o_DrawIndexedInstanced;
private:

};

void SetD3dEsp(bool hook);
void FreeMenu();

DWORD MainThread(LPVOID lp);



extern HMODULE g_dllbase;


extern bool g_stop_mouse_key;

#include <Windows.h>
#include <gl/GL.h>
#include "wglext.h"
#include "glext.h"

#include <iostream>
#include <string>

HDC hdc = nullptr;

LRESULT CALLBACK WindowProc(
  _In_ HWND hwnd,
  _In_ UINT uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		hdc = GetDC(hwnd);
		int pfn = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pfn, &pfd);

		HGLRC context = wglCreateContext(hdc);
		wglMakeCurrent(hdc, context);

		auto wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
		auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(context);

		const int attribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			0 // End
		};

		int pixelFormat;
		UINT numFormats;

		wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		int gl_attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0 // End
		};

		context = wglCreateContextAttribsARB(hdc, NULL, gl_attribs);
		wglMakeCurrent(hdc, context);

		// MessageBoxA(NULL, (const char *)glGetString(GL_VERSION), "OpenGL Version", MB_OK | MB_ICONINFORMATION);

		glClearColor(0.13f, 0.13f, 0.15f, 1.0f);

		auto glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC) wglGetProcAddress("glCreateVertexArrays");
		auto glCreateBuffers = (PFNGLCREATEBUFFERSPROC) wglGetProcAddress("glCreateBuffers");
		auto glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC) wglGetProcAddress("glVertexArrayVertexBuffer");
		auto glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC) wglGetProcAddress("glVertexArrayAttribFormat");
		auto glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC) wglGetProcAddress("glVertexArrayAttribBinding");
		auto glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC) wglGetProcAddress("glEnableVertexArrayAttrib");
		auto glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) wglGetProcAddress("glBindVertexArray");
		auto glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC) wglGetProcAddress("glNamedBufferStorage");

		GLuint vao = 0;
		glCreateVertexArrays(1, &vao);
		
		GLuint vbo = 0;
		glCreateBuffers(1, &vbo);

		float data[] = 
		{
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.0f,  0.5f
		};

		glNamedBufferStorage(vbo, sizeof(data), data, GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 2);
		glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);
		glEnableVertexArrayAttrib(vao, 0);

		glBindVertexArray(vao);
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(wglGetCurrentContext());

		PostQuitMessage(0);
		break;

	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE, 
	_In_ PWSTR pCmdLine, 
	_In_ int nCmdShow)
{
	WNDCLASSEX wc = {};
	wc.hInstance = hInstance;
	wc.lpszClassName = L"OpenGL-Window";
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = &WindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);

	HWND window = CreateWindowEx(0, wc.lpszClassName, L"OpenGL", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		1280, 720, NULL, NULL, hInstance, NULL);

	ShowWindow(window, SW_NORMAL);
	UpdateWindow(window);

	MSG msg = {};

	glViewport(0, 0, 1280, 720);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		SwapBuffers(hdc);
	}

	return (int) msg.wParam;
}
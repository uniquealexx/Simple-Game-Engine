#include "Include.hpp"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t* className = L"DX11Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(className, L"Game", WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);

    auto renderer = std::make_unique<DX11Renderer>();
    if (!renderer->Initialize(hwnd, 800, 600)) {
        return 1;
    }

    // render here
    renderer->AddPrimitive(std::make_unique<Cube>());
    renderer->AddPrimitive(std::make_unique<Quad>());

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            renderer->RenderFrame();
        }
    }

    renderer->Shutdown();
    return 0;
}
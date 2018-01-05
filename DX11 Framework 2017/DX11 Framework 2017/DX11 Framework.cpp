#include "Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Application * theApp = new Application();
	
	if (FAILED(theApp->Initialise(hInstance, nCmdShow))) //macro to tell user if the application has failed to be created
	{
		return -1; //if returned less than 0, applcation failed
	}

    // Main message loop
    MSG msg = {0};

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) /// any error mesage Windows would send 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			theApp->Update();
            theApp->Draw();
        }
    }

	delete theApp;
	theApp = nullptr;

    return (int) msg.wParam;
}
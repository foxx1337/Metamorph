// Metamorph.cpp : Defines the entry point for the application.
//

#include "Metamorph.h"

#include "framework.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

class OutputStream final
{
protected:
    OutputStream(DWORD handle, FILE *stream) :
        nStdHandle(handle), cppStdStream(stream)
    {}
   
public:
    const DWORD nStdHandle;
    FILE *cppStdStream;

    /// <summary>
    /// Determines whether the stdlib stream already goes somewhere (a file) or
    /// it needs to be forcibly redirected (to a console buffer for example).
    /// </summary>
    /// <returns><c>true</c> if mapped to file, <c>false<c> if it's lost.</returns>
    bool IsMappedToFile() const
    {
        const int fd = _fileno(cppStdStream);
        const HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
        return hFile != INVALID_HANDLE_VALUE;
    }
    
    static const OutputStream StandardOutput;
    static const OutputStream StandardError;
};

const OutputStream OutputStream::StandardOutput = OutputStream(STD_OUTPUT_HANDLE, stdout);
const OutputStream OutputStream::StandardError = OutputStream(STD_ERROR_HANDLE, stderr);

/// <summary>
/// Attempts to redirect stdout or stderr to the <c>CONOUT$</c> standard Windows file.
/// </summary>
/// <param name="stream">Descriptor for the stream to redirect.</param>
/// <returns><c>true</c> on success.</returns>
bool CanRedirectConoutUnbuffered(const OutputStream &stream)
{
    HANDLE hExisting = GetStdHandle(stream.nStdHandle);
    if (hExisting != INVALID_HANDLE_VALUE)
    {
        if (!stream.IsMappedToFile())
        {
            // Running not redirected, with a lost output stream, redirect to CONOUT$.
            FILE *unused;
            freopen_s(&unused, "CONOUT$", "w", stream.cppStdStream);
            setvbuf(stream.cppStdStream, nullptr, _IONBF, 0);
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
/// Determines whether this process runs from a console process. If so, attaches
/// to the parent's console and redirects <c>stdout</c> and <c>stderr</c>.
/// </summary>
/// <returns><c>true</c> if running in a console and the stdout and the stderr
/// streams are available next.</returns>
bool CanAttachToConsole()
{
    return AttachConsole(ATTACH_PARENT_PROCESS)
        && CanRedirectConoutUnbuffered(OutputStream::StandardOutput)
        && CanRedirectConoutUnbuffered(OutputStream::StandardError);
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int                 RunAsConsole(int argc, wchar_t **argv);
int                 RunAsGui(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    if (CanAttachToConsole())
    {
        return RunAsConsole(__argc, __wargv);
    }

    return RunAsGui(hInstance, nCmdShow);
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_METAMORPH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_METAMORPH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            HFONT hFont = CreateFont(48, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                TEXT("Times New Roman"));
            SelectObject(hdc, hFont);
            RECT rect;
            GetClientRect(hWnd, &rect);
            DrawText(hdc, TEXT("A fully fledged Windows application."),
                -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            DeleteObject(hFont);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int RunAsGui(HINSTANCE hInstance, int nCmdShow)
{
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_METAMORPH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_METAMORPH));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

int RunAsConsole(int argc, wchar_t **wargv)
{
    printf("Console app with args:\n");
    for (int i = 0; i < argc; i++)
    {
        int ret = printf("\t%ls", wargv[i]);
        printf(" - %d wide chars.\n", ret);
    }
    fprintf(stderr, "This should go to standard error.\n");

    return 0;
}

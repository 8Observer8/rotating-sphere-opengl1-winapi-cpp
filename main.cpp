#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>
#include <windows.h>

#include <cmath>
#include <iostream>
#include <sstream>

float vert[] = { 1, 1, 0, 1, -1, 0, -1, -1, 0, -1, 1, 0 };
float xAlfa = 60;
float zAlfa = 0;
POINTFLOAT pos = { 0, -10 };
float oktahedron[] = { 0, 0, 2, -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0, -1, -1, 0 };
float okta[] = { 0, 0, 2, -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0, -1, -1, 0 };

float maxFPS = 60.f;
LARGE_INTEGER frequency;

float getSecondsPerFrame(LARGE_INTEGER startCounter, LARGE_INTEGER endCounter)
{
    return ((float)(endCounter.QuadPart - startCounter.QuadPart) /
            (float)frequency.QuadPart);
}

void DrawSphere(double r, int lats, int longs)
{
    int i, j;
    for (i = 0; i <= lats; i++)
    {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        glColor3f(0.5f, 0, 1);
        for (j = 0; j <= longs; j++)
        {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

void ShowWorld()
{
    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vert);
    for (int i = -5; i < 10; i++)
        for (int j = -5; j < 10; j++)
        {
            glPushMatrix();
            glColor4f(1, 1, 1, 0.5f);
            glTranslatef(i * 2, j * 2, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }

    glPushMatrix();
    glVertexPointer(3, GL_FLOAT, 0, &oktahedron);
    glColor4f(1, 0.6f, 1, 0.5f);
    glTranslatef(0, 0, 2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glPopMatrix();

    glPushMatrix();
    glVertexPointer(3, GL_FLOAT, 0, &okta);

    glColor4f(1, 0.5f, 1, 0.5f);
    glRotatef(180, 0, 1, 0);
    glTranslatef(0, 0, -2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void MoveCamera()
{
    if (GetKeyState(VK_UP) < 0)
        xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
    if (GetKeyState(VK_DOWN) < 0)
        xAlfa = --xAlfa < 0 ? 0 : xAlfa;
    if (GetKeyState(VK_LEFT) < 0)
        ++zAlfa;
    if (GetKeyState(VK_RIGHT) < 0)
        --zAlfa;

    float ugol = -zAlfa / 180 * M_PI;
    float speed = 0;
    if (GetKeyState('W') < 0)
        speed = 0.1;
    if (GetKeyState('S') < 0)
        speed = -0.1;
    if (GetKeyState('A') < 0)
    {
        speed = 0.1;
        ugol -= M_PI * 0.5;
    }
    if (GetKeyState('D') < 0)
    {
        speed = 0.1;
        ugol += M_PI * 0.5;
    }
    if (speed != 0)
    {
        pos.x += sin(ugol) * speed;
        pos.y += cos(ugol) * speed;
    }

    glRotatef(-xAlfa, 1, 0, 0);
    glRotatef(-zAlfa, 0, 0, 1);
    glTranslatef(-pos.x, -pos.y, -5);
}
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC *, HGLRC *);
void DisableOpenGL(HWND, HDC, HGLRC);

void Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
        "GLSample",
        "OpenGL Sample",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        500,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    Init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glFrustum(-1, 1, -1, 1, 0.1, 80);
    gluPerspective(50.f, 1.f, 0.1f, 100.f);
    glMatrixMode(GL_MODELVIEW);

    LARGE_INTEGER startCounter, endCounter;    // Limit FPS
    LARGE_INTEGER currentCounter, lastCounter; // Delta time
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastCounter);

    float fpsTime = 0.f;
    double angle = 0;

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            QueryPerformanceCounter(&startCounter);

            // Get delta time
            QueryPerformanceCounter(&currentCounter);
            float dt = getSecondsPerFrame(lastCounter, currentCounter);
            // std::cout << dt << std::endl;
            fpsTime += dt;
            if (fpsTime > 0.3)
            {
                std::ostringstream charStream;
                charStream.precision(2);
                charStream << 1.f / dt;
                SetWindowTextA(hwnd, charStream.str().c_str());
                fpsTime = 0.f;
            }
            QueryPerformanceCounter(&lastCounter);

            /* OpenGL animation code goes here */
            glClearColor(0.5f, 1, 1, 0.5f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            MoveCamera();

            glPushMatrix();
            const double radiusOfOrbit = 3;
            const double angleSpeed = 90;
            angle += angleSpeed * dt * 3.14159 / 180;
            float orbitX = radiusOfOrbit * std::cos(angle);
            float orbitY = radiusOfOrbit * std::sin(angle);
            glTranslatef(orbitX, orbitY, 2.f);
            DrawSphere(1, 20, 20);
            glPopMatrix();

            ShowWorld();
            glPopMatrix();

            SwapBuffers(hDC);

            // Limit the FPS to the max FPS
            QueryPerformanceCounter(&endCounter);
            float secondsPerFrame = getSecondsPerFrame(startCounter, endCounter);
            if (1000.f / maxFPS > secondsPerFrame)
            {
                Sleep(1000.f / maxFPS - secondsPerFrame);
            }
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

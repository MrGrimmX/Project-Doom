#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <Windows.h>

using namespace std;

// Dimensiones de la pantalla de la consola
int nScreenWidth = 120;
int nScreenHeight = 40;

// Variables del jugador
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f; // Ángulo de visión

// Dimensiones del mapa
int nMapHeight = 16;
int nMapWidth = 16;

// Campo de visión (Field of View) y distancia máxima de renderizado
float fFOV = 3.14159f / 4.0f;
float fMaxDepth = 16.0f;

// Sensibilidad del ratón
float fMouseSensitivity = 0.5f;

int main()
{
    // Crear un buffer de pantalla para evitar el parpadeo de la consola
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Ocultar el cursor parpadeante de la consola para que se vea más limpio
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // Obtener el manejador de la ventana de la consola actual
    HWND hwndConsole = GetConsoleWindow();

    // Crear el mapa (# = Pared, . = Espacio vacío)
    wstring map;
    map += L"################";
    map += L"#..............#";
    map += L"#.......########";
    map += L"#..............#";
    map += L"#......##......#";
    map += L"#......##......#";
    map += L"#..............#";
    map += L"###............#";
    map += L"###............#";
    map += L"#..............#";
    map += L"#........#######";
    map += L"#..............#";
    map += L"#..####........#";
    map += L"#....##........#";
    map += L"#................";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    // Bucle principal del juego
    while (1)
    {
        // Calcular el tiempo por cuadro (deltaTime) para un movimiento suave
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        // --- CONTROLES DE MOVIMIENTO (TECLADO) ---
        // Avanzar hacia adelante
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        // Retroceder
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        // Desplazamiento lateral izquierdo (Strafe A)
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        // Desplazamiento lateral derecho (Strafe D)
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        // --- CONTROL DE CÁMARA (RATÓN) ---
        // Solo capturar el ratón si la ventana de la consola está activa (en primer plano)
        if (GetForegroundWindow() == hwndConsole)
        {
            RECT rect;
            if (GetWindowRect(hwndConsole, &rect))
            {
                // Calcular el centro de la ventana de la consola
                int centerX = rect.left + (rect.right - rect.left) / 2;
                int centerY = rect.top + (rect.bottom - rect.top) / 2;

                POINT mousePos;
                GetCursorPos(&mousePos);

                // Calcular la diferencia de movimiento en el eje X
                int deltaX = mousePos.x - centerX;

                // Modificar el ángulo del jugador basándose en el movimiento del ratón
                fPlayerA += ((float)deltaX * fMouseSensitivity) * fElapsedTime;

                // Volver a bloquear el ratón en el centro
                SetCursorPos(centerX, centerY);
            }
        }

        // Permitir salir del juego limpiamente con la tecla ESC
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;

        // --- RENDERIZADO POR COLUMNAS ---
        for (int x = 0; x < nScreenWidth; x++)
        {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;
            float fDistanceToWall = 0.0f;
            bool bHitWall = false;

            float fEyeX = sinf(fRayAngle); 
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fMaxDepth)
            {
                fDistanceToWall += 0.1f;
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fMaxDepth;
                }
                else
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;
                    }
                }
            }

            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';
            if (fDistanceToWall <= fMaxDepth / 4.0f)      nShade = 0x2588; 
            else if (fDistanceToWall < fMaxDepth / 3.0f)  nShade = 0x2593; 
            else if (fDistanceToWall < fMaxDepth / 2.0f)  nShade = 0x2592; 
            else if (fDistanceToWall < fMaxDepth)         nShade = 0x2591; 
            else                                          nShade = ' ';    

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling)
                    screen[y * nScreenWidth + x] = ' '; 
                else if (y >= nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade; 
                else
                {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)      screen[y * nScreenWidth + x] = '#';
                    else if (b < 0.5)  screen[y * nScreenWidth + x] = 'x';
                    else if (b < 0.75) screen[y * nScreenWidth + x] = '.';
                    else               screen[y * nScreenWidth + x] = ' ';
                }
            }
        }

        // Mostrar HUD
        swprintf_s(screen, 60, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f [ESC para salir]", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        // Dibujar Minimapa
        for (int nx = 0; nx < nMapWidth; nx++)
            for (int ny = 0; ny < nMapHeight; ny++)
                screen[ny * nScreenWidth + (nScreenWidth - nMapWidth - 2) + nx] = map[ny * nMapWidth + nx];

        screen[(int)fPlayerY * nScreenWidth + (nScreenWidth - nMapWidth - 2) + (int)fPlayerX] = 'P';

        // Desplegar buffer
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    return 0;
}
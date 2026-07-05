// ASCII Spinning Cube
// A small C++ console project that renders a rotating 3D cube in the Windows terminal.
// Features: perspective projection, z-buffering, dynamic console resizing.

#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <thread>
#include <vector>

struct Vec3
{
    float x;
    float y;
    float z;
};

// Cube and camera settings
const float CUBE_SIZE = 1.0f;
const float CAMERA_DISTANCE = 5.0f;

// Rendering settings
const int MIN_SCREEN_WIDTH = 20;
const int MIN_SCREEN_HEIGHT = 10;
const int POINT_SAMPLES = 45;

// Rotation speed
const float ROTATION_SPEED_X = 0.0025f;
const float ROTATION_SPEED_Y = 0.0035f;
const float ROTATION_SPEED_Z = 0.0010f;

// Frame delay: 16 ms is about 60 FPS
const int FRAME_DELAY_MS = 16;

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

int screenWidth = 0;
int screenHeight = 0;

std::string screenBuffer;
std::string outputBuffer;
std::vector<float> zBuffer;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

void hideCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;

    if (!GetConsoleCursorInfo(consoleHandle, &cursorInfo))
    {
        return;
    }

    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

bool updateConsoleSize()
{
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (!GetConsoleScreenBufferInfo(consoleHandle, &info))
    {
        return false;
    }

    int newWidth = info.srWindow.Right - info.srWindow.Left + 1;

    // The last console line is avoided to reduce accidental scrolling.
    int newHeight = info.srWindow.Bottom - info.srWindow.Top;

    if (newWidth < MIN_SCREEN_WIDTH)
    {
        newWidth = MIN_SCREEN_WIDTH;
    }

    if (newHeight < MIN_SCREEN_HEIGHT)
    {
        newHeight = MIN_SCREEN_HEIGHT;
    }

    if (newWidth == screenWidth && newHeight == screenHeight)
    {
        return false;
    }

    screenWidth = newWidth;
    screenHeight = newHeight;

    screenBuffer.assign(screenWidth * screenHeight, ' ');
    zBuffer.assign(screenWidth * screenHeight, 0.0f);

    outputBuffer.clear();
    outputBuffer.reserve((screenWidth + 1) * screenHeight);

    return true;
}

void clearBuffers()
{
    std::fill(screenBuffer.begin(), screenBuffer.end(), ' ');
    std::fill(zBuffer.begin(), zBuffer.end(), 0.0f);
}

Vec3 rotatePoint(Vec3 point)
{
    float sinX = std::sin(angleX);
    float cosX = std::cos(angleX);

    float sinY = std::sin(angleY);
    float cosY = std::cos(angleY);

    float sinZ = std::sin(angleZ);
    float cosZ = std::cos(angleZ);

    // Rotation around X axis
    float x1 = point.x;
    float y1 = point.y * cosX - point.z * sinX;
    float z1 = point.y * sinX + point.z * cosX;

    // Rotation around Y axis
    float x2 = x1 * cosY + z1 * sinY;
    float y2 = y1;
    float z2 = -x1 * sinY + z1 * cosY;

    // Rotation around Z axis
    float x3 = x2 * cosZ - y2 * sinZ;
    float y3 = x2 * sinZ + y2 * cosZ;
    float z3 = z2;

    return { x3, y3, z3 };
}

void drawPoint(Vec3 point, char symbol)
{
    Vec3 rotated = rotatePoint(point);

    float z = rotated.z + CAMERA_DISTANCE;

    if (z <= 0.0f)
    {
        return;
    }

    float oneOverZ = 1.0f / z;

    // The cube size depends mostly on terminal height.
    // This makes horizontal resizing smoother.
    float projectionScale = screenHeight * 0.95f;

    int screenX = static_cast<int>(
        screenWidth / 2.0f + projectionScale * oneOverZ * rotated.x * 2.0f
    );

    int screenY = static_cast<int>(
        screenHeight / 2.0f - projectionScale * oneOverZ * rotated.y
    );

    if (screenX < 0 || screenX >= screenWidth || screenY < 0 || screenY >= screenHeight)
    {
        return;
    }

    int index = screenX + screenY * screenWidth;

    // Z-buffering:
    // if several points hit the same terminal cell, keep the closest one.
    if (oneOverZ > zBuffer[index])
    {
        zBuffer[index] = oneOverZ;
        screenBuffer[index] = symbol;
    }
}

void drawCube()
{
    float step = (CUBE_SIZE * 2.0f) / POINT_SAMPLES;

    for (int i = 0; i <= POINT_SAMPLES; i++)
    {
        float x = -CUBE_SIZE + i * step;

        for (int j = 0; j <= POINT_SAMPLES; j++)
        {
            float y = -CUBE_SIZE + j * step;

            drawPoint({ x, y, -CUBE_SIZE }, '@');  // front face
            drawPoint({ x, y,  CUBE_SIZE }, '#');  // back face
            drawPoint({ -CUBE_SIZE, y, x }, '~');  // left face
            drawPoint({  CUBE_SIZE, y, x }, '$');  // right face
            drawPoint({ x,  CUBE_SIZE, y }, '+');  // top face
            drawPoint({ x, -CUBE_SIZE, y }, ';');  // bottom face
        }
    }
}

void render()
{
    outputBuffer.clear();

    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            outputBuffer += screenBuffer[x + y * screenWidth];
        }

        if (y != screenHeight - 1)
        {
            outputBuffer += '\n';
        }
    }

    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;

    SetConsoleCursorPosition(consoleHandle, cursorPosition);

    DWORD written = 0;
    WriteConsoleA(
        consoleHandle,
        outputBuffer.c_str(),
        static_cast<DWORD>(outputBuffer.size()),
        &written,
        nullptr
    );
}

int main()
{
    hideCursor();
    updateConsoleSize();

    while (true)
    {
        updateConsoleSize();

        clearBuffers();
        drawCube();
        render();

        angleX += ROTATION_SPEED_X;
        angleY += ROTATION_SPEED_Y;
        angleZ += ROTATION_SPEED_Z;

        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY_MS));
    }

    return 0;
}

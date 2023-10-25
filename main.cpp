#include "raylib.h"
#include <glm/glm.hpp>

#include <stdlib.h>         // Required for: malloc(), free()
#include <string.h>         // Required for: memcpy()
#include <vector>

#define MAX_SAMPLES              512 //512
#define MAX_SAMPLES_PER_UPDATE   4096 //4096

struct StringNode
{
    float position;
    float velocity;
};

std::vector<StringNode> string(128*4);

void updateString()
{
    for(int samples = 0; samples < 6; samples++)
    {
    for(int s = 1; s < string.size()-2; s++)
    {
        string[s].velocity += ((string[s-1].position+string[s+1].position)/2)-string[s].position;
    }
    for(int s = 1; s < string.size()-2; s++)
    {
        string[s].position += string[s].velocity;
    }
    string[0].position = 0;
    string[string.size()-1].position = 0;
    }
}

int harmonic = 1;
void AudioInputCallback(void *buffer, unsigned int frames)
{
    short *d = (short *)buffer;

    for (int i = 0; i < frames; i++)
    {
        if(i%2==0)
        {
            updateString();
        }
        for(int s = 1; s < string.size()-1; s++)
        {
            string[s].velocity *= 0.99998;
            d[i] += string[s/harmonic].position;
        }
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1500;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - raw audio streaming");

    InitAudioDevice();              // Initialize audio device

    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);

    // Init raw audio stream (sample rate: 44100, sample size: 16bit-short, channels: 1-mono)
    AudioStream stream = LoadAudioStream(44100, 16, 1);

    SetAudioStreamCallback(stream, AudioInputCallback);

    // Buffer for the single cycle waveform we are synthesizing

    // Frame buffer, describing the waveform when repeated over the course of a frame
    short *writeBuf = (short *)malloc(sizeof(short)*MAX_SAMPLES_PER_UPDATE);

    PlayAudioStream(stream);        // Start processing stream buffer (no data loaded currently)

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            float stringSize = string.size();
            for(int s = 0; s < stringSize; s++)
            {
                float velocity = sinf32(((float)s/stringSize) * (PI * harmonic)) / 10.0 * harmonic;
                string[s].velocity += velocity;
            }
        }

        

        // Refill audio stream if required
        if (IsAudioStreamProcessed(stream))
        {
            UpdateAudioStream(stream, writeBuf, MAX_SAMPLES_PER_UPDATE);
        }
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            if(IsKeyPressed(KEY_UP))
            {
                DrawRectangle(0,0,screenWidth,32,{0,0,0,255});
                harmonic += 1;
            }
            if(IsKeyPressed(KEY_DOWN))
            {
                DrawRectangle(0,0,screenWidth,32,{0,0,0,255});
                harmonic -= 1;
                if(harmonic < 1)
                {
                    harmonic = 1;
                }
            }
            //ClearBackground({0,16,32,1});
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,2});
            for(uint i = 0; i < string.size(); i++)
            {
                //DrawRectangle(0,screenHeight/2+springs[i].position,screenWidth,32,{255,255,255,3});
                DrawPixel(i * screenWidth / string.size(),screenHeight/2 + string[i].position*3,WHITE);
            }
            DrawText(TextFormat("pluck harmonic: %i",harmonic),8,8,32,WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(writeBuf);             // Unload write buffer

    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

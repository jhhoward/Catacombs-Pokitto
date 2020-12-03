#include "Pokitto.h"
#include "PokittoTimer.h"
#include "PokittoSound.h"
#include "Platform.h"
#include "Defines.h"
#include "Game.h"
#include "Font.h"

Pokitto::Core pokitto;
unsigned long lastTimingSample;

uint8_t Platform::GetInput(void)
{
  uint8_t result = 0;
  
  if(pokitto.buttons.aBtn())
  {
    result |= INPUT_B;  
  }
  if(pokitto.buttons.bBtn())
  {
    result |= INPUT_A;  
  }
  if(pokitto.buttons.cBtn())
  {
    result |= INPUT_START;  
  }
  if(pokitto.buttons.upBtn())
  {
    result |= INPUT_UP;  
  }
  if(pokitto.buttons.downBtn())
  {
    result |= INPUT_DOWN;  
  }
  if(pokitto.buttons.leftBtn())
  {
    result |= INPUT_LEFT;  
  }
  if(pokitto.buttons.rightBtn())
  {
    result |= INPUT_RIGHT;  
  }

  return result;
}

void Platform::SetLED(uint8_t r, uint8_t g, uint8_t b)
{

}

uint8_t* Platform::GetScreenBuffer()
{
    return Pokitto::Display::getBuffer();
}

const unsigned int sfx_3_jump_length = 1329;
const unsigned char sfx_3_jump[] = {
117,98,99,120,145,131,124,114,105,96,87,79,70,63,53,46,36,37,54,72,89,107,123,141,157,174,190,207,223,239,220,191,162,135,104,97,130,159,193,201,184,168,153,136,123,103,99,106,135,142,127,123,112,106,96,89,81,74,66,58,51,43,39,52,69,85,101,115,131,146,162,176,192,206,222,232,212,185,160,134,108,96,125,151,182,198,187,172,158,143,130,116,100,102,112,141,137,127,122,113,107,98,92,84,78,70,64,56,51,42,46,61,75,90,104,118,131,145,159,173,186,200,213,228,223,197,176,151,130,103,99,125,150,176,198,190,176,164,151,138,125,114,98,104,109,138,141,129,127,117,114,106,102,94,90,83,79,73,69,63,59,53,49,43,46,57,66,77,86,95,103,112,120,128,136,144,151,159,166,173,180,186,192,198,203,209,214,218,223,217,208,200,191,183,175,167,160,152,145,137,130,123,116,110,102,100,107,114,122,129,136,143,150,157,164,170,177,181,178,174,170,167,163,160,156,153,149,147,143,140,136,134,130,128,124,123,119,117,113,111,108,108,114,117,126,99,125,143,133,137,132,134,130,131,127,128,125,125,123,123,121,120,119,118,117,116,115,114,113,112,111,110,110,109,108,107,107,106,106,105,104,104,103,103,102,102,101,101,100,99,98,98,97,96,95,95,96,98,100,101,103,105,106,108,110,111,113,114,116,117,119,120,122,123,125,126,128,130,131,133,134,136,137,139,140,142,143,145,146,148,149,151,152,154,155,157,159,160,162,163,165,166,167,166,163,160,158,155,152,150,147,145,142,140,137,135,132,130,127,125,122,120,117,114,115,117,120,123,126,128,131,134,136,139,142,144,147,149,152,155,155,153,152,150,149,148,147,145,144,143,141,140,139,138,136,135,133,133,131,130,128,127,125,125,123,122,120,120,118,117,115,115,117,119,122,123,127,104,120,139,132,135,131,133,131,131,130,130,129,129,128,127,126,126,125,124,124,123,123,122,121,120,120,119,119,118,118,117,116,115,115,114,114,113,112,112,111,111,110,109,109,108,108,107,106,106,105,105,104,104,103,102,102,101,101,100,99,99,98,98,97,97,96,96,95,94,94,93,93,95,96,97,98,99,100,102,103,104,105,106,108,109,110,111,112,114,115,116,117,118,120,121,122,123,125,126,127,129,130,131,133,134,135,137,138,140,141,143,144,146,147,149,150,152,153,155,157,158,160,161,163,165,166,168,169,167,164,161,158,155,152,149,145,142,139,136,133,130,127,123,120,117,114,115,120,123,127,131,135,139,142,146,150,154,155,152,150,148,146,145,142,141,138,137,134,133,130,129,126,125,122,120,119,116,117,119,126,114,127,136,130,132,129,130,127,127,125,125,123,122,121,120,119,118,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,99,102,104,106,109,111,113,115,117,120,122,124,126,128,130,132,134,136,139,141,143,145,147,149,151,154,156,158,160,160,155,152,148,144,140,136,132,128,124,120,117,119,124,128,132,136,141,145,149,149,147,144,142,140,138,135,133,131,129,127,125,123,120,119,118,125,118,127,134,130,131,128,128,126,126,124,123,122,121,120,119,118,116,116,114,114,112,112,110,110,108,108,107,106,105,104,105,107,110,112,114,117,119,121,123,125,127,129,131,134,136,138,140,141,143,145,147,149,151,153,154,150,147,144,140,137,134,130,127,124,120,119,123,127,131,134,138,141,145,144,142,140,138,136,134,132,130,128,127,125,123,121,119,125,121,127,133,129,130,127,127,126,125,124,123,122,121,120,119,118,117,117,116,115,114,113,112,112,111,110,109,109,111,113,115,117,119,121,122,124,126,128,130,132,133,135,137,139,141,142,144,146,148,148,144,141,138,135,132,128,125,122,121,125,128,132,135,138,141,140,138,136,135,133,131,129,128,126,124,123,121,125,123,128,131,128,129,127,127,125,125,124,123,122,122,121,120,119,118,118,117,116,116,115,114,113,114,116,117,119,121,122,124,125,127,128,129,131,133,134,136,137,139,140,141,143,142,139,136,134,131,129,127,124,123,125,128,130,133,135,137,137,135,134,133,131,130,129,128,126,125,124,123,125,124,129,130,128,128,127,127,126,125,124,124,123,123,122,122,121,121,120,120,119,118,118,117,118,119,120,121,122,124,125,126,127,128,129,130,131,132,133,133,134,135,136,137,138,138,136,135,133,132,130,129,127,126,124,125,127,128,130,131,133,134,134,133,132,131,130,129,129,128,127,126,126,125,125,126,125,128,129,128,128,127,127,127,127,126,126,125,125,125,125,124,124,124,124,123,123,123,123,122,122,122,122,123,123,124,124,125,125,126,126,127,127,128,128,129,129,129,130,130,130,131,131,131,132,132,132,132,131,131,130,129,129,128,128,127,127,126,126,127,127,128,128,129,129,130,130,129,129,129,129,128,128,128,128,128,127,127,127,127,127,127,127,127,127,127,128,128,128,128,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127
};

const unsigned char wave[] =
{
    0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255  
};

void Platform::PlaySound(const uint16_t* audioPattern)
{
    //Pokitto::Sound::playSFX(sfx_3_jump, sfx_3_jump_length);
 //   Pokitto::Sound::playSFX(wave, sizeof(wave));
}

bool Platform::IsAudioEnabled()
{
    return true;
}

void Platform::SetAudioEnabled(bool isEnabled)
{
    
}

void Platform::ExpectLoadDelay()
{
    lastTimingSample = Pokitto::Core::getTime();
}

void Platform::FillScreen(uint8_t col)
{
    Pokitto::Display::fillScreen(col);
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
    Pokitto::Display::drawPixel(x, y, colour);
}

void Platform::DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
    
}

void Platform::DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
    
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
    
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
	uint8_t w = bitmap[0];
	uint8_t h = bitmap[1];

	bitmap += 2;

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int blockY = j / 8;
			int blockIndex = (w * blockY + i) * 2;
			uint8_t pixels = bitmap[blockIndex];
			uint8_t maskPixels = bitmap[blockIndex + 1];
			uint8_t bitmask = 1 << (j % 8);

			if (maskPixels & bitmask)
			{
				if (x + i >= 0 && y + j >= 0)
				{
					if (pixels & bitmask)
					{
						PutPixel(x + i, y + j, COLOUR_WHITE);
					}
					else
					{
						PutPixel(x + i, y + j, 0);
					}
				}
			}
		}
	}
}

void Platform::DrawVLine(uint8_t x, int y1, int y2, uint8_t colour)
{
    Pokitto::Display::setColor(colour);
    if(y1 < 0)
        y1 = 0;
    if(y2 >= DISPLAY_HEIGHT)
        y2 = DISPLAY_HEIGHT - 1;
    Pokitto::Display::drawColumn(x, y1, y2);
}

void Platform::SetPalette(const uint16_t* palette)
{
    Pokitto::Display::load565Palette(palette);
}

int main()
{
    unsigned long tickAccum = 0;
    
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    PC::begin();
    PD::persistence = true;
    PD::invisiblecolor = 256;
    Pokitto::Sound::ampEnable(true);

    lastTimingSample = Pokitto::Core::getTime();

    while(Platform::GetInput() != 0)
    {
        pokitto.update();
    }

    Game::Init();

    /*uint8_t bright[3] = { 255, 198, 145 };
    uint8_t dark[3] = { 14, 20, 32 };
    for(int n = 0; n < 16; n++)
    {
        uint8_t intensity = (uint8_t) ((n * 255) / 15);
        uint8_t r = (intensity * bright[0] + (255 - intensity) * dark[0]) / 256;
        uint8_t g = (intensity * bright[1] + (255 - intensity) * dark[1]) / 256;
        uint8_t b = (intensity * bright[2] + (255 - intensity) * dark[2]) / 256;
        PD::paletteptr[n + 16] = PD::RGBto565(r, g, b);
    }*/
    
    //Pokitto::Sound::playTone(1, 50, 128, 100);

    const bool visualiseLighting = false;
    
    if(visualiseLighting)
    {
        for(int n = 0; n < 256; n++)
        {
            uint8_t intensity = (uint8_t) (((n & 0xf) * 255) / 15);
            PD::paletteptr[n] = PD::RGBto565(intensity, intensity, intensity);
        }
    }
    
    while( PC::isRunning() )
    {
        if( !PC::update() ) 
            continue;
            
        unsigned long timingSample = Pokitto::Core::getTime();
        tickAccum += (timingSample - lastTimingSample);
        lastTimingSample = timingSample;

    	constexpr int16_t frameDuration = 1000 / TARGET_FRAMERATE;
    	while(tickAccum > frameDuration)
    	{
    		Game::Tick();
    		tickAccum -= frameDuration;
    		//tickAccum = 0;
    	}

        timingSample = Pokitto::Core::getTime();
        Game::Draw();
        unsigned long actualFrameTime = Pokitto::Core::getTime() - timingSample;
        int fps = 1000 / actualFrameTime;
        Font::PrintInt((int16_t) fps, 0, 0);
    }
    
    return 0;
}

#include "Pokitto.h"
#include "PokittoTimer.h"
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

void Platform::PlaySound(const uint16_t* audioPattern)
{
    
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
						PutPixel(x + i, y + j, 1);
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


int main()
{
    unsigned long tickAccum = 0;
    
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    PC::begin();
    PD::persistence = true;
    PD::invisiblecolor = 255;

    lastTimingSample = Pokitto::Core::getTime();

    while(Platform::GetInput() != 0)
    {
        pokitto.update();
    }

    Game::Init();

    uint8_t bright[3] = { 255, 198, 145 };
    uint8_t dark[3] = { 14, 20, 32 };
    for(int n = 0; n < 16; n++)
    {
        uint8_t intensity = (uint8_t) ((n * 255) / 15);
        uint8_t r = (intensity * bright[0] + (255 - intensity) * dark[0]) / 256;
        uint8_t g = (intensity * bright[1] + (255 - intensity) * dark[1]) / 256;
        uint8_t b = (intensity * bright[2] + (255 - intensity) * dark[2]) / 256;
        PD::paletteptr[n + 16] = PD::RGBto565(r, g, b);
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
    		tickAccum = 0;
    	}

        timingSample = Pokitto::Core::getTime();
        Game::Draw();
        unsigned long actualFrameTime = Pokitto::Core::getTime() - timingSample;
        int fps = 1000 / actualFrameTime;
        Font::PrintInt((int16_t) fps, 0, 0);
    }
    
    return 0;
}

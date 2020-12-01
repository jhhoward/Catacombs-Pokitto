#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../Catacombs/lodepng.h"

using namespace std;

#define GENERATED_PATH "Source/Catacombs/Generated"

const char* spriteDataHeaderOutputPath = GENERATED_PATH "/SpriteData.inc.h";
const char* spriteTypesHeaderOutputPath = GENERATED_PATH "/SpriteTypes.h";

constexpr int numColours = 16;
uint16_t palette[numColours];
uint8_t paletteRGB[numColours * 3];
const char* palettePath = "Images/palette.png";
const char* paletteOutputPath = GENERATED_PATH "/Palette.inc.h";
const char* texturesOutputPath = GENERATED_PATH "/Textures.inc.h";

enum class ImageColour
{
	Transparent,
	Black,
	White
};

ImageColour CalculateColour(vector<uint8_t>& pixels, unsigned int x, unsigned int y, unsigned int pitch)
{
	unsigned int index = (y * pitch + x) * 4;
	if(pixels[index] == 0 && pixels[index + 1] == 0 && pixels[index + 2] == 0 && pixels[index + 3] == 255)
	{
		return ImageColour::Black;
	}
	else if (pixels[index] == 255 && pixels[index + 1] == 255 && pixels[index + 2] == 255 && pixels[index + 3] == 255)
	{
		return ImageColour::White;
	}

	return ImageColour::Transparent;
}

void EncodeSprite3D(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);
	
	if(error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}
	
	if(height != 16)
	{
		cout << inputPath << " : sprite must be 16 pixels high" << endl;
		return;
	}
	if((width % 16) != 0)
	{
		cout << inputPath << " : sprite width must be multiple of 16 pixels" << endl;
		return;
	}
	
	vector<uint16_t> colourMasks;
	vector<uint16_t> transparencyMasks;
	
	for(unsigned x = 0; x < width; x++)
	{
		uint16_t colour = 0;
		uint16_t transparency = 0;
		
		for(unsigned y = 0; y < height; y++)
		{
			uint16_t mask = (1 << y);
			ImageColour pixelColour = CalculateColour(pixels, x, y, width);
			
			if(pixelColour == ImageColour::Black)
			{
				// Black
				transparency |= mask;
			}
			else if (pixelColour == ImageColour::White)
			{
				// White
				transparency |= mask;
				colour |= mask;
			}
		}
		
		colourMasks.push_back(colour);
		transparencyMasks.push_back(transparency);
	}
	
	unsigned int numFrames = width / 16;

	typefs << "// Generated from " << inputPath << endl;
	typefs << "constexpr uint8_t " << variableName << "_numFrames = " << dec << numFrames << ";" << endl;
	typefs << "extern const uint16_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "constexpr uint8_t " << variableName << "_numFrames = " << dec << numFrames << ";" << endl;
	fs << "extern const uint16_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl << "\t";
	
	for(unsigned x = 0; x < width; x++)
	{
		// Interleaved transparency and colour
		fs << "0x" << hex << transparencyMasks[x] << ",0x" << hex << colourMasks[x];
	
		if(x != width - 1)
		{
			fs << ",";
		}
	}
	
	fs << endl;	
	fs << "};" << endl;	
}

void EncodeTextures(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);
	
	if(error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}
	
	if(height != 16)
	{
		cout << inputPath << " : texture must be 16 pixels high" << endl;
		return;
	}
	if((width % 16) != 0)
	{
		cout << inputPath << " : texture width must be multiple of 16 pixels" << endl;
		return;
	}
	
	vector<uint16_t> colourMasks;
	
	for(unsigned x = 0; x < width; x++)
	{
		uint16_t colour = 0;
		
		for(unsigned y = 0; y < height; y++)
		{
			uint16_t mask = (1 << y);
			ImageColour pixelColour = CalculateColour(pixels, x, y, width);
			
			if(pixelColour != ImageColour::Black)
			{
				// White
				colour |= mask;
			}
		}
		
		colourMasks.push_back(colour);
	}
	
	unsigned int numTextures = width / 16;

	typefs << "// Generated from " << inputPath << endl;
	typefs << "constexpr uint8_t " << variableName << "_numTextures = " << dec << numTextures << ";" << endl;
	typefs << "extern const uint16_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "constexpr uint8_t " << variableName << "_numTextures = " << dec << numTextures << ";" << endl;
	fs << "extern const uint16_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl << "\t";
	
	for(unsigned x = 0; x < width; x++)
	{
		fs << "0x" << hex << colourMasks[x];
	
		if(x != width - 1)
		{
			fs << ",";
		}
	}
	
	fs << endl;	
	fs << "};" << endl;	
}

void EncodeHUDElement(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);

	if (error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}

	if (height != 8)
	{
		cout << inputPath << " : height must be 8 pixels" << endl;
		return;
	}

	vector<uint8_t> colourMasks;

	for (unsigned y = 0; y < height; y += 8)
	{
		for (unsigned x = 0; x < width; x++)
		{
			uint8_t colour = 0;

			for (unsigned z = 0; z < 8 && y + z < height; z++)
			{
				uint8_t mask = (1 << z);

				ImageColour pixelColour = CalculateColour(pixels, x, y + z, width);

				if (pixelColour == ImageColour::White)
				{
					// White
					colour |= mask;
				}
			}

			colourMasks.push_back(colour);
		}
	}

	typefs << "// Generated from " << inputPath << endl;
	typefs << "extern const uint8_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "extern const uint8_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl;

	for (unsigned x = 0; x < colourMasks.size(); x++)
	{
		fs << "0x" << hex << (int)(colourMasks[x]);

		if (x != colourMasks.size() - 1)
		{
			fs << ",";
		}
	}

	fs << endl;
	fs << "};" << endl;
}

void EncodeSprite2D(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);
	
	if(error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}
	
	unsigned x1 = 0, y1 = 0;
	unsigned x2 = width, y2 = height;
	
	// Crop left
	bool isCropping = true;
	for(unsigned x = x1; x < x2 && isCropping; x++)
	{
		for(unsigned y = y1; y < y2; y++)
		{
			ImageColour colour = CalculateColour(pixels, x, y, width);
			
			if(colour != ImageColour::Transparent)
			{
				isCropping = false;
				break;
			}
		}
		
		if(isCropping)
		{
			x1++;
		}
	}

	// Crop right
	isCropping = true;
	for(unsigned x = x2 - 1; x >= x1 && isCropping; x--)
	{
		for(unsigned y = y1; y < y2; y++)
		{
			ImageColour colour = CalculateColour(pixels, x, y, width);
			
			if(colour != ImageColour::Transparent)
			{
				isCropping = false;
				break;
			}
		}
		
		if(isCropping)
		{
			x2--;
		}
	}

	// Crop top
	isCropping = true;
	for(unsigned y = y1; y < y2 && isCropping; y++)
	{
		for(unsigned x = x1; x < x2; x++)
		{
			ImageColour colour = CalculateColour(pixels, x, y, width);
			
			if(colour != ImageColour::Transparent)
			{
				isCropping = false;
				break;
			}
		}
		
		if(isCropping)
		{
			y1++;
		}
	}

	// Crop bottom
	isCropping = true;
	for(unsigned y = y2 - 1; y >= y1 && isCropping; y--)
	{
		for(unsigned x = x1; x < x2; x++)
		{
			ImageColour colour = CalculateColour(pixels, x, y, width);
			
			if(colour != ImageColour::Transparent)
			{
				isCropping = false;
				break;
			}
		}
		
		if(isCropping)
		{
			y2--;
		}
	}
	
	vector<uint8_t> colourMasks;
	vector<uint8_t> transparencyMasks;
	
	for(unsigned y = y1; y < y2; y += 8)
	{
		for(unsigned x = x1; x < x2; x++)
		{
			uint8_t colour = 0;
			uint8_t transparency = 0;
			
			for(unsigned z = 0; z < 8 && y + z < height; z++)
			{
				uint8_t mask = (1 << z);
				
				ImageColour pixelColour = CalculateColour(pixels, x, y + z, width);
				
				if(pixelColour == ImageColour::Black)
				{
					// Black
					transparency |= mask;
				}
				else if (pixelColour == ImageColour::White)
				{
					// White
					transparency |= mask;
					colour |= mask;
				}
			}
			
			colourMasks.push_back(colour);
			transparencyMasks.push_back(transparency);
		}
	}

	typefs << "// Generated from " << inputPath << endl;
	typefs << "extern const uint8_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "extern const uint8_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl;
	fs << "\t" << dec << (x2 - x1) << ", " << dec << (y2 - y1) << "," << endl << "\t";
	
	for(unsigned x = 0; x < colourMasks.size(); x++)
	{
		fs << "0x" << hex << (int)(colourMasks[x]) << ",";
		fs << "0x" << hex << (int)(transparencyMasks[x]);
	
		if(x != colourMasks.size() - 1)
		{
			fs << ",";
		}
	}
	
	fs << endl;	
	fs << "};" << endl;	
	
/*	fs << "const uint8_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl;
	fs << "\t" << dec << (x2 - x1) << ", " << dec << (y2 - y1) << "," << endl << "\t";
	
	for(unsigned x = 0; x < colourMasks.size(); x++)
	{
		fs << "0x" << hex << (int)(colourMasks[x]);
	
		if(x != colourMasks.size() - 1)
		{
			fs << ",";
		}
	}
	
	fs << endl;	
	fs << "};" << endl;	
	fs << "const uint8_t " << variableName << "_mask[] PROGMEM =" << endl;
	fs << "{" << endl << "\t";
	
	for(unsigned x = 0; x < transparencyMasks.size(); x++)
	{
		fs << "0x" << hex << (int)(transparencyMasks[x]);
	
		if(x != transparencyMasks.size() - 1)
		{
			fs << ",";
		}
	}

	fs << endl;	
	fs << "};" << endl;	
	*/
}

uint16_t ToRGB565(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t b = (blue >> 3) & 0x1f;
	uint16_t g = ((green >> 2) & 0x3f) << 5;
	uint16_t r = ((red >> 3) & 0x1f) << 11;

	return (uint16_t)(r | g | b);
}

int GetPaletteIndex(uint8_t red, uint8_t green, uint8_t blue)
{
	int closest = -1;
	int closestDist = 0;

	for (int n = 0; n < numColours; n++)
	{
		int distance = 0;
		distance += (red - paletteRGB[n * 3]) * (red - paletteRGB[n * 3]);
		distance += (green - paletteRGB[n * 3 + 1]) * (green - paletteRGB[n * 3 + 1]);
		distance += (blue - paletteRGB[n * 3 + 2]) * (blue - paletteRGB[n * 3 + 2]);
		if (closest == -1 || distance < closestDist)
		{
			closest = n;
			closestDist = distance;
		}
	}

	return closest;
}

bool LoadPalette()
{
	vector<unsigned char> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, palettePath);

	if (error)
	{
		cerr << "Error loading palette file" << endl;
		return false;
	}

	if (width != numColours || height != 1)
	{
		cerr << "Palette wrong dimensions" << endl;
		return false;
	}

	for (int n = 0; n < numColours; n++)
	{
		palette[n] = ToRGB565(pixels[n * 4], pixels[n * 4 + 1], pixels[n * 4 + 2]);
		paletteRGB[n * 3] = pixels[n * 4];
		paletteRGB[n * 3 + 1] = pixels[n * 4 + 1];
		paletteRGB[n * 3 + 2] = pixels[n * 4 + 2];
	}

	//uint8_t bright[3] = { 255, 198, 145 };
	uint8_t bright[3] = { 255, 255, 255 };
	uint8_t dark[3] = { 14, 20, 32 };

	ofstream paletteHeader;
	paletteHeader.open(paletteOutputPath);
	paletteHeader << "const uint16_t GamePalette[] = {" << endl << "  ";
	for (int n = 0; n < 256; n++)
	{
		int paletteIndex = n >> 4;
		int intensity = n & 15;

		//float alpha = (intensity + 1.0f) / 16.0f;
		float alpha = (intensity / 15.0f);
		float lightR = (bright[0] * alpha + dark[0] * (1.0f - alpha)) / 255.0f;
		float lightG = (bright[1] * alpha + dark[1] * (1.0f - alpha)) / 255.0f;
		float lightB = (bright[2] * alpha + dark[2] * (1.0f - alpha)) / 255.0f;
				
		float r = paletteRGB[paletteIndex * 3] * lightR;
		float g = paletteRGB[paletteIndex * 3 + 1] * lightG;
		float b = paletteRGB[paletteIndex * 3 + 2] * lightB;
		int value565 = ToRGB565((uint8_t)r, (uint8_t)g, (uint8_t)b);
		//int value565 = ToRGB565((uint8_t)(alpha * paletteRGB[paletteIndex * 3]), (uint8_t)(alpha * paletteRGB[paletteIndex * 3 + 1]), (uint8_t)(alpha * paletteRGB[paletteIndex * 3 + 2]));
		paletteHeader << value565;

		if (n != 255)
		{
			paletteHeader << ", ";
		}
	}
	//for (int n = 0; n < numColours; n++)
	//{
	//	paletteHeader << palette[n];
	//	if (n != numColours - 1)
	//	{
	//		paletteHeader << ", ";
	//	}
	//}
	paletteHeader << endl << "};" << endl;
	paletteHeader.close();

	return true;
}

void EncodeColourTextures(const char* filename)
{
	vector<unsigned char> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, filename);

	if (!error)
	{
		ofstream of;
		of.open(texturesOutputPath);
		of << "const uint8_t ColourTextures[] = {" << endl << "  ";

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int p = (y * width + x) * 4;
				int paletteIndex = GetPaletteIndex(pixels[p], pixels[p + 1], pixels[p + 2]);
				paletteIndex <<= 4;
				of << paletteIndex;
				of << ", ";
			}
		}
		of << endl;
		of << "};" << endl;

		of.close();
	}
}


int main(int argc, char* argv[])
{
	if (!LoadPalette())
	{
		cerr << "Failed to load palette file" << endl;
		return -1;
	}

	EncodeColourTextures("Images/textures-colour.png");

	ofstream dataFile;
	ofstream typeFile;

	dataFile.open(spriteDataHeaderOutputPath);
	typeFile.open(spriteTypesHeaderOutputPath);

	EncodeSprite3D(typeFile, dataFile, "Images/enemy.png", "skeletonSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/mage.png", "mageSpriteData");
//	EncodeSprite3D(typeFile, dataFile, "Images/skeleton.png", "skeletonSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/torchalt1.png", "torchSpriteData1");
	EncodeSprite3D(typeFile, dataFile, "Images/torchalt2.png", "torchSpriteData2");
	EncodeSprite3D(typeFile, dataFile, "Images/fireball2.png", "projectileSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/fireball.png", "enemyProjectileSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/entrance.png", "entranceSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/exit.png", "exitSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/urn.png", "urnSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/sign.png", "signSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/crown.png", "crownSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/coins.png", "coinsSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/scroll.png", "scrollSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/chest.png", "chestSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/chestopen.png", "chestOpenSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/potion.png", "potionSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/bat.png", "batSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/spider.png", "spiderSpriteData");

	EncodeSprite2D(typeFile, dataFile, "Images/hand1.png", "handSpriteData1");
	EncodeSprite2D(typeFile, dataFile, "Images/hand2.png", "handSpriteData2");

	EncodeTextures(typeFile, dataFile, "Images/textures.png", "wallTextureData");

	EncodeHUDElement(typeFile, dataFile, "Images/font.png", "fontPageData");
	EncodeHUDElement(typeFile, dataFile, "Images/heart.png", "heartSpriteData");
	EncodeHUDElement(typeFile, dataFile, "Images/mana.png", "manaSpriteData");

	dataFile.close();
	typeFile.close();
	
	return 0;
}



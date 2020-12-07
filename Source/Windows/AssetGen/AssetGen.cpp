#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "../Catacombs/lodepng.h"
#include "OriginalSounds.inc.h"

using namespace std;

#define GENERATED_PATH "Source/Catacombs/Generated"

const char* audioDataHeaderOutputPath = GENERATED_PATH "/AudioData.inc.h";

const char* spriteDataHeaderOutputPath = GENERATED_PATH "/SpriteData.inc.h";
const char* spriteTypesHeaderOutputPath = GENERATED_PATH "/SpriteTypes.h";

constexpr int numColours = 16;
uint16_t palette[numColours];
uint8_t paletteRGB[numColours * 3];
const char* palettePath = "Images/palette.png";
const char* lightingPath = "Images/lighting.png";
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
	{
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
	}

	bool useLightingRamp = true;
	vector<unsigned char> lighting;
	{
		unsigned width, height;
		unsigned error = lodepng::decode(lighting, width, height, lightingPath);

		if (error)
		{
			cerr << "Error loading lighting file" << endl;
			return false;
		}

		if (width != numColours || height != 1)
		{
			cerr << "Lighting wrong dimensions" << endl;
			return false;
		}
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

	uint8_t bloody[3] = { 142, 16, 23 };

	ofstream paletteHeader;
	paletteHeader.open(paletteOutputPath);
	paletteHeader << "const uint16_t GamePalette[] = {" << endl << "  ";
	for (int n = 0; n < 256; n++)
	{
		int paletteIndex = n >> 4;
		int intensity = n & 15;
		float lightR, lightG, lightB;

		if (useLightingRamp)
		{
			lightR = lighting[intensity * 4] / 255.0f;
			lightG = lighting[intensity * 4 + 1] / 255.0f;
			lightB = lighting[intensity * 4 + 2] / 255.0f;
		}
		else
		{
			float alpha = (intensity / 15.0f);
			lightR = (bright[0] * alpha + dark[0] * (1.0f - alpha)) / 255.0f;
			lightG = (bright[1] * alpha + dark[1] * (1.0f - alpha)) / 255.0f;
			lightB = (bright[2] * alpha + dark[2] * (1.0f - alpha)) / 255.0f;
		}
				
		float r = paletteRGB[paletteIndex * 3] * lightR;
		float g = paletteRGB[paletteIndex * 3 + 1] * lightG;
		float b = paletteRGB[paletteIndex * 3 + 2] * lightB;
		int value565 = ToRGB565((uint8_t)r, (uint8_t)g, (uint8_t)b);
		paletteHeader << value565 << ", ";
	}

	for (int shade = 0; shade < 3; shade++)
	{
		float bloodAlpha = (shade + 1.0f) / 5.0f;

		for (int n = 0; n < 256; n++)
		{
			int paletteIndex = n >> 4;
			int intensity = n & 15;

			float lightR, lightG, lightB;

			if (useLightingRamp)
			{
				lightR = lighting[intensity * 4] / 255.0f;
				lightG = lighting[intensity * 4 + 1] / 255.0f;
				lightB = lighting[intensity * 4 + 2] / 255.0f;
			}
			else
			{
				float alpha = (intensity / 15.0f);
				lightR = (bright[0] * alpha + dark[0] * (1.0f - alpha)) / 255.0f;
				lightG = (bright[1] * alpha + dark[1] * (1.0f - alpha)) / 255.0f;
				lightB = (bright[2] * alpha + dark[2] * (1.0f - alpha)) / 255.0f;
			}

			float r = paletteRGB[paletteIndex * 3] * lightR;
			float g = paletteRGB[paletteIndex * 3 + 1] * lightG;
			float b = paletteRGB[paletteIndex * 3 + 2] * lightB;

			r = bloody[0] * bloodAlpha + (1.0f - bloodAlpha) * r;
			g = bloody[1] * bloodAlpha + (1.0f - bloodAlpha) * g;
			b = bloody[2] * bloodAlpha + (1.0f - bloodAlpha) * b;

			int value565 = ToRGB565((uint8_t)r, (uint8_t)g, (uint8_t)b);
			paletteHeader << value565 << ", ";
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

		int numTextures = height / width;

		for (int t = 0; t < numTextures; t++)
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < width; y++)
				{
					int p = (t * width * width + y * width + x) * 4;
					int paletteIndex = GetPaletteIndex(pixels[p], pixels[p + 1], pixels[p + 2]);
					paletteIndex <<= 4;
					of << paletteIndex;
					of << ", ";
				}
			}

		}

		of << endl;
		of << "};" << endl;

		of.close();
	}
}

void EncodeColourSprite3D(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);

	if (error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}

	if (height != 16)
	{
		cout << inputPath << " : sprite must be 16 pixels high" << endl;
		return;
	}
	if ((width % 16) != 0)
	{
		cout << inputPath << " : sprite width must be multiple of 16 pixels" << endl;
		return;
	}

	unsigned int numFrames = width / 16;
	typefs << "// Generated from " << inputPath << endl;
	typefs << "constexpr uint8_t " << variableName << "_numFrames = " << dec << numFrames << ";" << endl;
	typefs << "extern const uint8_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "constexpr uint8_t " << variableName << "_numFrames = " << dec << numFrames << ";" << endl;
	fs << "extern const uint8_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl << "\t";

	for (unsigned x = 0; x < width; x++)
	{
		for (unsigned y = 0; y < height; y++)
		{
			int index = (y * width + x) * 4;
			int paletteIndex = GetPaletteIndex(pixels[index], pixels[index + 1], pixels[index + 2]);
			paletteIndex <<= 4;

			if (pixels[index + 3] != 255)
			{
				paletteIndex = 0xf;
			}

			fs << paletteIndex;
			if (y != height - 1 || x != width - 1)
			{
				fs << ", ";
			}
		}
	}

	fs << endl;
	fs << "};" << endl;
}

void EncodeColourSprite2D(ofstream& typefs, ofstream& fs, const char* inputPath, const char* variableName)
{
	vector<uint8_t> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, inputPath);

	if (error)
	{
		cout << inputPath << " : decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}

	typefs << "// Generated from " << inputPath << endl;
	typefs << "extern const uint8_t " << variableName << "[];" << endl;

	fs << "// Generated from " << inputPath << endl;
	fs << "extern const uint8_t " << variableName << "[] PROGMEM =" << endl;
	fs << "{" << endl << "\t";

	fs << width << ", " << height << ", ";

	for (unsigned y = 0; y < height; y++)
	{
		for (unsigned x = 0; x < width; x++)
		{
			int index = (y * width + x) * 4;
			int paletteIndex = GetPaletteIndex(pixels[index], pixels[index + 1], pixels[index + 2]);
			paletteIndex <<= 4;

			if (pixels[index + 3] != 255)
			{
				paletteIndex = 0xf;
			}

			fs << paletteIndex;
			if (y != height - 1 || x != width - 1)
			{
				fs << ", ";
			}
		}
	}

	fs << endl;
	fs << "};" << endl;
}

struct wav_hdr_s {
	uint8_t	RIFF[4];	/* RIFF Header	  */ //Magic header
	uint32_t ChunkSize;	  /* RIFF Chunk Size  */
	uint8_t	WAVE[4];	/* WAVE Header	  */
	uint8_t	fmt[4];	 /* FMT header	   */
	uint32_t Subchunk1Size;  /* Size of the fmt chunk				*/
	uint16_t AudioFormat;	/* Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM */
	uint16_t NumOfChan;	  /* Number of channels 1=Mono 2=Sterio		   */
	uint32_t SamplesPerSec;  /* Sampling Frequency in Hz				 */
	uint32_t bytesPerSec;	/* bytes per second */
	uint16_t blockAlign;	 /* 2=16-bit mono, 4=16-bit stereo */
	uint16_t bitsPerSample;  /* Number of bits per sample	  */
	uint8_t	Subchunk2ID[4]; /* "data"  string   */
	uint32_t Subchunk2Size;  /* Sampled data length	*/
};

void EncodeSound(ofstream& ofs, const uint16_t* data, const char* varName)
{
	list<uint8_t> samples;

	const uint16_t* ptr = data;
	constexpr int sampleRate = 8000;
	
	uint8_t output = 0;

	while (*ptr != TONES_END)
	{
		uint16_t frequency = *ptr++;
		uint16_t duration = *ptr++;

		int numSamples = (duration * sampleRate) / 1000;
		int freqCounter = 0;
		for (int n = 0; n < numSamples; n++)
		{
			if (frequency != 0)
			{
				/*double PI = 3.141592;
				double wave = ((double)sampleRate / frequency);
				double amplitude = sin(n * PI * 2 / wave);
				output = (uint8_t)(amplitude * 127 + 127);
				*/
				
				freqCounter++;
				int flip = (sampleRate / frequency);
				output = 255 - (freqCounter * 255 / flip);
				if (freqCounter >= flip)
				{
					freqCounter = 0;
				}

				/*freqCounter++;
				int flip = (sampleRate / frequency);
				if (freqCounter >= flip)
				{
					output = output == 0 ? 255 : 0;
					freqCounter = 0;
				}*/
			}
			samples.push_back(output);
		}
	}

	ofs << "const int " << varName << "_length = " << samples.size() << ";" << endl;
	ofs << "const uint8_t " << varName << "[] = {" << endl;
	for (uint8_t& sample : samples)
	{
		ofs << ((int)sample) << ",";
	}
	ofs << "};" << endl << endl;

	{
		wav_hdr_s wav_hdr;
		wav_hdr.AudioFormat = 1; //PCM
		wav_hdr.bitsPerSample = 8;
		wav_hdr.blockAlign = 1;
		wav_hdr.ChunkSize = sizeof(wav_hdr) - 4;
		memcpy(&wav_hdr.WAVE[0], "WAVE", 4);
		memcpy(&wav_hdr.fmt[0], "fmt ", 4);
		wav_hdr.NumOfChan = 1;
		wav_hdr.bytesPerSec = sampleRate * (uint32_t)(wav_hdr.bitsPerSample >> 3) * (uint32_t)wav_hdr.NumOfChan;
		memcpy(&wav_hdr.RIFF[0], "RIFF", 4);
		wav_hdr.Subchunk1Size = 16;
		wav_hdr.SamplesPerSec = sampleRate;
		memcpy(&wav_hdr.Subchunk2ID[0], "data", 4);
		wav_hdr.Subchunk2Size = samples.size();

		ostringstream oss;
		oss << varName << ".wav";

		ofstream wavFs(oss.str(), ios::binary);
		wavFs.write((char*)&wav_hdr, sizeof(wav_hdr));

		for (uint8_t& sample : samples)
		{
			wavFs.write((char*)(&sample), 1);
		}

		wavFs.close();
	}
}

void EncodeWav(ofstream& ofs, const char* filename, const char* varName)
{
	ifstream fs(filename, ios::binary);

	if (!fs.is_open())
	{
		cout << "Error loading " << filename << endl;
		return;
	}

	wav_hdr_s header;
	fs.read((char*)&header, sizeof(wav_hdr_s));

	bool success = true;

	if (header.NumOfChan != 1)
	{
		cout << filename << " has more than one channel" << endl;
		success = false;
	}
	if (header.bitsPerSample != 8)
	{
		cout << filename << " is not 8 bit" << endl;
		success = false;
	}
	if (header.SamplesPerSec != 8000)
	{
		cout << filename << " is not 8kHz: is " << header.SamplesPerSec << endl;
		success = false;
	}
	if (header.AudioFormat != 1)
	{
		cout << filename << " is not PCM" << endl;
		success = false;
	}

	cout << "Expecting " << header.Subchunk2Size << " bytes of data" << endl;

	if (success)
	{
		ofs << "// Generated from " << filename << endl;
		ofs << "const int " << varName << "_length = " << header.Subchunk2Size << ";" << endl;
		ofs << "const uint8_t " << varName << "[] = {" << endl;
		for(int n = 0; n < header.Subchunk2Size; n++)
		{
			unsigned char sample = 0;
			fs.read((char*) &sample, 1);
			ofs << ((int)sample) << ",";
		}
		ofs << "};" << endl << endl;
	}


	fs.close();
}

int main(int argc, char* argv[])
{
	if (!LoadPalette())
	{
		cerr << "Failed to load palette file" << endl;
		return -1;
	}

	EncodeColourTextures("Images/textures-colour.png");

	ofstream audioFile;
	audioFile.open(audioDataHeaderOutputPath);
	//EncodeWav(audioFile, "Sounds/fireball.wav", "fireballSound");
	EncodeSound(audioFile, Sounds::Attack, "Sounds::Attack");
	EncodeSound(audioFile, Sounds::Hit, "Sounds::Hit");
	EncodeSound(audioFile, Sounds::Kill, "Sounds::Kill");
	EncodeSound(audioFile, Sounds::Ouch, "Sounds::Ouch");
	EncodeSound(audioFile, Sounds::Pickup, "Sounds::Pickup");
	EncodeSound(audioFile, Sounds::PlayerDeath, "Sounds::PlayerDeath");
	EncodeSound(audioFile, Sounds::Shoot, "Sounds::Shoot");
	EncodeSound(audioFile, Sounds::SpotPlayer, "Sounds::SpotPlayer");
	audioFile.close();

	ofstream dataFile;
	ofstream typeFile;

	dataFile.open(spriteDataHeaderOutputPath);
	typeFile.open(spriteTypesHeaderOutputPath);

/*	EncodeSprite3D(typeFile, dataFile, "Images/enemy.png", "skeletonSpriteData");
	EncodeSprite3D(typeFile, dataFile, "Images/mage.png", "mageSpriteData");
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
	*/

	EncodeColourSprite3D(typeFile, dataFile, "Images/enemyc.png", "skeletonSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/magec.png", "mageSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/torchalt1c.png", "torchSpriteData1");
	EncodeColourSprite3D(typeFile, dataFile, "Images/torchalt2c.png", "torchSpriteData2");
	EncodeColourSprite3D(typeFile, dataFile, "Images/fireball2c.png", "projectileSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/fireballc.png", "enemyProjectileSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/entrancec.png", "entranceSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/exitc.png", "exitSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/urnc.png", "urnSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/signc.png", "signSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/crownc.png", "crownSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/coinsc.png", "coinsSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/scrollc.png", "scrollSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/chestc.png", "chestSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/chestopenc.png", "chestOpenSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/potionc.png", "potionSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/batc.png", "batSpriteData");
	EncodeColourSprite3D(typeFile, dataFile, "Images/spiderc.png", "spiderSpriteData");

	EncodeColourSprite2D(typeFile, dataFile, "Images/hand1c.png", "handSpriteData1");
	EncodeColourSprite2D(typeFile, dataFile, "Images/hand2c.png", "handSpriteData2");

	EncodeColourSprite2D(typeFile, dataFile, "Images/hudc.png", "statusBarData");
	EncodeColourSprite2D(typeFile, dataFile, "Images/logoc.png", "logoData");

	//EncodeSprite2D(typeFile, dataFile, "Images/hand1.png", "handSpriteData1");
	//EncodeSprite2D(typeFile, dataFile, "Images/hand2.png", "handSpriteData2");

	EncodeTextures(typeFile, dataFile, "Images/textures.png", "wallTextureData");

	EncodeHUDElement(typeFile, dataFile, "Images/font.png", "fontPageData");
	EncodeHUDElement(typeFile, dataFile, "Images/heart.png", "heartSpriteData");
	EncodeHUDElement(typeFile, dataFile, "Images/mana.png", "manaSpriteData");

	dataFile.close();
	typeFile.close();
	
	return 0;
}



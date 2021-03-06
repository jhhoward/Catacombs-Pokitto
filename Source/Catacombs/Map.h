#pragma once

#include <stdint.h>

enum class CellType : uint8_t 
{
	Empty = 0,

	// Monster types
	Monster,

	// Non collidable decorations
	Torch,
	Entrance,
	Exit,

	// Items
	Potion,
	Coins,
	Crown,
	Scroll,

	// Collidable decorations
	Urn,
	Chest,
	ChestOpened,
	Sign,

	// Solid cells
	BrickWall,
	WallDecoration1,
	WallDecoration2,
	WallDecoration3,
	WallDecoration4,
	WallDecoration5,
	
	FirstCollidableCell = Urn,
	FirstSolidCell = BrickWall
};

class Map
{
public:
	static constexpr int width = 32;
	static constexpr int height = 24;

	static bool IsSolid(uint8_t x, uint8_t y);
	static bool IsBlocked(uint8_t x, uint8_t y);
	static inline bool IsBlockedAtWorldPosition(int16_t x, int16_t y)
	{
		return IsBlocked((uint8_t)(x >> 8), (uint8_t)(y >> 8));
	}

	static CellType GetCell(uint8_t x, uint8_t y);
	static CellType GetCellSafe(uint8_t x, uint8_t y);
	static void SetCell(uint8_t x, uint8_t y, CellType cellType);
	static void DebugDraw();
	static void DrawMinimap();

	static bool IsClearLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	
	static void LoadMenuMap();
	
	static void RevertToStaticLightMap();
	static void GenerateLightMap();
	static void AddLight(int x, int y, int reach);
	static void AddDynamicLight(int x, int y, int intensity);
	static uint8_t GetLightingAtCell(int x, int y);
	static uint8_t SampleWorldLighting(int x, int y);

private:	
	static uint8_t level[width * height];
	static uint8_t lightMap[width * height];
	static uint8_t staticLightMap[width * height];
	
	static constexpr int menuMapWidth = 8;
    static constexpr int menuMapHeight = 8;
    static const uint8_t menuMap[menuMapWidth * menuMapHeight];

};

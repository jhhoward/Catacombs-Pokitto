#pragma once
#include <stdint.h>
#include "Defines.h"

class Sounds
{
public:
	static const uint8_t Attack[];
	static const uint8_t Kill[];
	static const uint8_t Hit[];
	static const uint8_t PlayerDeath[];
	static const uint8_t SpotPlayer[];
	static const uint8_t Shoot[];
	static const uint8_t Pickup[];
	static const uint8_t Ouch[];

	static const int Attack_length;
	static const int Kill_length;
	static const int Hit_length;
	static const int PlayerDeath_length;
	static const int SpotPlayer_length;
	static const int Shoot_length;
	static const int Pickup_length;
	static const int Ouch_length;

};
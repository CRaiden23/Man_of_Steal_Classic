#pragma once

#include "Sound.h"
#include "GameDefines.h"
#include "Sndlist.h"

// Polymorphed CAUdio class that plays music in 2d space opposed to 3d space

class CMusic : public CAudio {
public:
	CSoundDesc loop(int i, float volume = 1.0f); ///< play looped music
	bool Fade(const CSoundDesc& d, float newVol);
}; //CMusic
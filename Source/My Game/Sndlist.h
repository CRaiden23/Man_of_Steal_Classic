/// \file SndList.h
/// \brief Enumerated type for sounds.

#pragma once

#include "Sound.h"

/// \brief Game sound enumerated type. 
///
/// These are the sounds used in actual gameplay. 
/// The sounds must be listed here in the same order that
/// they are in the sound settings XML file.

enum eSoundType{ 
  SILENCE_SOUND, PUNCH_SOUND, PUNCHWALL_SOUND, OW_SOUND, CLANG_SOUND, MONEY_SOUND, FX_EXPLODE_SOUND,
  SONG1_SOUND, SONG2_SOUND, SONG3_SOUND, SONG4_SOUND, SONG5_SOUND, MUS_AMBIENT00_SOUND, MUS_ALERT00_SOUND, MUS_BOSS00_SOUND,
  PHURT1_SOUND, PHURT2_SOUND, PHURT3_SOUND,
  PHURT4_SOUND, EHURT1_SOUND, EHURT2_SOUND, EHURT3_SOUND,
  EHURT4_SOUND, NIGHTV_SOUND, NIGHTV2_SOUND, SPOT_SOUND, BATTERYP_SOUND,
  ETRAPPIT_CLICK, ETRAPPIT_OPEN, GUN_SOUND, BLIP_SOUND, END_SOUND, ENTER_SOUND
}; //eSoundType
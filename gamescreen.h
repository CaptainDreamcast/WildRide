#pragma once

#include <prism/wrapper.h>
#include <prism/mugenanimationhandler.h>
#include <prism/mugensoundfilereader.h>

#define TEXT_Z 50
#define TILE_Z 20
#define COASTER_Z 10
#define BOSS_Z 5
#define CHECKPOINT_Z 5

Screen* getGameScreen();

MugenSpriteFile* getGameSprites();
MugenAnimations* getGameAnimations();
MugenSounds* getGameSounds();
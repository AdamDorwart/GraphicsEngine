#pragma once
#include "Texture.h"

bool ParsePPMTexture(Texture* texture, 
					 const char* filename, 
					 unsigned int texType, 
					 unsigned int side);
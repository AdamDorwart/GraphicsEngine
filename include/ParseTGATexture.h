#pragma once
#include "Texture.h"

bool ParseTGATexture(Texture* texture, 
					 const char* filename, 
					 unsigned int texType, 
					 unsigned int side);
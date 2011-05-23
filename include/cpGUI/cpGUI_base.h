// cpGUI 
// 
// Copyright (c) 2009 Jason Cupp
// Copyright 2010 Patrick VanDusen
//
//This software is provided 'as-is', without any express or implied warranty. 
//In no event will the authors be held liable for any damages arising from the 
//use of this software.
//
//Permission is granted to anyone to use this software for any purpose, including 
//commercial applications, and to alter it and redistribute it freely, subject to 
//the following restrictions:
//
//
//1. The origin of this software must not be misrepresented; you must not claim 
//that you wrote the original software. If you use this software in a product, an 
//acknowledgment in the product documentation would be appreciated but is not required.
//
//
//2. Altered source versions must be plainly marked as such, and must not be 
//misrepresented as being the original software.
//
//
//3. This notice may not be removed or altered from any source distribution.



#ifndef CPGUI_BASE
#define CPGUI_BASE

#include <cmath>
#include <cstring>

#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#include <SFML/Graphics.hpp>

namespace cp
{
	enum
	{
		CP_DEFAULT = -1,
		CP_ST_MOUSE_ENTER,
		CP_ST_MOUSE_EXIT,
		CP_ST_MOUSE_IN,
		CP_ST_MOUSE_LBUTTON_DOWN,
		CP_ST_MOUSE_LBUTTON_RELEASED,
		CP_ST_CHANGED,
		CP_ST_NONE,
		CP_TXT_RIGHT_ALIGN,
		CP_TXT_LEFT_ALIGN,
		CP_TXT_CENTER_ALIGN
	};

	struct FontData
	{
		sf::Font *myFont;
		short timesUsed;
	};

}

#endif

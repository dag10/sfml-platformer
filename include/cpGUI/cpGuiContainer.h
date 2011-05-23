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

#ifndef CP_GUI_CONTAINER_H
#define CP_GUI_CONTAINER_H

namespace cp {

	class cpObject;

	class cpGuiContainer
	{
	public:
		cpGuiContainer();
		~cpGuiContainer();
		void Register(cpObject* object);
		void Unregister(cpObject* object);
		void SetObjectFocus(cpObject* object, bool focused);
		void NextObjectFocus();
		void AcceptsKeys(bool accept, bool UseArrowKeys);
		void ProcessKeys(sf::Event *evt);
		void Show(bool show);
		bool GetShow();
		void ReleaseUnusedFonts();
		sf::Font* GetFont(std::string filename, unsigned int fontSize=20);

		std::vector<cpObject*> control;
		std::string defaultFilename;
		bool acceptsKeys, useArrowKeys, bShow;
		sf::Clock clock;
		
	private:
		typedef std::map<std::string, FontData> FontMap;
		std::map<std::string, FontData> fontMap;
		short rightleft;
		sf::Font *defaultFont;
	};

}

#endif

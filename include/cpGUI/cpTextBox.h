//cpGUI - Copyright (c) 2009 Jason Cupp
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

#ifndef CPTEXTBOX
#define CPTEXTBOX

#include "cpGUI_base.h"
#include "cpObject.h"
#include "cpScrollBar.h"

namespace cp
{
	class cpTextBox : public cpObject
	{
	public:
		cpTextBox(sf::RenderWindow *parent, cpGuiContainer *GUI, float posx=0,
			float posy=0, float width=CP_DEFAULT, float height=CP_DEFAULT);
		~cpTextBox();
		void Draw();
		void LoadFile( const char* const filename);
		int CheckState(const sf::Input *input);

		bool SetSize(float width, float height);
		void SetPosition(float posx, float posy);
		void SetFont(std::string filename, unsigned int size);
		void SetBackgroundColor(sf::Color color);
		void SetFontStyle(unsigned long TextStyle);
		void SetFontSize(unsigned int size);
		void SetLabelColor(sf::Color color);


	private:
		std::vector<std::string> lineStrings;
		std::vector<sf::String> lineSFStrings;
		const char* Filename;
		int viewableLines, totalLines, startingLine;
		unsigned int fontSize;
		cpScrollBar *scrollbar;
		sf::Clock clock;
	};
}

#endif

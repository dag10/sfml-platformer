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


#ifndef CPCHECKBOX
#define CPCHECKBOX

#include "cpGUI_base.h"
#include "cpButton.h"

namespace cp
{
	class cpCheckBox : public cpButton
	{
	public:
		cpCheckBox();
		cpCheckBox(sf::RenderWindow* parent, cpGuiContainer *GUI, std::string label, 
			float posx=0, float posy=0);
		void Draw();
		int CheckState(const sf::Input *input); 
		bool SetSize(float width, float height);
		void SetChecked(bool checked);
		bool GetChecked();

	private:
		void CreateRects(std::string label);

		sf::Shape line1, line2, line3;
		bool isChecked;

	};
}

#endif

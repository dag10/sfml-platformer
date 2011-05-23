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




#include "cpGUI_base.h"
#include "cpCheckBox.h"
using namespace cp;


/// **********************************************************
/// A simple on/off check box with a label
/// **********************************************************
cpCheckBox::cpCheckBox(sf::RenderWindow *parent, cpGuiContainer *GUI, 
					   std::string label, float posx, float posy) : cpButton(parent,
					   GUI, label, posx, posy, CP_DEFAULT, CP_DEFAULT)
{
	labelInside = false;
	isChecked = false;
	Width = Label.GetRect().GetHeight() - 4;
	Height = Width;
	CreateRects(Label.GetText());
}

cpCheckBox::cpCheckBox() : cpButton(){}

/// Updates the look of the check box
void cpCheckBox::CreateRects(std::string label)
{
	Label.SetText(label);
	Label.SetColor(textColor);
	Width = Label.GetRect().GetHeight() - 4;
	if(Width < 20)
		Width = 20;
	Height = Width;


	backRect = sf::Shape::Rectangle(PosX, PosY, PosX+Width, PosY+Height,
		backgroundColor, 1.f, outlineColor);

	gradient = sf::Shape::Rectangle(PosX+2, PosY+2, PosX+Width-2,
		PosY+Height-2, backgroundColor, 1.f, backgroundColor2);
	sf::Color color = sf::Color(backgroundColor2.r+35, 
		backgroundColor2.g+35, backgroundColor2.b+35);
	gradient.SetPointColor(0, backgroundColor2);
	gradient.SetPointColor(1,color);
	gradient.SetPointColor(2, backgroundColor);
	gradient.SetPointColor(3, color);

	float thick = Width/10;

	line1 = sf::Shape::Line(PosX+3, PosY+(Height/2), PosX+thick*2, PosY+(Height/2),
		thick, textColor);
	line2 = sf::Shape::Line(PosX+thick*2, PosY+(Height/2), PosX+Width-(Width*2/3), PosY+Height-3,
		thick, textColor);
	line3 = sf::Shape::Line(PosX+Width-(Width*2/3), PosY+Height-3, PosX+Width-3, PosY+3,
		thick, textColor);

	Label.SetPosition(PosX + Width + 4, PosY+Height/3 - Label.GetRect().GetHeight()/2);
}

/// Draws all the parts of the checkbox
void cpCheckBox::Draw()
{
	if(!bShow)
		return;

	cpButton::Draw();

	if(isChecked)
	{
		Parent->Draw(line1);
		Parent->Draw(line2);
		Parent->Draw(line3);
	}
}

/// This function checks for mouse events within the control.
/// it updates the look of the control according to the event.
/// it also returns an integer enumeration member to inform the user.
/// possible return values are:
/// CP_ST_NONE, CP_ST_MOUSE_ENTER, CP_ST_MOUSE_EXIT,
/// CP_ST_MOUSE_IN, CP_ST_LBUTTON_DOWN, CP_ST_LBUTTON_RELEASED.
/// 
/// The difference between CP_ST_MOUSE_ENTER & CP_ST_MOUSE_IN
/// is that IN is active the entire time the mouse is inside the control
/// and ENTER is only active right when the mouse enters the control
int cpCheckBox::CheckState(const sf::Input *input)
{
	int state = cpButton::CheckState(input);

	if(state == CP_ST_NONE)
	{
		if(hasFocus)
			outlineColor = sf::Color(90,90,230);
		else
			outlineColor = sf::Color::Black;
	}
	else
		outlineColor = outlineColorTemp;

	if(state == CP_ST_MOUSE_LBUTTON_RELEASED)
		isChecked = !isChecked;

	CreateRects(Label.GetText());

	return state;
}

/// This is not available.  The size is based on
/// the size of the Label's font.
bool cpCheckBox::SetSize(float width, float height)
{
	return false;
}

/// Set whether or not the box is checked
void cpCheckBox::SetChecked(bool checked)
{
	isChecked = checked;
}

/// Returns the state of the check box
bool cpCheckBox::GetChecked()
{
	return isChecked;
}

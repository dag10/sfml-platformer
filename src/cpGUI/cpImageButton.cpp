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
#include "cpImageButton.h"
using namespace cp;


/// ******************************************************************
/// A class of buttons that contain an image.
/// These buttons cannot be resized; the size is based on the 
/// size of the image sent to it.  Also, there is no text label.
/// ******************************************************************
cpImageButton::cpImageButton(sf::RenderWindow *parent, cpGuiContainer *GUI, 
							 sf::Image *image, float posx, float posy) : cpObject(parent,
							 GUI, "", posx, posy, (float)image->GetWidth()+3,
								(float)image->GetHeight()+3)				
{
	sprite.SetImage(*image);
	sprite.SetPosition(posx + 1, posy + 1);
}

cpImageButton::cpImageButton() : cpObject(NULL, NULL, ""){}

/// Clears the sprite and loads the new image into it.
/// the sprite is used to display the image in the button.
void cpImageButton::SetImage(sf::Image *image)
{
	Width = (float)image->GetWidth()+3;
	Height = (float)image->GetHeight()+3;
	sprite.SetImage(sf::Image());
	sprite.SetImage(*image);
	CreateRects("");
}

/// Draws the different parts that make up an image button
void cpImageButton::Draw()
{
	if(!bShow)
		return;

	Parent->Draw(backRect);
	Parent->Draw(sprite);

	if(hasFocus)
		DrawDashedRect(PosX+2, PosY+2, Width-4, Height-4);
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
int cpImageButton::CheckState(const sf::Input *input)
{
	int state = cpObject::CheckState(input);
	
	if(state == CP_ST_MOUSE_LBUTTON_DOWN)
		backgroundColor = sf::Color(140, 140, 140);

	if(state == CP_ST_MOUSE_LBUTTON_RELEASED)
		backgroundColor = backgroundColorTemp;

	CreateRects("");
	return state;
}

/// Sets the image button's width & height dimensions
bool cpImageButton::SetSize(float width, float height)
{
	return false;
}

/// Sets the image button's position coordinates in the window
void cpImageButton::SetPosition(float posx, float posy)
{
	cpObject::SetPosition(posx, posy);
	CreateRects("");
}

/// Sets the image button's background color
void cpImageButton::SetBackgroundColor(sf::Color color)
{
	cpObject::SetBackgroundColor(color);
	CreateRects("");
}

/// Sets whether the image button is shown or hidden
void cpImageButton::Show(bool show)
{
	cpObject::Show(show);
	CreateRects("");
}

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
#include "cpTextInputBox.h"
using namespace cp;


/// ***********************************************************
/// Rudimentary text input box.  It only allows you to enter
/// the number of characters that the box can contain.  The
/// font size is tied to the height of the box, so you can't
/// change the font size.
///
/// You can set CP_TXT_LEFT_ALIGN, CP_TXT_RIGHT_ALIGN, or
/// CP_TXT_CENTER_ALIGN.  This simple text box does not
/// support text selection, but you can backspace.
/// ***********************************************************
cpTextInputBox::cpTextInputBox(sf::RenderWindow *parent, cpGuiContainer *GUI,
							   std::string label, float posx, float posy,
							   float width, float height, int Style) : cpObject(
							   parent, GUI, label, posx, posy, width, height)
{
	backgroundColor = sf::Color::White;
	bTooBig = false;
	caretOn = true;
	elapsedTime = 0.f;
	SetFont(Font, (unsigned int)Height-2);
	Label.SetSize(Height-2);
	tempLabel = Label;
	tempText = Label.GetText();

	style = Style;
	if(style < CP_TXT_RIGHT_ALIGN || style > CP_TXT_CENTER_ALIGN)
		style = CP_TXT_LEFT_ALIGN;

	CreateRects(Label.GetText());

}

cpTextInputBox::cpTextInputBox() : cpObject(NULL, NULL, ""){}

/// Updates the look of the text input box.
/// Computes the text alignement and position of the
/// Caret/cursor.
void cpTextInputBox::CreateRects(std::string label)
{
	Label.SetText(label);
	Label.SetColor(textColor);
	Label.SetSize(Height-2);
	tempLabel = Label;

	float thickness = 1.f;
	backRect = sf::Shape::Rectangle(PosX, PosY, PosX+Width, PosY+Height,
		backgroundColor, thickness);
	for(int t=0; t<4; t++)
		backRect.SetPointOutlineColor(t, outlineColor);

	if(style == CP_TXT_RIGHT_ALIGN)
		Label.SetPosition(PosX+Width-Label.GetRect().GetWidth()-8, PosY-Height/6);
	if(style == CP_TXT_LEFT_ALIGN)
		Label.SetPosition(PosX+4, PosY-Height/6);
	if(style == CP_TXT_CENTER_ALIGN)
		Label.SetPosition(PosX+(Width/2)-(Label.GetRect().GetWidth()/2),
			PosY-Height/6);

	caret = sf::Shape::Line(Label.GetPosition().x + Label.GetRect().GetWidth() + 3,
		PosY+2, Label.GetPosition().x + Label.GetRect().GetWidth() + 3, PosY+Height-2,
		1.f, sf::Color(50,50,50));

}

/// Draws the text input box. The cursor/caret is
/// drawn every .75 seconds
void cpTextInputBox::Draw()
{
	if(!bShow)
		return;

	Parent->Draw(backRect);
	Parent->Draw(Label);

	if(gui->clock.GetElapsedTime() - elapsedTime > 0.75)
	{
		elapsedTime = gui->clock.GetElapsedTime();
		caretOn = !caretOn;
	}

	if(hasFocus && caretOn)
		Parent->Draw(caret);
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
int cpTextInputBox::CheckState(const sf::Input *input)
{
	int state = cpObject::CheckState(input);

	if(state == CP_ST_NONE)
	{
		if(hasFocus)
			outlineColor = sf::Color(90,90,230);
		else
			outlineColor = sf::Color::Black;
	}
	else
		outlineColor = outlineColorTemp;

	CreateRects(Label.GetText());
	return state;
}

/// This function is used within the Event loop.
/// This accepts text input and assigns it to
/// the Label if it fits in the box.
void cpTextInputBox::ProcessTextInput(sf::Event *evt)
{
	if(!hasFocus)
		return;
	if (evt->Type == sf::Event::TextEntered)
    {
        int textSize = tempText.size();
        unsigned short unicode = evt->Text.Unicode;

        if (unicode == 8 || unicode == 127) //If backspace
		{
            if (textSize > 0)
                tempText.erase(textSize - 1, 1);
		}
        else if (unicode >= 32 && unicode <= 126)
            tempText += (char)unicode;
        else if (unicode >= 192 && unicode <= 255)
            tempText += (char)unicode;
		return;
	}

    tempLabel.SetText(tempText);
	if(CheckTextFit())
		Label.SetText(tempText);
}

/// Limits the size of the Label to the size of the box
bool cpTextInputBox::CheckTextFit()
{
	if(tempLabel.GetRect().GetWidth() > Width - 8)
	{
		tempText.erase(tempText.size()-1, 1);
		return false;
	}
	return true;
}

/// Set the width & height dimensions of the text input box.
/// also reloads the font with the new font size for
/// better text quality
bool cpTextInputBox::SetSize(float width, float height)
{
	Width = width;
	Height = height;
	SetFont(Font, (unsigned int)Height-2);
	CreateRects(Label.GetText());
	return true;
}

/// Sets the coordinates for the position of the text input box
void cpTextInputBox::SetPosition(float posx, float posy)
{
	cpObject::SetPosition(posx, posy);
	CreateRects(Label.GetText());
}

/// The font can be changed, but the font size
/// is determined by the size of the box.
void cpTextInputBox::SetFont(std::string filename, unsigned int size)
{
	cpObject::SetFont(filename, (unsigned int)Height-2);
	CreateRects(Label.GetText());
}

/// Sets the text input box's background color
void cpTextInputBox::SetBackgroundColor(sf::Color color)
{
	cpObject::SetBackgroundColor(color);
	CreateRects(Label.GetText());
}

/// Sets the font's style: Bold, Italic, Underlined, Regular
void cpTextInputBox::SetFontStyle(unsigned long TextStyle)
{
	cpObject::SetFontStyle(TextStyle);
	CreateRects(Label.GetText());
}

/// Not available.  Font size is determined
/// by the box's height.
void cpTextInputBox::SetFontSize(unsigned int size)
{
}

/// Sets the color of the text
void cpTextInputBox::SetLabelColor(sf::Color color)
{
	cpObject::SetLabelColor(color);
	CreateRects(Label.GetText());
}

/// Sets the text displayed in the text input box
void cpTextInputBox::SetLabelText(std::string text)
{
	cpObject::SetLabelText(text);
	tempText = text;
	CreateRects(text);
}


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




#include "cpGUI_base.h"
#include "cpGuiContainer.h"
#include "cpObject.h"
#include "cpButton.h"
#include "cpScrollBar.h"

#include <stdexcept>

using namespace cp;


/// *********************************************************
/// Class that contains all objects/controls in the GUI.
/// Used mostly for setting focus and keyboard controls.
/// *********************************************************
cpGuiContainer::cpGuiContainer()
{
	clock.Reset();
	control.reserve(10);
	acceptsKeys = true;
	useArrowKeys = true;
	bShow = true;
	rightleft = 1;

	// Find the Windows directory, and load a font
	// segoeui for Vista
	// tahoma for XP
	// arial if the others aren't present
	//
	// If you're not in Windows, you're on your own and you'll
	// have to settle for the default font, or use the
	// cpObject::SetFont() function.
	defaultFont = new sf::Font(sf::Font::GetDefaultFont());
	defaultFilename = "Default";
#ifdef WIN32
	char tempdir[MAX_PATH];
	GetWindowsDirectoryA(tempdir, sizeof(tempdir)/sizeof(char));
	std::string dir;
	dir.assign(tempdir);

	defaultFilename = dir + "/Fonts/segoeui.ttf";
	if(!defaultFont->LoadFromFile(defaultFilename, 20))
	{
		defaultFilename = dir + "/Fonts/tahoma.ttf";
		if(!defaultFont->LoadFromFile(defaultFilename, 20))
		{
			defaultFilename = dir + "/Fonts/arial.ttf";
			defaultFont->LoadFromFile(defaultFilename, 20);
		}
	}
#endif
	std::string key = defaultFilename;
	if(key != "Default")
		key = key + ";20";
	FontData data = {defaultFont, 1};
	fontMap.insert(std::make_pair(key , data));
}

cpGuiContainer::~cpGuiContainer()
{
	while(fontMap.begin() != fontMap.end())
	{
		delete fontMap.begin()->second.myFont;
		fontMap.erase(fontMap.begin());
	}
}

void cpGuiContainer::ReleaseUnusedFonts()
{
	for(FontMap::iterator iter = fontMap.begin(); iter != fontMap.end(); iter++)
	{
		iter->second.timesUsed = 0;
	}

	for(unsigned int t=0; t < control.size(); t++)
	{
		std::string key = "Default";
		if(control[t]->Font != "Default")
		{
			std::stringstream size;
			size << control[t]->textFont->GetCharacterSize();
			key = control[t]->Font + ";" + size.str();
		}
		FontMap::iterator it = fontMap.find(key);
		it->second.timesUsed++;
	}
	FontMap::iterator it1 = fontMap.begin();
	FontMap::iterator it2 = fontMap.end();
	FontMap::iterator itTemp;

	while(it1 != it2)
	{
		if(it1->second.timesUsed == 0)
		{
			if(it1->first != defaultFilename + ";20" && it1->first != "Default")
			{
				sf::Font *oldFont = it1->second.myFont;
				delete oldFont;
				fontMap.erase(it1++);
			}
			else
				++it1;
		}
		else
			++it1;
	}
}

sf::Font* cpGuiContainer::GetFont(std::string filename, unsigned int fontSize)
{
	std::string key = filename;
	if(filename != "Default")
	{
		std::stringstream size;
		size << fontSize;
		key = filename + ";" + size.str();
	}
	FontMap::iterator it = fontMap.find(key);
	if(it == fontMap.end())
	{
		sf::Font *font = new sf::Font;
		if(font->LoadFromFile(filename, fontSize))
		{
			FontData data = {font, 1};
			it = fontMap.insert(std::make_pair(key, data)).first;
		}
		else
			throw std::runtime_error("Font " + filename + " could not be loaded");
	}
	sf::Font *result = it->second.myFont;

	return result;
}

/// All controls that can accept focus or use the
/// mouse wheel should be registered to the GuiContainer.
void cpGuiContainer::Register(cpObject *object)
{
	control.push_back(object);
}

/// This is used to "unregister" a control that has been
/// registered. Used in classes that are a compilation
/// of different objects when each separate object should
/// not receive focus.
void cpGuiContainer::Unregister(cpObject *object)
{
	for(unsigned int t=0; t < control.size(); t++)
	{
		if(control[t] == object)
			control.erase(control.begin()+t);
	}
}

/// Sets or removes focus from an object that is registered
/// to the GuiContainer.
void cpGuiContainer::SetObjectFocus(cpObject *object, bool focused)
{
	if(control.size() == 0)
		return;

	for(unsigned int t = 0; t < control.size(); t++)
	{
		if(control[t] == object)
			control[t]->hasFocus = focused;
		else
			control[t]->hasFocus = false;
	}
}

/// Cause the next object to receive focus.
/// This is not based on screen position.
/// It is based on the order of registration.
void cpGuiContainer::NextObjectFocus()
{
	if(control.size() == 0)
		return;

	bool show = false;
	for(unsigned int t=0; t < control.size(); t++)
	{
		if(control[t]->GetShow())
			show = true;
	}
	if(!show)
		return;

	int focus = -1;
	int size = control.size();
	for(int t = 0; t < size; t++)
	{
		if(control[t]->hasFocus)
			focus = t;
	}
	if(focus > -1)
		control[focus]->hasFocus = false;
	do
	{
		focus = focus + (1 * rightleft);
		if(focus >= size)
			focus = 0;
		if(focus < 0)
			focus = size - 1;
	}while(!control[focus]->GetShow());

	control[focus]->hasFocus = true;
}

/// Sets whether you can use Tab to move focus to the next object.
/// Also sets whether the arrow keys can move the focus.
/// Classes that use arrow keys locally should set
/// UseArrowKeys to false.
void cpGuiContainer::AcceptsKeys(bool accept, bool UseArrowKeys)
{
	acceptsKeys = accept;
	useArrowKeys = UseArrowKeys;
}

/// Used in the event loop to process the focus changing keys.
/// This function also sends the mouse wheel delta to the
/// classes that need that information.
void cpGuiContainer::ProcessKeys(sf::Event *evt)
{
	if(control.size() == 0)
		return;

	int focus = -1;

	for(unsigned int t=0; t < control.size(); t++)
	{
		control[t]->wheelDelta = 0;
		if(control[t]->hasFocus)
			focus = t;
	}
	if(focus > -1)
	{
		if(control[focus]->useWheel)
			control[focus]->wheelDelta = evt->MouseWheel.Delta;
	}


	if(!acceptsKeys)
		return;

	if((evt->Type == sf::Event::KeyPressed) && (evt->Key.Code == sf::Key::Tab))
	{
		rightleft = 1;
		NextObjectFocus();
	}

	if(!useArrowKeys)
		return;

	if(evt->Type == sf::Event::KeyPressed && ((evt->Key.Code == sf::Key::Left)
		|| (evt->Key.Code == sf::Key::Up)))
	{
		rightleft = -1;
		NextObjectFocus();
	}

	if(evt->Type == sf::Event::KeyPressed && ((evt->Key.Code == sf::Key::Right)
		|| (evt->Key.Code == sf::Key::Down)))
	{
		rightleft = 1;
		NextObjectFocus();
	}



}

/// Show or hide every control that is registered
/// to the GuiContainer.
void cpGuiContainer::Show(bool show)
{
	if(control.size() == 0)
		return;

	for(unsigned int t=0; t < control.size(); t++)
		control[t]->Show(show);
	bShow = show;
}

/// Returns whether or not all contained objects are shown.
bool cpGuiContainer::GetShow()
{
	return bShow;
}



/// *********************************************************************
/// Create constructor and functions for cpObject.
/// cpObject is the base class that all other controls inherit from.
/// *********************************************************************
cpObject::cpObject(sf::RenderWindow* parent, cpGuiContainer *GUI, std::string label,
				   float posx, float posy, float width, float height)
{
	GUI->Register(this);

	Width = width;
	Height = height;
	PosX = posx; // control's x & y position
	PosY = posy;
	Parent = parent; // the render window that the control is displayed on
	bDefaultSize = false; // did user set the size or leave it default?
	mouseIn = false;  // is the mouse pointer inside the control?
	lbuttonDown = false; // is the left mouse button down?
	returnDown = false;  // is the return key down?
	hasFocus = false;  // Does this control have the focus?
	bShow = true;  // Should the control be drawn?
	useWheel = false;
	keyCode = 0;
	wheelDelta = 0;
	gui = GUI;
	Font = gui->defaultFilename;


	if(width == CP_DEFAULT || height == CP_DEFAULT)
		bDefaultSize = true;

	backgroundColor = sf::Color(240,240,240);
	backgroundColorTemp = backgroundColor;
	outlineColor = sf::Color(0,0,0);
	outlineColorTemp = outlineColor;
	mouseoverColor = sf::Color(167,217,245);

	SetFont(Font,20);
	if(Font == "Default")
		Label.SetSize(20);

	// Set the generic attributes of the control's Label
	textColor = sf::Color::Black;
	Label.SetText(label);
	Label.SetColor(textColor);

	SetPosition(PosX, PosY);  // if PosX or PosY is invalid, this deals with it

	// create the generic background of a control
	backRect.AddPoint(PosX+2, PosY, backgroundColor);
	backRect.AddPoint(PosX+Width-2, PosY, backgroundColor);
	backRect.AddPoint(PosX+Width, PosY+2, backgroundColor);
	backRect.AddPoint(PosX+Width, PosY+Height-2, backgroundColor);
	backRect.AddPoint(PosX+Width-2, PosY+Height, backgroundColor);
	backRect.AddPoint(PosX+2, PosY+Height, backgroundColor);
	backRect.AddPoint(PosX, PosY+Height-2, backgroundColor);
	backRect.AddPoint(PosX, PosY+2, backgroundColor);
	backRect.SetOutlineWidth(1.f);
	backRect.EnableOutline(true);

}


/// If the given position is invalid, this deals with it
/// This function also updates the control's position when the user moves it
/// by calling this function
void cpObject::SetPosition(float posx, float posy)
{
	PosX = posx;
	PosY = posy;
	if(PosX < 0)
		PosX = 0;
	if(PosY < 0)
		PosY = 0;
	if(PosX > Parent->GetWidth() - Label.GetRect().GetWidth())
		PosX = Parent->GetWidth() - Label.GetRect().GetWidth();
	if(PosY > Parent->GetHeight() - Label.GetRect().GetHeight())
		PosY = Parent->GetHeight() - Label.GetRect().GetHeight();
}

/// Change the size of the control.
/// If the given size is smaller than the size of the control's
/// Label, it returns false and nothing is changed.
bool cpObject::SetSize(float width, float height)
{
	if(Label.GetRect().GetWidth() > Width ||
		Label.GetRect().GetHeight() > Height)
		return false;
	Width = width;
	Height = height;
	bDefaultSize = false;
	return true;
}

/// Sets the font of the control's Label.
/// Send a filename of "Default" to use SFML's
/// default font.
void cpObject::SetFont(std::string filename, unsigned int size)
{
	if(filename == "Default")
		filename = gui->defaultFilename;
	textFont = gui->GetFont(filename, size);
	Label.SetFont(*textFont);
	Label.SetSize((float)textFont->GetCharacterSize());
	if(filename == "Default")
		Label.SetSize((float)size);
	Font = filename;
	gui->ReleaseUnusedFonts();
}

/// Sets the controls generic background color
void cpObject::SetBackgroundColor(sf::Color color)
{
	backgroundColor = color;
	backgroundColorTemp = color;
}

/// Sets the font style for the control's Label.
/// sf::String::Bold | sf::String::Italic | sf::String::Underlined
/// or sf::String::Regular
void cpObject::SetFontStyle(unsigned long TextStyle)
{
	Label.SetStyle(TextStyle);
}

/// Change the font size. The font is actually reloaded
/// with the new size for better quality.
void cpObject::SetFontSize(unsigned int size)
{
	tempLabel = Label;
	SetFont(Font, size);
}

/// Set's color of control's text
void cpObject::SetLabelColor(sf::Color color)
{
	textColor = color;
	Label.SetColor(textColor);
}

/// Sets the text of the control's Label
void cpObject::SetLabelText(std::string text)
{
	Label.SetText(text);
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
int cpObject::CheckState(const sf::Input *input)
{
	int state = CP_ST_NONE;

	if(!bShow)
		return state;

	outlineColor = sf::Color::Black;
	outlineColorTemp = outlineColor;

	if(CheckMouseEntered(input))
	{
		if(mouseIn)
		{
			outlineColor = mouseoverColor;
			outlineColorTemp = outlineColor;
			state = CP_ST_MOUSE_IN;
			mouseIn = true;
		}

		if(!mouseIn)
		{
			state = CP_ST_MOUSE_ENTER;
			mouseIn = true;
		}

		if(input->IsMouseButtonDown(sf::Mouse::Left))
		{
			outlineColor = sf::Color(44, 98, 139);
			outlineColorTemp = outlineColor;
			state = CP_ST_MOUSE_LBUTTON_DOWN;
			lbuttonDown = true;
			gui->SetObjectFocus(this, true);
		}

		if(!input->IsMouseButtonDown(sf::Mouse::Left) &&
			lbuttonDown)
		{
			state = CP_ST_MOUSE_LBUTTON_RELEASED;
			lbuttonDown = false;
		}
	}

	else
	{
		if(mouseIn)
		{
			outlineColor = sf::Color::Black;
			outlineColorTemp = outlineColor;
			state = CP_ST_MOUSE_EXIT;
			mouseIn = false;
		}
	}

	if(hasFocus && gui->acceptsKeys && input->IsKeyDown(sf::Key::Return))
	{
		outlineColor = sf::Color(44, 98, 139);
		outlineColorTemp = outlineColor;
		state = CP_ST_MOUSE_LBUTTON_DOWN;
		returnDown = true;
		gui->SetObjectFocus(this, true);
	}

	if(hasFocus && !input->IsKeyDown(sf::Key::Return) &&
		gui->acceptsKeys && returnDown)
	{
		state = CP_ST_MOUSE_LBUTTON_RELEASED;
		returnDown = false;
	}

	return state;
}

/// Checks to see if mouse pointer is inside control.  Returns true if it is.
bool cpObject::CheckMouseEntered(const sf::Input *input)
{
	//sf::Vector2f mousePos = Parent->ConvertCoords(input->GetMouseX(), input->GetMouseY());
	sf::Vector2f mousePos(input->GetMouseX(), input->GetMouseY());
	return (mousePos.x >= PosX && mousePos.x <= PosX+Width &&
        mousePos.y >= PosY && mousePos.y <= PosY+Height);
}

/// sets the Label's text and recreates the generic background
/// area with the updated attributes.  This should happen after
/// any change that is graphical.
void cpObject::CreateRects(std::string label)
{
	Label.SetText(label);
	Label.SetColor(textColor);

	backRect.SetPointPosition(0,PosX+2, PosY);
	backRect.SetPointPosition(1,PosX+Width-2, PosY);
	backRect.SetPointPosition(2,PosX+Width, PosY+2);
	backRect.SetPointPosition(3,PosX+Width, PosY+Height-2);
	backRect.SetPointPosition(4,PosX+Width-2, PosY+Height);
	backRect.SetPointPosition(5,PosX+2, PosY+Height);
	backRect.SetPointPosition(6,PosX, PosY+Height-2);
	backRect.SetPointPosition(7,PosX, PosY+2);
	for(int t=0; t<8; t++)
	{
		backRect.SetPointColor(t, backgroundColor);
		backRect.SetPointOutlineColor(t, outlineColor);
	}
}

/// Returns a Vector2f with the object's position coordinates
sf::Vector2f cpObject::GetPosition()
{
	return sf::Vector2f(PosX, PosY);
}

/// Returns a Vector2f with the object's width & height
sf::Vector2f cpObject::GetSize()
{
	return sf::Vector2f(Width, Height);
}

/// Returns the object's background color
sf::Color cpObject::GetBackgroundColor()
{
	return backgroundColor;
}

/// Returns an unsigned long of the style
unsigned long cpObject::GetFontStyle()
{
	return Label.GetStyle();
}

/// Returns the label's text color
sf::Color cpObject::GetLabelColor()
{
	return textColor;
}

/// Returns the Label's text
std::string cpObject::GetLabelText()
{
	return Label.GetText();
}

/// This function draws a dashed line that will make up
/// one side of a dashed rectangle for a focused object
void cpObject::DrawDashedLine(sf::Vector2f start, sf::Vector2f end)
{
	bool on = true;
	sf::Vector2f current = end-start;
	float length = sqrtf(current.x * current.x + current.y * current.y);
	float pos = 0.f;

	current /= length;

	while((pos+2)<=length)
	{
		if(on)
		{
			sf::Vector2f begin = start + pos * current;
			sf::Vector2f end = start + (pos + 2) * current;
			sf::Shape line = sf::Shape::Line(begin.x, begin.y, end.x, end.y, 1.0f,
				outlineColor, 0.f, sf::Color::Black);
			Parent->Draw(line);
		}
		on = !on;
		pos += 2;
	}
}

/// This function draws the four dashed-line sides of a rectangle
/// to show that certain objects have focus
void cpObject::DrawDashedRect(float posx, float posy, float width, float height)
{
	DrawDashedLine(sf::Vector2f(posx, posy), sf::Vector2f(posx+width, posy));
	DrawDashedLine(sf::Vector2f(posx+width, posy), sf::Vector2f(posx+width, posy+height));
	DrawDashedLine(sf::Vector2f(posx+width, posy+height), sf::Vector2f(posx, posy+height));
	DrawDashedLine(sf::Vector2f(posx, posy+height), sf::Vector2f(posx, posy));
}

/// Causes an object to be shown or hidden
void cpObject::Show(bool show)
{
	bShow = show;
}

/// Returns whether or not the object is shown.
bool cpObject::GetShow()
{
	return bShow;
}

/// Forces an object to receive focus
void cpObject::SetFocus(bool focus)
{
	gui->SetObjectFocus(this, focus);
}

/// Allows an object to receive mouse wheel delta info
/// from the GuiContainer
void cpObject::SetUseWheel(bool use)
{
	useWheel = use;
}

void cpObject::SetMouseoverColor(sf::Color color)
{
	mouseoverColor = color;
}

sf::Color cpObject::GetMouseoverColor()
{
	return mouseoverColor;
}


/// *********************************************************************************
/// create cpButton constructor which inherits from
/// cpObject.  Also create all functions for cpButton.
/// Some functions are unique, but most augment functions from cpObject
/// *********************************************************************************
cpButton::cpButton(sf::RenderWindow* parent, cpGuiContainer *GUI, std::string label,
				   float posx, float posy, float width, float height) : cpObject(parent,
				   GUI, label, posx, posy, width, height)
{
	labelInside = true;

	backgroundColor2 = sf::Color(170,170,170);
	backgroundColor2Temp = backgroundColor2;

	TestLabelFit();

	gradient.AddPoint(PosX+2, PosY+Height/2, backgroundColor);
	gradient.AddPoint(PosX+Width-2, PosY+Height/2, backgroundColor);
	gradient.AddPoint(PosX+Width-2, PosY+Height-3, backgroundColor2);
	gradient.AddPoint(PosX+Width-3, PosY+Height-2, backgroundColor2);
	gradient.AddPoint(PosX+3, PosY+Height-2, backgroundColor2);
	gradient.AddPoint(PosX+2, PosY+Height-3, backgroundColor2);
}

cpButton::cpButton() : cpObject(NULL, NULL, ""){}

/// If the button is shown, everything in the button is drawn.
/// If the button has focus, a dashed rectangle is drawn.
void cpButton::Draw()
{
	if(!bShow)
		return;

	Parent->Draw(backRect);
	Parent->Draw(gradient);
	Parent->Draw(Label);

	if(hasFocus && labelInside)
		DrawDashedRect(PosX+2, PosY+2, Width-4, Height-4);

}

/// Sets the button's position in the window
void cpButton::SetPosition(float posx, float posy)
{
	cpObject::SetPosition(posx, posy);
	CreateRects(Label.GetText());
}

/// Sets the width & height dimensions of the button
bool cpButton::SetSize(float width, float height)
{
	if(!cpObject::SetSize(width, height))
		return false;
	CreateRects(Label.GetText());
	return true;
}

/// Loads a font to use on the Label
void cpButton::SetFont(std::string filename, unsigned int size)
{
	cpObject::SetFont(filename, size);
	CreateRects(Label.GetText());
}

/// Sets the button's background color
void cpButton::SetBackgroundColor(sf::Color color)
{
	cpObject::SetBackgroundColor(color);
	int r=0;
	int g=0;
	int b=0;
	if(backgroundColor.r > 70)
		r = backgroundColor.r - 70;
	if(backgroundColor.g > 70)
		g = backgroundColor.g - 70;
	if(backgroundColor.b > 70)
		b = backgroundColor.b - 70;
	backgroundColor2 = sf::Color(r,g,b);
	backgroundColor2Temp = backgroundColor2;
	CreateRects(Label.GetText());
}

/// Sets the Label's font style: Bold, Italic, Underlined, Regular
void cpButton::SetFontStyle(unsigned long TextStyle)
{
	cpObject::SetFontStyle(TextStyle);
	CreateRects(Label.GetText());
}

/// Sets the Label's font size.  The font is reloaded with
/// the new size for optimal quality
void cpButton::SetFontSize(unsigned int size)
{
	cpObject::SetFontSize(size);
	TestLabelFit();
	CreateRects(Label.GetText());
}

/// Sets the text color
void cpButton::SetLabelColor(sf::Color color)
{
	cpObject::SetLabelColor(color);
	CreateRects(Label.GetText());
}

/// Sets the text shown in the button
void cpButton::SetLabelText(std::string text)
{
	cpObject::SetLabelText(text);
	CreateRects(Label.GetText());
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
int cpButton::CheckState(const sf::Input *input)
{
	int state = cpObject::CheckState(input);

	backgroundColor2 = backgroundColor2Temp;

	if(state == CP_ST_MOUSE_IN)
	{
		backgroundColor2 = mouseoverColor;
	}
	if(state == CP_ST_MOUSE_LBUTTON_DOWN)
	{
		backgroundColor2 = sf::Color(44, 98, 139);
	}

	CreateRects(Label.GetText());
	return state;
}

/// Updates the look of all the different parts of the button
void cpButton::CreateRects(std::string label)
{
	cpObject::CreateRects(label);
	if(Label.GetRect().GetWidth() > Width || bDefaultSize)
	{
		Width = Label.GetRect().GetWidth() + 10;
	}
	if(Label.GetRect().GetHeight() > Height || bDefaultSize)
	{
		Height = Label.GetRect().GetHeight() + 10;
	}
	labelX = PosX+(Width/2.f) - Label.GetRect().GetWidth()/2.f;
	labelY = PosY+(Height/2.f) - Label.GetRect().GetHeight()/1.7f;
	Label.SetPosition(labelX, labelY);

	gradient.SetPointPosition(0,PosX+2, PosY+Height/2);
	gradient.SetPointPosition(1,PosX+Width-2, PosY+Height/2);
	gradient.SetPointPosition(2,PosX+Width-2, PosY+Height-3);
	gradient.SetPointPosition(3,PosX+Width-3, PosY+Height-2);
	gradient.SetPointPosition(4,PosX+3, PosY+Height-2);
	gradient.SetPointPosition(5,PosX+2, PosY+Height-3);
	gradient.SetPointColor(0, backgroundColor);
	gradient.SetPointColor(1, backgroundColor);
	for(int t=2; t<6; t++)
		gradient.SetPointColor(t, backgroundColor2);
}

/// Sets whether the button is shown or hidden
void cpButton::Show(bool show)
{
	cpObject::Show(show);
	CreateRects(Label.GetText());
}

/// Tests to see if the Label's text can fit inside the button
void cpButton::TestLabelFit()
{
	if(!labelInside)
		return;
	if(Label.GetRect().GetWidth() > Width || bDefaultSize)
	{
		Width = Label.GetRect().GetWidth() + 10;
		bDefaultSize = true;
	}
	if(Label.GetRect().GetHeight() > Height || bDefaultSize)
	{
		Height = Label.GetRect().GetHeight() + 10;
	}
	labelX = PosX+(Width/2) - Label.GetRect().GetWidth()/2;
	labelY = PosY+(Height/2.f) - Label.GetRect().GetHeight()/1.7f;
	Label.SetPosition(labelX, labelY);
}



/// ********************************************************************
/// Vertical scrollbar
///
/// ********************************************************************
cpScrollBar::cpScrollBar(sf::RenderWindow *parent, cpGuiContainer *GUI,
						 float length, int ViewableLines, int TotalLines,
						 float posx, float posy)
{
	Parent = parent;
	gui = GUI;
	Length = length;
	viewableLines = ViewableLines;
	totalLines = TotalLines;
	PosX = posx;
	PosY = posy;

	if(viewableLines >= totalLines)
		viewableLines = totalLines;
	slLength = Length - 30;
	if(totalLines > 0)
		slLength = (Length-30)*viewableLines/totalLines;

	backgroundColor = sf::Color(240,240,240);
	backgroundColor2 = sf::Color(170,170,170);
	mouseDown = false;
	slDown = false;
	clickGroove = false;
	sliderDrag = false;
	sf::Vector2f mousePos = Parent->ConvertCoords(Parent->GetInput().GetMouseX(),
		Parent->GetInput().GetMouseY());
	mouseY = mousePos.y;
	lineInc = (Length - slLength - 30) / (totalLines-viewableLines);
	clock.Reset();

	upButton = new cpButton(Parent, gui, " ", PosX, PosY, 15,15);
	downButton = new cpButton(Parent, gui, " ", PosX, PosY+Length-15, 15, 15);
	groove = sf::Shape::Rectangle(PosX+1, PosY+15, PosX+14, PosY+Length-15,
		sf::Color(backgroundColor),2.f, sf::Color(200,200,200));

	sliderShape.AddPoint(PosX, PosY+15, backgroundColor);
	sliderShape.AddPoint(PosX+7.5f, PosY+15, backgroundColor);
	sliderShape.AddPoint(PosX+15, PosY+15, backgroundColor2);
	sliderShape.AddPoint(PosX+15, PosY+15+slLength, backgroundColor2);
	sliderShape.AddPoint(PosX+7.5f, PosY+15+slLength, backgroundColor);
	sliderShape.AddPoint(PosX, PosY+15+slLength, backgroundColor);
	sliderShape.SetOutlineWidth(1.f);
	currentPosition = 15;

	upArrow.AddPoint(PosX+7.5f, PosY+2.5f, sf::Color::Black, backgroundColor);
	upArrow.AddPoint(PosX+12, PosY+11, sf::Color::Black, backgroundColor);
	upArrow.AddPoint(PosX+3, PosY+11, sf::Color::Black, backgroundColor);
	upArrow.SetOutlineWidth(1.f);

	downArrow.AddPoint(PosX+3, PosY+Length-11, sf::Color::Black, backgroundColor);
	downArrow.AddPoint(PosX+12, PosY+Length-11, sf::Color::Black, backgroundColor);
	downArrow.AddPoint(PosX+7.5f, PosY+Length-2.5f, sf::Color::Black, backgroundColor);
	downArrow.SetOutlineWidth(1.f);

	MoveLines();

	gui->Unregister(upButton);
	gui->Unregister(downButton);
}

cpScrollBar::cpScrollBar(){}

cpScrollBar::~cpScrollBar()
{
	delete downButton;
	downButton = NULL;
	delete upButton;
	upButton = NULL;
}

/// Draws the different parts that make up a scrollbar
/// if the slider is long enough, draw the "grab lines"
/// in the center
void cpScrollBar::Draw()
{
	Parent->Draw(groove);
	upButton->Draw();
	Parent->Draw(upArrow);
	downButton->Draw();
	Parent->Draw(downArrow);
	Parent->Draw(sliderShape);
	if(slLength > 35)
	{
		Parent->Draw(line1);
		Parent->Draw(line2);
		Parent->Draw(line3);
	}
}

/// This controls the various ways to move the slider
/// and returns an integer that is the position of the slider.
/// This position represents the line to start with.
int cpScrollBar::CheckState(const sf::Input *input)
{
	int upState = upButton->CheckState(input);
	int dnState = downButton->CheckState(input);
	sf::Vector2f mousePos = Parent->ConvertCoords(input->GetMouseX(), input->GetMouseY());
	if(CheckMouseSlider(input) == CP_ST_MOUSE_LBUTTON_DOWN && !mouseDown)
	{
		mouseY = mousePos.y;
		sliderDrag = true;
		mouseDown = true;
	}
	if(sliderDrag && currentPosition > 14 && currentPosition < PosY + Length
		- slLength - 14)
	{
		float temp = mousePos.y;
		currentPosition = currentPosition + temp - mouseY;
		CheckSliderValid();
		MoveLines();
		mouseY = temp;
	}
	if(CheckMouseSlider(input) == CP_ST_MOUSE_LBUTTON_RELEASED ||
		(!input->IsMouseButtonDown(sf::Mouse::Left) && sliderDrag))
	{
		sliderDrag = false;
		mouseDown = false;
	}

	if(upState == CP_ST_MOUSE_LBUTTON_DOWN && !mouseDown && clock.GetElapsedTime() > 0.1f)
	{
		mouseDown = true;
		MoveSlider(-1);
		clock.Reset();
		clock2.Reset();
	}

	if(upState == CP_ST_MOUSE_LBUTTON_DOWN && mouseDown && clock.GetElapsedTime() > 0.7f
		&& clock2.GetElapsedTime() > 0.1f && currentPosition > 14)
	{
		MoveSlider(-1);
		clock2.Reset();
	}
	if(upState == CP_ST_MOUSE_LBUTTON_RELEASED || dnState == CP_ST_MOUSE_LBUTTON_RELEASED)
		mouseDown = false;

	if(dnState == CP_ST_MOUSE_LBUTTON_DOWN && !mouseDown && clock.GetElapsedTime() > 0.1f)
	{
		mouseDown = true;
		MoveSlider(1);
		clock.Reset();
		clock2.Reset();
	}

	if(dnState == CP_ST_MOUSE_LBUTTON_DOWN && mouseDown && clock.GetElapsedTime() > 0.7f
		&& clock2.GetElapsedTime() > 0.1f && currentPosition > 14)
	{
		MoveSlider(1);
		// I don't know what forrestcup was doing here
		// but these aren't even used so I'm commenting out
		/*
		float c1 = clock.GetElapsedTime();
		float c2 = clock2.GetElapsedTime();
		*/
		clock2.Reset();
	}
	if(input->IsMouseButtonDown(sf::Mouse::Left) && !mouseDown && !sliderDrag &&
		CheckMouseEntered(input))
		clickGroove = true;

	if(!input->IsMouseButtonDown(sf::Mouse::Left) && clickGroove)
	{
		int d = -1;
		if(mousePos.y > PosY+currentPosition+slLength)
			d = 1;
		for(int t=0; t < viewableLines; t++)
			MoveSlider(d);
		clickGroove = false;
	}

	int position = 0;
	if(Length-30-slLength > 0)
	{
		position = int((currentPosition - 15)/
			(Length-30 - slLength)*(totalLines-viewableLines) + 0.5);
	}

	CreateRects();

	return position;
}

/// This moves the "grab lines" relative to the slider
void cpScrollBar::MoveLines()
{
	float temp = PosY+currentPosition + slLength / 2;
	line1 = sf::Shape::Line(PosX+3, temp-4, PosX+12, temp-4,
		2.f, backgroundColor2, 1.f, backgroundColor);
	line2 = sf::Shape::Line(PosX+3, temp, PosX+12, temp,
		2.f, backgroundColor2, 1.f, backgroundColor);
	line3 = sf::Shape::Line(PosX+3, temp+4, PosX+12, temp+4,
		2.f, backgroundColor2, 1.f, backgroundColor);
}

/// Moves the slider by the amount of one line increment.
/// direction = 1 for down & -1 for up
void cpScrollBar::MoveSlider(short direction)
{
	currentPosition = currentPosition + lineInc * direction;

	CheckSliderValid();

	MoveLines();
}

/// If the slider is too far down, or too far up,
/// put it within the limits.
void cpScrollBar::CheckSliderValid()
{
	if(currentPosition > Length - slLength - 15)
		currentPosition = Length - slLength - 15;
	if(currentPosition < 15)
		currentPosition = 15;
}

/// Checks to see if mouse pointer is inside control.  Returns true if it is.
bool cpScrollBar::CheckMouseEntered(const sf::Input *input)
{
	sf::Vector2f mousePos = Parent->ConvertCoords(input->GetMouseX(), input->GetMouseY());
	return (mousePos.x >= PosX && mousePos.x <= PosX+15 &&
		mousePos.y >= PosY && mousePos.y <= PosY+Length);
}

/// This checks the state of the mouse inside the shape
/// that makes up the slider and returns an integer that
/// represents the mouse activity
int cpScrollBar::CheckMouseSlider(const sf::Input *input)
{
	sf::Vector2f mousePos = Parent->ConvertCoords(input->GetMouseX(), input->GetMouseY());
	if(mousePos.x >= PosX && mousePos.x <= PosX+15 &&
		mousePos.y >= PosY+currentPosition &&
		mousePos.y <= PosY+currentPosition+slLength)
	{
		if(input->IsMouseButtonDown(sf::Mouse::Left))
		{
			slDown = true;
			return CP_ST_MOUSE_LBUTTON_DOWN;
		}
	}
	if(!input->IsMouseButtonDown(sf::Mouse::Left) && slDown)
	{
		slDown = false;
		return CP_ST_MOUSE_LBUTTON_RELEASED;
	}

	return CP_ST_NONE;
}

/// Recompute the slider based on a new number of viewable lines
/// Vs. total lines
void cpScrollBar::Repopulate(int ViewableLines, int TotalLines)
{
	viewableLines = ViewableLines; //14
	totalLines = TotalLines; // 10
	if(viewableLines >= totalLines)
		viewableLines = totalLines; // vl = 10
	slLength = Length - 30;
	if(totalLines > 0)
		slLength = (Length-30)*viewableLines/totalLines;
	lineInc = 0;
	if(totalLines > viewableLines)
		lineInc = (Length - slLength - 30) / (totalLines-viewableLines);
	CreateRects();
}

/// Sets the length of the scroll bar and recomputes the slider
/// based on the new length and new number of viewable lines
void cpScrollBar::SetSize(float length, int ViewableLines)
{
	viewableLines = (int)ViewableLines;
	Length = length;
	if(viewableLines > totalLines-1)
	{
		viewableLines = totalLines;
	}
	slLength = Length - 30;
	if(totalLines > 0)
		slLength = (Length-30)*viewableLines/totalLines;

	currentPosition = 15;
	lineInc = 0;
	if(totalLines > viewableLines)
		lineInc = (Length - slLength - 30) / (totalLines-viewableLines);
	CreateRects();
}

/// Updates the look of the scrollbar
void cpScrollBar::CreateRects()
{
	groove = sf::Shape::Rectangle(PosX+1, PosY+15, PosX+14, PosY+Length-15,
		sf::Color(backgroundColor),2.f, sf::Color(200,200,200));

	sliderShape.SetPointPosition(0,PosX, PosY+currentPosition);
	sliderShape.SetPointPosition(1,PosX+7.5f, PosY+currentPosition);
	sliderShape.SetPointPosition(2,PosX+15, PosY+currentPosition);
	sliderShape.SetPointPosition(3,PosX+15, PosY+currentPosition+slLength);
	sliderShape.SetPointPosition(4,PosX+7.5f, PosY+currentPosition+slLength);
	sliderShape.SetPointPosition(5,PosX, PosY+currentPosition+slLength);
	sliderShape.SetOutlineWidth(1.f);
	for(int t=0; t < 6; t++)
		sliderShape.SetPointOutlineColor(t, sf::Color::Black);
	MoveLines();

	upButton->SetPosition(PosX, PosY);
	downButton->SetPosition(PosX, PosY+Length-15);

	upArrow.SetPointPosition(0,PosX+7.5f, PosY+2.5f);
	upArrow.SetPointPosition(1,PosX+12, PosY+11);
	upArrow.SetPointPosition(2,PosX+3, PosY+11);

	downArrow.SetPointPosition(0,PosX+3, PosY+Length-11);
	downArrow.SetPointPosition(1,PosX+12, PosY+Length-11);
	downArrow.SetPointPosition(2,PosX+7.5f, PosY+Length-2.5f);
}

/// Sets new coordinates for the position of the scrollbar
void cpScrollBar::SetPosition(float posx, float posy)
{
	currentPosition = 15;
	PosX = posx;
	PosY = posy;
	CreateRects();
}


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
#include "cpSelection-DropDownBox.h"
using namespace cp;


/// *************************************************************************
/// This class is a selection box.  Choices can be added, and
/// those choices will be listed in the box.  A selection can
/// be made either by clicking on the choice, or with the keyboard
/// *************************************************************************
cpSelectionBox::cpSelectionBox(sf::RenderWindow *parent, cpGuiContainer *GUI, 
							   float posx, float posy, float width, float height) :
cpObject(parent, GUI, "", posx, posy, width, height)
{
	currentSelection = 0;
	currentStartingLine = 0;
	backgroundColor = sf::Color::White;
	CreateRects("");
	Label.SetText("Jason Cupp");
	fontSize = textFont->GetCharacterSize();
	viewableLines = int((Height-5)/(fontSize+5));
	mouseDown = false;
	keyDown = false;
	clock.Reset();
	
	SetUseWheel(true);

	scrollbar = new cpScrollBar(Parent, gui, Height, viewableLines, choices.size(),
		PosX+Width-15, PosY);


	gui->AcceptsKeys(true, false);
}

cpSelectionBox::cpSelectionBox() : cpObject(NULL, NULL, "") {}

cpSelectionBox::~cpSelectionBox()
{
	delete scrollbar;
	scrollbar = NULL;
}

/// Updates the look of the selection box and draws
/// all of the choices with the current selection highlighted
void cpSelectionBox::Draw()
{
	if(!bShow)
		return;

	Parent->Draw(backRect);
	short barWidth = 0;
	if((unsigned int)viewableLines < choices.size())
	{
		scrollbar->Draw();
		barWidth = 15;
	}

	if(choices.size() > 0)
	{
		int max = choices.size();
		if(choices.size() > (unsigned int)viewableLines)
			max = viewableLines;
		if(currentSelection >= currentStartingLine && currentSelection < 
			currentStartingLine + max)
		{
			selBox = sf::Shape::Rectangle(PosX, choices[currentSelection].GetPosition().y,
				PosX+Width-barWidth, choices[currentSelection].GetPosition().y+fontSize+5,
				sf::Color::Blue);
			Parent->Draw(selBox);
		}
		for(int t=0; t < max; t++)
		{
			choices[t+currentStartingLine].SetPosition(PosX+10, PosY+((Height-viewableLines*
				(fontSize+5))/2)+((fontSize+5)*t));
			if(currentSelection == t+currentStartingLine)
				choices[t+currentStartingLine].SetColor(sf::Color::White);
			else
				choices[t+currentStartingLine].SetColor(textColor);
			Parent->Draw(choices[t+currentStartingLine]);
		}
	}
}

/// Function to add a choice to the choice list.
/// The new addition will be placed on the bottom of the list.
void cpSelectionBox::AddChoice(std::string choice)
{
	sf::String temp = Label;
	temp.SetText(choice);
	choices.push_back(temp);
	CheckTextWidth();
	scrollbar->Repopulate(viewableLines, choices.size());
}

/// Removes the last choice from the list
void cpSelectionBox::RemoveLastChoice()
{
	if(choices.size() <= 0)
		return;
	choices.pop_back();
	if(currentStartingLine > 0)
		currentStartingLine--;
	if(unsigned(currentSelection) >= choices.size())
		currentSelection -= 1;
	scrollbar->Repopulate(viewableLines, choices.size());
	scrollbar->MoveSlider(0);
}

/// Makes sure the text of all choices fits in the selection box
/// BEWARE: if the text is too wide for the box, it will be 
/// truncated never to be seen again.
void cpSelectionBox::CheckTextWidth()
{
	if(choices.size() < 1)
		return;
	std::string temp;
	for(unsigned int t=0; t < choices.size(); t++)
	{
		temp = choices[t].GetText();
		while(choices[t].GetRect().GetWidth() > Width-30)
		{
			temp.erase(temp.length()-1);
			choices[t].SetText(temp);
		}
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
/// 
/// In this class, the mouse wheel delta information is taken from 
/// the GuiContainer to update the scrollbar
int cpSelectionBox::CheckState(const sf::Input *input)
{
	int state = cpObject::CheckState(input);
	sf::Vector2f mousePos = Parent->ConvertCoords(input->GetMouseX(), input->GetMouseY()); 

	if(state == CP_ST_NONE)
	{
		if(hasFocus)
			outlineColor = sf::Color(90,90,230);
		else
			outlineColor = sf::Color::Black;
	}
	else
		outlineColor = outlineColorTemp;

	currentStartingLine = scrollbar->CheckState(input);
	if(wheelDelta == 1)
	{
		scrollbar->MoveSlider(-1);
		wheelDelta = 0;
	}
	if(wheelDelta == -1)
	{
		scrollbar->MoveSlider(1);
		wheelDelta = 0;
	}

	if(input->IsKeyDown(sf::Key::Down) && (unsigned int)currentSelection < choices.size()-1 
		&& hasFocus && clock.GetElapsedTime() > .3f)
	{
		currentSelection++;
		if(currentSelection >= currentStartingLine+viewableLines)
		{
			scrollbar->MoveSlider(1);
			currentStartingLine++;
		}
		clock.Reset();
	}
	if(input->IsKeyDown(sf::Key::Up) && currentSelection > 0 && 
		hasFocus && clock.GetElapsedTime() > .3)
	{
		currentSelection--;
		if(currentSelection < currentStartingLine)
		{
			scrollbar->MoveSlider(-1);
			currentStartingLine--;
		}
		clock.Reset();
	}


	if(input->IsMouseButtonDown(sf::Mouse::Left))
	{
		if(mousePos.x > PosX && mousePos.x < PosX+Width-15 && mousePos.y > PosY && 
			mousePos.y < PosY+Height)
			mouseDown = true;
	}

	if(input->IsMouseButtonDown(sf::Mouse::Left) && mouseDown)
	{
		unsigned int max = viewableLines;
		if(max > choices.size())
			max = choices.size();
		for(unsigned int t=0; t < max; t++)
		{
			if(mousePos.y >= choices[t+currentStartingLine].GetPosition().y &&
				mousePos.y <= choices[t+currentStartingLine].GetPosition().y+fontSize+5)
				currentSelection = t+currentStartingLine;
		}
		mouseDown = false;
	}
	CreateRects("");
	return state;
}

/// Sets the current selection. An integer representing
/// the index of the selection starting at 0.
void cpSelectionBox::SetSelection(int sel)
{
	currentSelection = sel;
}

/// Returns an integer starting at 0 representing the
/// current selection's index.
int cpSelectionBox::GetSelection()
{
	return currentSelection;
}

/// Sets the width & height dimensions of the selection box.
/// BEWARE: if you make the box smaller than the width of your
/// text, your text will be truncated and lost
bool cpSelectionBox::SetSize(float width, float height)
{
	Width = width;
	Height = height;
	CreateRects(Label.GetText());
	viewableLines = int((Height-5)/(fontSize+5));
	scrollbar->SetSize(Height, viewableLines);
	scrollbar->SetPosition(PosX+Width-15, PosY);
	return false;
}

/// Updates the look of the selection box
void cpSelectionBox::CreateRects(std::string label)
{
	cpObject::CreateRects(label);
}

/// Sets the font's style: Bold, Italic, Underlined, Regular
void cpSelectionBox::SetFontStyle(unsigned long TextStyle)
{
	cpObject::SetFontStyle(TextStyle);
	for(unsigned int t=0; t < choices.size(); t++)
		choices[t].SetStyle(TextStyle);
	CheckTextWidth();
}

/// Sets the font's size.  The current font is reloaded
/// with the new size for better quality.
/// BEWARE: if resizing the font makes the text too wide for
/// your box, it will be truncated and lost
void cpSelectionBox::SetFontSize(unsigned int size)
{
	cpObject::SetFontSize(size);
	for(unsigned int t=0; t < choices.size(); t++)
	{
		choices[t].SetFont(*textFont);
		choices[t].SetSize(Label.GetSize());
	}
	fontSize = size;
	viewableLines = int((Height-5)/(fontSize+5));
	CheckTextWidth();
	scrollbar->SetSize(Height, viewableLines);
}

/// Loads a font for the text. Use a filename of "Default" for SFML's
/// default font.
/// BEWARE: if the size the font makes the text too wide for
/// your box, it will be truncated and lost
void cpSelectionBox::SetFont(std::string filename, unsigned int size)
{
	cpObject::SetFont(filename, size);
	for(unsigned int t=0; t < choices.size(); t++)
	{
		choices[t].SetFont(*textFont);
		choices[t].SetSize(float(textFont->GetCharacterSize()));
	}
	fontSize = size;
	viewableLines = int((Height-5)/(fontSize+5));
	CheckTextWidth();
	scrollbar->SetSize(Height, viewableLines);
}

/// Sets the coordinates for the position of the selection box
void cpSelectionBox::SetPosition(float posx, float posy)
{
	cpObject::SetPosition(posx, posy);
	scrollbar->SetPosition(posx+Width-15, posy);
	CreateRects("");
}

/// Sets the background color of the selection box.
void cpSelectionBox::SetBackgroundColor(sf::Color color)
{
	cpObject::SetBackgroundColor(color);
	CreateRects("");
}


// **********************************************************************
// Drop down box
// **********************************************************************
cpDropDownBox::cpDropDownBox(sf::RenderWindow *parent, cpGuiContainer *GUI, 
							 std::string label, float posx, float posy, 
							 float width, float height) : cpObject(parent,
							 GUI, label, posx, posy, width, height)
{
	backgroundColor = sf::Color::White;
	textColor = sf::Color(180,180,180);
	tempTextColor = sf::Color::Black;
	maxDepth = 100;
	depth = Height;

	CreateRects(label);
	CheckTextWidth();

	downBtn = new cpButton(Parent, gui, " ", PosX+Width-Height, PosY, Height, depth);
	btnBackground = downBtn->GetBackgroundColor();

	downArrow.AddPoint(PosX+Width-Height+Height/3, PosY+Height/3, sf::Color::Black, btnBackground);
	downArrow.AddPoint(PosX+Width-Height+Height*2/3, PosY+Height/3, sf::Color::Black, btnBackground);
	downArrow.AddPoint(PosX+Width-Height+Height/2, PosY+Height*2/3, sf::Color::Black, btnBackground);
	downArrow.SetOutlineWidth(1.f);

	sBox = new cpSelectionBox(Parent, gui, PosX, PosY+Height, Width, Height);
	if(PosY+Height+sBox->GetSize().y > Parent->GetHeight())
		sBox->SetPosition(PosX, PosY-sBox->GetSize().y);
	sBox->Show(false);

	SetFont(Font, (unsigned int)Height-4);


	gui->Unregister(downBtn);
}

cpDropDownBox::~cpDropDownBox()
{
	delete downBtn;
	downBtn = NULL;
	delete sBox;
	sBox = NULL;
}

/// Updates the look of the dropdown box.
void cpDropDownBox::CreateRects(std::string label)
{
	backRect = sf::Shape::Rectangle(PosX, PosY, PosX+Width, PosY+Height, backgroundColor,
		1.f, outlineColor);
	Label.SetPosition(PosX+4, PosY-Height/14);
	Label.SetSize(Height-4);
	Label.SetColor(textColor);
}

/// Moves the Down button and it's arrow to a new position.
void cpDropDownBox::MoveButton()
{
	downArrow.SetPointPosition(0,PosX+Width-Height+Height/3, PosY+Height/3);
	downArrow.SetPointPosition(1,PosX+Width-Height+Height*2/3, PosY+Height/3);
	downArrow.SetPointPosition(2,PosX+Width-Height+Height/2, PosY+Height*2/3);
	downBtn->SetPosition(PosX+Width-Height, PosY);
}

/// Draws the dropdown box. If it is activated, it also
/// draws the dropdown box's selection box.
void cpDropDownBox::Draw()
{
	if(!bShow)
		return;
	Parent->Draw(backRect);
	Parent->Draw(Label);
	downBtn->Draw();
	Parent->Draw(downArrow);
	sBox->Draw();
}

/// Checks to make sure the text fits within the width of the box.
/// BEWARE: if the text is too wide, it will be truncated and lost.
void cpDropDownBox::CheckTextWidth()
{
	std::string temp;
	temp = Label.GetText();
	while(Label.GetRect().GetWidth() > Width-15-Height)
	{
		temp.erase(temp.length()-1);
		Label.SetText(temp);
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
/// 
/// The Selection Box part of the drop down box also recieves
/// mouse wheel delta information from the GuiContainer.
int cpDropDownBox::CheckState(const sf::Input *input)
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


	int btnState = downBtn->CheckState(input);
	int sboxState = sBox->CheckState(input);
	if(sboxState == CP_ST_MOUSE_LBUTTON_RELEASED && sBox->GetShow())
	{
		Label.SetText(sBox->choices[sBox->GetSelection()].GetText());
		sBox->Show(false);
		gui->SetObjectFocus(this, true);
		textColor = tempTextColor;
	}

	if(btnState == CP_ST_MOUSE_LBUTTON_RELEASED || state == 
		CP_ST_MOUSE_LBUTTON_RELEASED)
	{
		sBox->Show(!sBox->GetShow());
		if(sBox->GetShow())
			gui->SetObjectFocus(sBox, true);
	}

	CreateRects(Label.GetText());
	return state;
}

/// Add a choice to the selections.  The new choice
/// will be added at the end of the list.
void cpDropDownBox::AddChoice(std::string choice)
{
	if(depth + Height <= maxDepth)
	{
		sBox->SetSize(Width, depth+Height);
		depth += Height;
	}

	sBox->AddChoice(choice);
	sBox->SetFontSize((unsigned int)Height-4);

	if(PosY+Height+depth > Parent->GetHeight())
		sBox->SetPosition(PosX, PosY-depth);
}

/// Removes the last choice in the list of selections.
void cpDropDownBox::RemoveLastChoice()
{
	sBox->RemoveLastChoice();
}


/// Not available.
/// Font size is determined by size of drop down box.
void cpDropDownBox::SetFontSize(unsigned int size)
{
}

/// Sets the text's font style: Bold, Italic, Underlined, Regular
/// Uses SFML's text styles.
void cpDropDownBox::SetFontStyle(unsigned long TextStyle)
{
	cpObject::SetFontStyle(TextStyle);
	CheckTextWidth();
	sBox->SetFontStyle(TextStyle);
}

/// The font can be changed, but the font size will
/// remain relative to the size of the drop down box
void cpDropDownBox::SetFont(std::string filename, unsigned int size)
{
	cpObject::SetFont(filename, (unsigned int)Height-4);
	Label.SetSize(Height-4);
	if(!sBox->choices.empty())
	{
		for(unsigned int t=0; t < sBox->choices.size(); t++)
		{
			sBox->choices[t].SetFont(*textFont);
			sBox->choices[t].SetSize(Height-4);
		}
	}
}

/// Sets the coordinates for the position of the dropdown box
void cpDropDownBox::SetPosition(float posx, float posy)
{
	cpObject::SetPosition(posx, posy);
	MoveButton();
	sBox->SetPosition(PosX, PosY+Height);
	if(PosY+Height+sBox->GetSize().y > Parent->GetHeight())
		sBox->SetPosition(PosX, PosY-sBox->GetSize().y);
	CreateRects(Label.GetText());
}

/// Sets the background color of the dropdown box & its
/// selection box.
void cpDropDownBox::SetBackgroundColor(sf::Color color)
{
	cpObject::SetBackgroundColor(color);
	sBox->SetBackgroundColor(color);
	CreateRects(Label.GetText());
}

/// Sets the width & height dimensions of the drop down box.
/// doesn't change the depth of the selection box area.
/// Also resizes the text font to fit the new height.
///
/// BEWARE: if resizing causes your text to be too wide to fit,
/// it will be trunctated and lost.
bool cpDropDownBox::SetSize(float width, float height)
{
	Width = width;
	Height = height;
	SetFont(Font, (unsigned int)Height-4);
	CheckTextWidth();
	downBtn->SetSize(Height, Height);
	MoveButton();
	CreateRects(Label.GetText());
	sBox->SetSize(Width, sBox->GetSize().y);
	return false;
}

/// The Selection Box area is based on the number of choices.
/// This sets the maximum depth.  After this depth has been reached,
/// a scroll bar will be used.
void cpDropDownBox::SetMaxDepth(float depth)
{
	maxDepth = depth;
}

/// Sets which choice is the current selection.
/// Accepts an integer starting with 0 that represents
/// the selection's index.
void cpDropDownBox::SetSelection(int sel)
{
	sBox->SetSelection(sel);
}

/// Returns the index of the current selection starting
/// with 0.
int cpDropDownBox::GetSelection()
{
	return sBox->GetSelection();
}

/// Sets the color of the text.
void cpDropDownBox::SetLabelColor(sf::Color color)
{
	cpObject::SetLabelColor(color);
	sBox->SetLabelColor(color);
	tempTextColor = color;
}

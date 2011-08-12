#include <cstdio>
#include <cstring>
#include <string>
#include "app.h"
#include "message_box.h"

//int MessageBox::numberOfButtons = 0;
//int MessageBox::buttonPressed = 0;

//ToolBar *MessageBox::frame = NULL;
//Label *MessageBox::textLabel = NULL;
//TextBox *MessageBox::textBox = NULL;
//Button *MessageBox::buttons[3] = {NULL, NULL, NULL};

//char *MessageBox::title = NULL;
//char *MessageBox::text = NULL;

MessageBox::MessageBox() {
}

MessageBox::~MessageBox() {
}

MessageBox::MessageBox(const char *theTitle, const char *theText, int theNumberOfButtons, bool haveTextBox) {
	init(theTitle, theText, theNumberOfButtons, haveTextBox);
}

void MessageBox::init(const char *theTitle, const char *theText, int theNumberOfButtons, bool haveTextBox) {
	title = theTitle;
	text = theText;
	numberOfButtons = theNumberOfButtons;

	buttonPressed = 0;
	
	// Set up widgets
	
	// set up frame
	int frameX = (app->windowWidth - strlen(theText) - 2) / 2;
	int frameY = (app->windowHeight - 8) / 2;

	frame = new ToolBar(frameX, frameY, title.c_str());

	// the text of the message
	textLabel = new Label(0, 0, text.c_str());
	
	int textBoxWidth = 30;
	textBox = new TextBox(0, 0, textBoxWidth, 1024, "", "");

	HBox *hbox = new HBox(0, 0, 1);

	// set up buttons
	buttons[0] = new Button("Yes",
			NULL,
			MessageBoxButtonCbk, 
			new MessageBoxButtonCbkInfo(this, 1));
	hbox->addWidget(buttons[0]);

	buttons[1] = new Button("No",
			NULL,
			MessageBoxButtonCbk, 
			new MessageBoxButtonCbkInfo(this, 2));
	hbox->addWidget(buttons[1]);

	buttons[2] = new Button("Cancel",
			NULL,
			MessageBoxButtonCbk, 
			new MessageBoxButtonCbkInfo(this, 3));
	hbox->addWidget(buttons[2]);

	// Put the text label and buttons in a vbox
	VBox *vbox = new VBox(0, 0, 1);

	customWidgetContainer = new VBox(0, 0, 0);
	customWidgetContainer->setVisible(false);

	vbox->addWidget(textLabel);
	vbox->addWidget(customWidgetContainer);
	vbox->addWidget(textBox);
	vbox->addWidget(hbox);

	frame->addWidget(vbox);

	if(haveTextBox) 
		textBox->setVisible(true);
	else
		textBox->setVisible(false);

	// Make all buttons invisble
	buttons[0]->setVisible(false);
	buttons[1]->setVisible(false);
	buttons[2]->setVisible(false);

	// depending on the number of buttons we show the buttons and set their names
	switch(numberOfButtons) {
		case 1:
			buttons[0]->setLabel("OK");
			buttons[0]->setVisible(true);
			break;

		case 2:
			buttons[0]->setLabel("OK");
			buttons[0]->setVisible(true);

			buttons[1]->setLabel("Cancel");
			buttons[1]->setVisible(true);
			break;

		case 3:
			buttons[0]->setLabel("Yes");
			buttons[0]->setVisible(true);

			buttons[1]->setLabel("No");
			buttons[1]->setVisible(true);

			buttons[2]->setLabel("Cancel");
			buttons[2]->setVisible(true);
			break;
	}
	frame->setVisible(false);
}

void MessageBox::setButtonText(int buttonNumber, const char *text) {
	buttons[buttonNumber - 1]->setLabel(text);
}


void MessageBox::addCustomWidget(Widget *wid) {
	customWidgetContainer->setVisible(true);
	customWidgetContainer->addWidget(wid);
}

void MessageBox::show() {
	TCODConsole *con = TCODConsole::root;

	Widget::setConsole(con);

	frame->setVisible(true);
	if(textBox->isVisible()) 
		Widget::keyboardFocus = textBox;

	TCOD_key_t key;

	// We loop until the user presses a button (or presses escape)
	while(buttonPressed == 0) {
		key = TCODConsole::checkForKeypress();
		// Warning: for the next line to work we needed to change gui/widget.hpp
		Widget::mouse = TCODMouse::getStatus();

		frame->computeSize();
		frame->update(key);
		frame->render();

		con->flush();

		if(key.vk == TCODK_ESCAPE)
			break;

		// Pressing enter is equivalent to pressing button 1
		if(key.vk == TCODK_ENTER) {
			buttonPressed = 1;
			break;
		}
	}

	frame->setVisible(false);
	Widget::focus = false; // Not having this line causes a bug where the focus stays true
}

int MessageBox::getButtonPressed() {
	return buttonPressed;
}

void MessageBox::setButtonPressed(int buttonNumber) {
	buttonPressed = buttonNumber;
}

const char *MessageBox::getString() {
	if(strlen(textBox->getValue()) > 0)
		return textBox->getValue();
	return NULL;
}

void MessageBox::setString(const char *str) {
	textBox->setText(str);
}

//--------------- Callback stuff

MessageBoxButtonCbkInfo::MessageBoxButtonCbkInfo(MessageBox *msg, int bn) {
	messageBox = msg;
	buttonNumber = bn;
}

void MessageBoxButtonCbk(Widget *wid, void *data) {
	MessageBoxButtonCbkInfo *info = reinterpret_cast<MessageBoxButtonCbkInfo*>(data);

	info->messageBox->setButtonPressed(info->buttonNumber);
}


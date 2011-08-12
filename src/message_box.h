#ifndef ASCII_PAINT_MSG_BOX_H
#define ASCII_PAINT_MSG_BOX_H

#include <string>
#include "gui/gui.hpp"
#include "app_user.h"

class MessageBox : public AppUser {
	public:
		MessageBox();
		MessageBox(const char *title, const char *text, int numberOfButtons, bool haveTextBox = false);
		virtual ~MessageBox();

		void setButtonText(int buttonNumber, const char *text);
		void addCustomWidget(Widget *wid);
		void show();
		//void show(char *title, char *text, int numberOfButtons, bool haveTextBox = false);
		int getButtonPressed();
		void setButtonPressed(int buttonNumber);
		const char *getString();
		void setString(const char *str);

	private:
		void init(const char *title, const char *text, int numberOfButtons, bool haveTextBox = false);
		int numberOfButtons;
		int buttonPressed;

		ToolBar *frame;
		Label *textLabel;
		TextBox *textBox;
		VBox *customWidgetContainer;
		Button *buttons[3];
		std::string title;
		std::string text;
};

struct MessageBoxButtonCbkInfo {
	MessageBoxButtonCbkInfo(MessageBox *msg, int bn);

	MessageBox *messageBox;
	int buttonNumber;
};

void MessageBoxButtonCbk(Widget *wid, void *data);

#endif


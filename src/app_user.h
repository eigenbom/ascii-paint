#ifndef ASCII_PAINT_APP_USER_H
#define ASCII_PAINT_APP_USER_H

class App;

class AppUser {
	public:
		static void setApp(App *app);

		static App *app;
};

#endif

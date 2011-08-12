#include "app_user.h"

App* AppUser::app = 0;

void AppUser::setApp(App *a) {
	app = a;
}


#ifndef ENGINE_GUI_MENU_H
#define ENGINE_GUI_MENU_H

#include "../core/gui/gui_obj.h"

gui_object_p Gui_BuildMainMenu();
gui_object_p Gui_BuildLoadGameMenu();
gui_object_p Gui_BuildSaveGameMenu();
gui_object_p Gui_BuildNewGameMenu();
gui_object_p Gui_BuildControlsMenu();
gui_object_p Gui_BuildStatisticsMenu();

#endif

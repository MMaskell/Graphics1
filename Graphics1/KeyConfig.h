#pragma once
#include "Globals.h"

using namespace std;
typedef map<string, int> keyMap;

namespace KeyConfig {
	extern keyMap keyBindings;
	extern GLFWwindow* win;
	void loadBindings();
	void saveBindings();
	bool isDown(string key);
}
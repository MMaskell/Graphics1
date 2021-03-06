#include "MainMenu.h"
#include "LevelSelect.h"
#include "LevelEditor.h"
#include "SettingsMenu.h"
#include "GradButton.h"

#define MENU_BUTTON_HEIGHT 50

MainMenu::MainMenu() {
	Button* play = new GradButton();
	play->setLabel("Play");
	auto playCall = [](BaseState* s) {
		newState = new LevelSelect();
	};
	play->setCallback(playCall);
	buttons.push_back(play);
	Button* editor = new GradButton();
	editor->setLabel("Level Editor");
	auto editorCall = [](BaseState* s) {
		newState = new LevelEditor();
	};
	editor->setCallback(editorCall);
	buttons.push_back(editor);
	Button* settings = new GradButton();
	settings->setLabel("Settings");
	auto settingsCall = [](BaseState* s) {
		newState = new SettingsMenu();
	};
	settings->setCallback(settingsCall);
	buttons.push_back(settings);
	Button* quit = new GradButton();
	quit->setLabel("Quit");
	auto quitCall = [](BaseState* s) {
		exit(EXIT_SUCCESS);
	};
	quit->setCallback(quitCall);
	buttons.push_back(quit);
}


MainMenu::~MainMenu() {
}


// Updates the menu
void MainMenu::update() {
}


// Called when the window is resized
void MainMenu::resizeEvent(GLFWwindow* window, int width, int height) {
	int i = 0;
	int n = buttons.size();
	for (Button* b : buttons) {
		int h = (int)fontLarge.h * 2;
		b->setWidth((int)(sWidth * 0.5 + h * i));
		b->setY(h * (n - i));
		b->setHeight((int)(h*0.9));
		i++;
	}
}
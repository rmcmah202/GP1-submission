#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>
#include <chrono>

// Game specific includes
#include "asteroidsGame.h"


using namespace std;

class cGame
{
public:
	cGame();

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;
	bool loop;

	// Sprites for displaying background and rocket textures
	cSprite spriteBkgd;
	cRocket thePlayer;
	cAsteroid theAsteroid;
	cBullet theBullet;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	vector<cAsteroid*> roomPositionArray;
	vector<cAsteroid*> theAsteroids;
	vector<cBullet*> theBullets;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextList;
	vector<LPCSTR> gameTextNames;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Create vector array of button textures
	vector<LPCSTR> btnNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector <cButton> theButtons;
	// Game objects
	// Define the elements and there position in/on the array/map
	int renderWidth, renderHeight;
	int theScore;
	string theSScore;
	gameState theGameState;
	btnTypes theBtnType;
	SDL_Point theAreaClicked;
	cTexture* tempTextTexture;
	SDL_Rect pos;
	FPoint scale;

	int xPos[16];
	int yPos[16];

	int rightWall;
	int leftWall;
	int bottomWall;
	int topWall;

	int DoorOneDown;
	int DoorTwoDown;
	int DoorThreeDown;
	int DoorFourDown;
	int DoorFiveDown;
	int DoorSixDown;
	int DoorOneLeft;
	int DoorTwoLeft;
	int DoorThreeLeft;
	int DoorFourLeft;
	int DoorFiveLeft;
	int DoorSixLeft;
	int DoorOneUp;
	int DoorTwoUp;
	int DoorThreeUp;
	int DoorFourUp;
	int DoorFiveUp;
	int DoorSixUp;
	int DoorOneRight;
	int DoorTwoRight;
	int DoorThreeRight;
	int DoorFourRight;
	int DoorFiveRight;
	int DoorSixRight;

};

#endif

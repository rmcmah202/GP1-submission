/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();



/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	theScore = 0;
	theAreaClicked = { 0, 0 };
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "bullet", "gameBackground", "playerCharacter", "enemySprite", "spawnRoom", "corridor1", "corridor2", "corridor3", "corridor4", "corridor5", "corridor6", "corridor7", "corridor8", "corridor9", "corridor10", "deadEnd1", "deadEnd2", "deadEnd3", "deadEnd4", "deadEnd5", "winBackground", "loseBackground" };
	texturesToUse = { "Images\\bullet.png", "Images\\mainGameBackground.png", "Images\\playerSprite.png", "Images\\enemySprite.png", "Images\\mazeSpawnRoom.png", "Images\\mazeCorridor1.png", "Images\\mazeCorridor2.png", "Images\\mazeCorridor3.png", "Images\\mazeCorridor4.png", "Images\\mazeCorridor5.png", "Images\\mazeCorridor6.png", "Images\\mazeCorridor7.png", "Images\\mazeCorridor8.png", "Images\\mazeCorridor9.png", "Images\\mazeCorridor10.png", "Images\\mazeDeadEnd1.png", "Images\\mazeDeadEnd2.png", "Images\\mazeDeadEnd3.png", "Images\\mazeDeadEnd4.png", "Images\\mazeDeadEnd5.png", "Images\\winBackground.png", "Images\\loseBackground.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}

	//Store the buttons
	btnNameList = { "exit_btn", "menu_btn", "play_btn"};
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png"};
	btnPos = { { 400, 375 }, { 400, 300 }, { 500, 500 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}

	theGameState = MENU;
	theBtnType = EXIT;

	// Create textures for Game Dialogue (text)
	fontList = { "Cheetah" };
	fontsToUse = { "Fonts/CHEETAH.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}

	gameTextNames = { "TitleTxt", "ThanksTxt", "SeeYouTxt", "StepTxt", "WonTxt", "LoseTxt" , "ButtonTxt" , "InstructionTxt" };
	gameTextList = { "A-Maze-ing!", "Thanks for playing!", "See you again soon!", "Steps: ", "YOU WON!", "You lose :(" , "Use WASD to move and space to shoot!" , "Navigate the maze and escape using as little steps as possible!" };  //text printed in font Title is the texture made from this
	for (int text = 0; text < gameTextNames.size(); text++) //For, integar text = 0, text is less than text name size, add 1 to text
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("Cheetah")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 })); //Adds the font type Cheetah and have a solid clear background to it
	}


	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/theme.wav", "Audio/gunShot.wav", "Audio/deathUrr.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1); // -1 makes it loop

	// Creates the background
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("gameBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("gameBackground")->getTWidth(), theTextureMgr->getTexture("gameBackground")->getTHeight());

	// Creates the player
	thePlayer.setSpritePos({ 480, 370 });
	thePlayer.setTexture(theTextureMgr->getTexture("playerCharacter"));
	thePlayer.setSpriteDimensions(theTextureMgr->getTexture("playerCharacter")->getTWidth(), theTextureMgr->getTexture("playerCharacter")->getTHeight());
	thePlayer.setRocketVelocity({ 0, 0 });

	// Create vector array of textures
	for (int counter = 0; counter < 2; counter++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[counter]->setSpritePos({ 930, 23 + (counter * 50) });
		theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[3]));
		theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[3])->getTWidth(), theTextureMgr->getTexture(textureName[3])->getTHeight());
		theAsteroids[counter]->setActive(true);
	}

	// Sets the score to "Steps: 0"
	theSScore = gameTextList[3] + to_string(theScore);

	//Contains the x coordinates for each room in the maze
	xPos[0] = 380;//spawn room
	xPos[1] = 313;//corridor 1
	xPos[2] = 313;//corridor 2
	xPos[3] = 246;//corridor 3
	xPos[4] = 246;//corridor 4
	xPos[5] = 246;//corridor 5
	xPos[6] = 313;//corridor 6
	xPos[7] = 837;//corridor 7
	xPos[8] = 930;//corridor 8
	xPos[9] = 179;//corridor 9
	xPos[10] = 180;//corridor10
	xPos[11] = 313;//dead end 1
	xPos[12] = 456;//dead end 2
	xPos[13] = 930;//dead end 3
	xPos[14] = 246;//dead end 4
	xPos[15] = 113;//dead end 5

	//Contains the y coordinates for each room in the maze
	yPos[0] = 264;//spawn room
	yPos[1] = 208;//corridor 1
	yPos[2] = 208;//corridor 2
	yPos[3] = 147;//corridor 3
	yPos[4] = 146;//corridor 4
	yPos[5] = 661;//corridor 5
	yPos[6] = 568;//corridor 6
	yPos[7] = 23;//corridor 7
	yPos[8] = 23;//corridor 8
	yPos[9] = 23;//corridor 9
	yPos[10] = 23;//corridor 10
	yPos[11] = 498;//dead end 1
	yPos[12] = 81;//dead end 2
	yPos[13] = 583;//dead end 3
	yPos[14] = 80;//dead end 4
	yPos[15] = 275;//dead end 5

	//sets the spawn points for each room
	for (int counter = 0; counter < 16; counter++)
	{
		roomPositionArray.push_back(new cAsteroid);
		roomPositionArray[counter]->setSpritePos({ xPos[counter], yPos[counter] });
		roomPositionArray[counter]->setTexture(theTextureMgr->getTexture(textureName[counter + 4]));
		roomPositionArray[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[counter + 4])->getTWidth(), theTextureMgr->getTexture(textureName[counter + 4])->getTHeight());
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case MENU:
	{
		// Moves the sprite to coordinates (0,0) in order to avoid bugs with collision
		spriteBkgd.setSpritePos({ 0, 0 });
		// Sets basic background
		spriteBkgd.setTexture(theTextureMgr->getTexture("gameBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("gameBackground")->getTWidth(), theTextureMgr->getTexture("gameBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //Temperory txture set to Title text
		pos = { 350, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Positon of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the text with the scale and position
		// Render the control text
		tempTextTexture = theTextureMgr->getTexture("ButtonTxt"); //Temporary texture set to Contorls text
		pos = { 250, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the texture with scale and position
		// Render the instructions text
		tempTextTexture = theTextureMgr->getTexture("InstructionTxt"); //Temporary texture set to Contorls text
		pos = { 250, 600, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the texture with scale and position

		//Rendering Buttons
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 350, 275 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale()); //Renders the play button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 350, 375 }); //Position of the exit button
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //Renders the exit button

	}
	break;
	case PLAYING:
	{
		// Sets background to main game background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("gameBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("gameBackground")->getTWidth(), theTextureMgr->getTexture("gameBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		
		//Render exit button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 725, 450 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Render the Score
		theTextureMgr->addTexture("Score", theFontMgr->getFont("Cheetah")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		cTexture* tempTextTexture2 = theTextureMgr->getTexture("Score");
		SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		FPoint scale2 = { 1, 1 };
		tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);
		
		//Updates the score
		theTextureMgr->deleteTexture("Score");
		theSScore = gameTextList[3] + to_string(theScore);

		//Render the rooms the player enters
		//spawn room
		if ((thePlayer.getSpritePos().x > 380 && thePlayer.getSpritePos().x < 569) && (thePlayer.getSpritePos().y > 271 && thePlayer.getSpritePos().y < 447))
		{
			roomPositionArray[0]->render(theRenderer, &roomPositionArray[0]->getSpriteDimensions(), &roomPositionArray[0]->getSpritePos(), roomPositionArray[0]->getSpriteRotAngle(), &roomPositionArray[0]->getSpriteCentre(), roomPositionArray[0]->getSpriteScale());

			//Sets the position of the walls
			rightWall = 562;
			leftWall = 387;
			topWall = 278;
			bottomWall = 440;

			//Sets the dimensions of the doors
			DoorOneLeft = 328;
			DoorTwoLeft = 383;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;


		}

		//corridor 1
		if ((thePlayer.getSpritePos().x > 308 && thePlayer.getSpritePos().x < 379) && (thePlayer.getSpritePos().y > 208 && thePlayer.getSpritePos().y < 517))
		{
			roomPositionArray[1]->render(theRenderer, &roomPositionArray[1]->getSpriteDimensions(), &roomPositionArray[1]->getSpritePos(), roomPositionArray[1]->getSpriteRotAngle(), &roomPositionArray[1]->getSpriteCentre(), roomPositionArray[1]->getSpriteScale());

			rightWall = 319;
			leftWall = 315;
			topWall = 215;
			bottomWall = 510;

			DoorOneRight = 208;
			DoorTwoRight = 214;
			DoorThreeRight = 328;
			DoorFourRight = 377;
			DoorFiveRight = 499;
			DoorSixRight = 566;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//corridor 2
		if ((thePlayer.getSpritePos().x > 308 && thePlayer.getSpritePos().x < 539) && (thePlayer.getSpritePos().y > 208 && thePlayer.getSpritePos().y < 213))
		{
			roomPositionArray[2]->render(theRenderer, &roomPositionArray[2]->getSpriteDimensions(), &roomPositionArray[2]->getSpritePos(), roomPositionArray[2]->getSpriteRotAngle(), &roomPositionArray[2]->getSpriteCentre(), roomPositionArray[2]->getSpriteScale());

			//
			rightWall = 531;
			leftWall = 315;
			topWall = 215;
			bottomWall = 208;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 310;
			DoorTwoDown = 379;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 527;
			DoorTwoUp = 594;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;


		}
		
		//corridor 3
		if ((thePlayer.getSpritePos().x > 239 && thePlayer.getSpritePos().x < 539) && (thePlayer.getSpritePos().y > 140 && thePlayer.getSpritePos().y < 206)) 
		{
			roomPositionArray[3]->render(theRenderer, &roomPositionArray[3]->getSpriteDimensions(), &roomPositionArray[3]->getSpritePos(), roomPositionArray[3]->getSpriteRotAngle(), &roomPositionArray[3]->getSpriteCentre(), roomPositionArray[3]->getSpriteScale());

			rightWall = 532;
			leftWall = 246;
			topWall = 147;
			bottomWall = 143;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 527;
			DoorTwoDown = 594;
			DoorThreeDown = 239;
			DoorFourDown = 289;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 456;
			DoorTwoUp = 525;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;


		}

		//corridor 4
		if ((thePlayer.getSpritePos().x > 239 && thePlayer.getSpritePos().x < 262) && (thePlayer.getSpritePos().y > 140 && thePlayer.getSpritePos().y < 746))
		{
			roomPositionArray[4]->render(theRenderer, &roomPositionArray[4]->getSpriteDimensions(), &roomPositionArray[4]->getSpritePos(), roomPositionArray[4]->getSpriteRotAngle(), &roomPositionArray[4]->getSpriteCentre(), roomPositionArray[4]->getSpriteScale());

			rightWall = 255;
			leftWall = 246;
			topWall = 147;
			bottomWall = 685;

			DoorOneRight = 140;
			DoorTwoRight = 206;
			DoorThreeRight = 660;
			DoorFourRight = 692;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;


		}

		//corridor 5
		if ((thePlayer.getSpritePos().x > 239 && thePlayer.getSpritePos().x < 945) && (thePlayer.getSpritePos().y > 660 && thePlayer.getSpritePos().y < 746))
		{
			roomPositionArray[5]->render(theRenderer, &roomPositionArray[5]->getSpriteDimensions(), &roomPositionArray[5]->getSpritePos(), roomPositionArray[5]->getSpriteRotAngle(), &roomPositionArray[5]->getSpriteCentre(), roomPositionArray[5]->getSpriteScale());

			rightWall = 945;
			leftWall = 246;
			topWall = 667;
			bottomWall = 685;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 246;
			DoorTwoUp = 256;
			DoorThreeUp = 305;
			DoorFourUp = 403;
			DoorFiveUp = 930;
			DoorSixUp = 1001;

			
		}

		//corridor 6
		if ((thePlayer.getSpritePos().x > 308 && thePlayer.getSpritePos().x < 872) && (thePlayer.getSpritePos().y > 561 && thePlayer.getSpritePos().y < 658))
		{
			roomPositionArray[6]->render(theRenderer, &roomPositionArray[6]->getSpriteDimensions(), &roomPositionArray[6]->getSpritePos(), roomPositionArray[6]->getSpriteRotAngle(), &roomPositionArray[6]->getSpriteCentre(), roomPositionArray[6]->getSpriteScale());

			rightWall = 865;
			leftWall = 312;
			topWall = 568;
			bottomWall = 601;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 305;
			DoorTwoDown = 403;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 831;
			DoorTwoUp = 928;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//corridor 7
		if ((thePlayer.getSpritePos().x > 830 && thePlayer.getSpritePos().x < 928) && (thePlayer.getSpritePos().y > 14 && thePlayer.getSpritePos().y < 601))
		{
			roomPositionArray[7]->render(theRenderer, &roomPositionArray[7]->getSpriteDimensions(), &roomPositionArray[7]->getSpritePos(), roomPositionArray[7]->getSpriteRotAngle(), &roomPositionArray[7]->getSpriteCentre(), roomPositionArray[7]->getSpriteScale());

			rightWall = 865;
			leftWall = 838;
			topWall = 23;
			bottomWall = 568;

			DoorOneRight = 495;
			DoorTwoRight = 560;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 17;
			DoorTwoLeft = 22;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 831;
			DoorTwoDown = 928;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//corridor 8
		if ((thePlayer.getSpritePos().x > 923 && thePlayer.getSpritePos().x < 1001) && (thePlayer.getSpritePos().y > 23 && thePlayer.getSpritePos().y < 517))
		{
			roomPositionArray[8]->render(theRenderer, &roomPositionArray[8]->getSpriteDimensions(), &roomPositionArray[8]->getSpritePos(), roomPositionArray[8]->getSpriteRotAngle(), &roomPositionArray[8]->getSpriteCentre(), roomPositionArray[8]->getSpriteScale());

			rightWall = 942;
			leftWall = 928;
			topWall = 27;
			bottomWall = 510;

			DoorOneRight = 17;
			DoorTwoRight = 29;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 495;
			DoorTwoLeft = 567;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//corridor 9
		if ((thePlayer.getSpritePos().x > 172 && thePlayer.getSpritePos().x < 872) && (thePlayer.getSpritePos().y > 14 && thePlayer.getSpritePos().y < 22))
		{
			roomPositionArray[9]->render(theRenderer, &roomPositionArray[9]->getSpriteDimensions(), &roomPositionArray[9]->getSpritePos(), roomPositionArray[9]->getSpriteRotAngle(), &roomPositionArray[9]->getSpriteCentre(), roomPositionArray[9]->getSpriteScale());

			rightWall = 865;
			leftWall = 179;
			topWall = 23;
			bottomWall = 15;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 836;
			DoorTwoDown = 866;
			DoorThreeDown = 172;
			DoorFourDown = 187;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//corridor 10
		if ((thePlayer.getSpritePos().x > 172 && thePlayer.getSpritePos().x < 188) && (thePlayer.getSpritePos().y > 14 && thePlayer.getSpritePos().y < 286))
		{
			roomPositionArray[10]->render(theRenderer, &roomPositionArray[10]->getSpriteDimensions(), &roomPositionArray[10]->getSpritePos(), roomPositionArray[10]->getSpriteRotAngle(), &roomPositionArray[10]->getSpriteCentre(), roomPositionArray[10]->getSpriteScale());

			rightWall = 186;
			leftWall = 179;
			topWall = 23;
			bottomWall = 279;

			DoorOneRight = 19;
			DoorTwoRight = 21;
			DoorThreeRight = 82;
			DoorFourRight = 84;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 271;
			DoorTwoLeft = 280;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//dead end 1
		if ((thePlayer.getSpritePos().x > 308 && thePlayer.getSpritePos().x < 539) && (thePlayer.getSpritePos().y > 491 && thePlayer.getSpritePos().y < 517))
		{
			roomPositionArray[11]->render(theRenderer, &roomPositionArray[11]->getSpriteDimensions(), &roomPositionArray[11]->getSpritePos(), roomPositionArray[11]->getSpriteRotAngle(), &roomPositionArray[11]->getSpriteCentre(), roomPositionArray[11]->getSpriteScale());

			rightWall = 531;
			leftWall = 315;
			topWall = 499;
			bottomWall = 510;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 311;
			DoorTwoUp = 320;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//dead end 2
		if ((thePlayer.getSpritePos().x > 452 && thePlayer.getSpritePos().x < 539) && (thePlayer.getSpritePos().y > 81 && thePlayer.getSpritePos().y < 145)) //y=89 for wall collision
		{
			roomPositionArray[12]->render(theRenderer, &roomPositionArray[12]->getSpriteDimensions(), &roomPositionArray[12]->getSpritePos(), roomPositionArray[12]->getSpriteRotAngle(), &roomPositionArray[12]->getSpriteCentre(), roomPositionArray[12]->getSpriteScale());

			rightWall = 531;
			leftWall = 460;
			topWall = 83;
			bottomWall = 83;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 456;
			DoorTwoDown = 460;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//dead end 3
		if ((thePlayer.getSpritePos().x > 923 && thePlayer.getSpritePos().x < 945) && (thePlayer.getSpritePos().y > 581 && thePlayer.getSpritePos().y < 746))
		{
			roomPositionArray[13]->render(theRenderer, &roomPositionArray[13]->getSpriteDimensions(), &roomPositionArray[13]->getSpritePos(), roomPositionArray[13]->getSpriteRotAngle(), &roomPositionArray[13]->getSpriteCentre(), roomPositionArray[13]->getSpriteScale());

			rightWall = 942;
			leftWall = 928;
			topWall = 588;
			bottomWall = 685;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 666;
			DoorTwoLeft = 686;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 1030;
			DoorTwoUp = 1030;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		//dead end 4
		if ((thePlayer.getSpritePos().x > 176 && thePlayer.getSpritePos().x < 391) && (thePlayer.getSpritePos().y > 80 && thePlayer.getSpritePos().y < 89))
		{
			roomPositionArray[14]->render(theRenderer, &roomPositionArray[14]->getSpriteDimensions(), &roomPositionArray[14]->getSpritePos(), roomPositionArray[14]->getSpriteRotAngle(), &roomPositionArray[14]->getSpriteCentre(), roomPositionArray[14]->getSpriteScale());

			rightWall = 380;
			leftWall = 179;
			topWall = 87;
			bottomWall = 60;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 178;
			DoorTwoDown = 186;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 178;
			DoorTwoUp = 186;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;


		}

		//dead end 5
		if ((thePlayer.getSpritePos().x > 105 && thePlayer.getSpritePos().x < 188) && (thePlayer.getSpritePos().y > 268 && thePlayer.getSpritePos().y < 286))
		{
			roomPositionArray[15]->render(theRenderer, &roomPositionArray[15]->getSpriteDimensions(), &roomPositionArray[15]->getSpritePos(), roomPositionArray[15]->getSpriteRotAngle(), &roomPositionArray[15]->getSpriteCentre(), roomPositionArray[15]->getSpriteScale());

			rightWall = 186;
			leftWall = 112;
			topWall = 275;
			bottomWall = 279;

			DoorOneRight = 770;
			DoorTwoRight = 770;
			DoorThreeRight = 770;
			DoorFourRight = 770;
			DoorFiveRight = 770;
			DoorSixRight = 770;
			DoorOneLeft = 770;
			DoorTwoLeft = 770;
			DoorThreeLeft = 770;
			DoorFourLeft = 770;
			DoorFiveLeft = 770;
			DoorSixLeft = 770;
			DoorOneDown = 1030;
			DoorTwoDown = 1030;
			DoorThreeDown = 1030;
			DoorFourDown = 1030;
			DoorFiveDown = 1030;
			DoorSixDown = 1030;
			DoorOneUp = 178;
			DoorTwoUp = 194;
			DoorThreeUp = 1030;
			DoorFourUp = 1030;
			DoorFiveUp = 1030;
			DoorSixUp = 1030;

			
		}

		// Render the player
		thePlayer.render(theRenderer, &thePlayer.getSpriteDimensions(), &thePlayer.getSpritePos(), thePlayer.getSpriteRotAngle(), &thePlayer.getSpriteCentre(), thePlayer.getSpriteScale());

		// Renders the enemies if the player is in the correct room
		if ((thePlayer.getSpritePos().x > 923 && thePlayer.getSpritePos().x < 1001) && (thePlayer.getSpritePos().y > 23 && thePlayer.getSpritePos().y < 517))
		{
			for (int draw = 0; draw < theAsteroids.size(); draw++)
			{
				theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
			}
		}

		// Render the bullets
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}

	}
	break;
	case END:
	{
		//Sets area clicked to coordinates (0,0) to avoid bugs
		theAreaClicked = { 0, 0 };

		//Renders lose background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("loseBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("loseBackground")->getTWidth(), theTextureMgr->getTexture("loseBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		//Renders texts
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("LoseTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Renders the buttons
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 250, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 250, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case WON:
	{
		//Sets area clicked to coordinates (0,0) and the player position to the center of the map to avoid bugs
		theAreaClicked = { 0, 0 };
		thePlayer.setSpritePos({ 480, 370 });

		//Renders the correct background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("winBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("winBackground")->getTWidth(), theTextureMgr->getTexture("winBackground")->getTHeight());

		//Renders all the texts for the title, as well as a thank you message, a see you later message and a "YOU WON" message
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("WonTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Renders the buttons
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 450, 500 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 550, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 550, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT:
	{
		//closes the program
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);

}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{
	
}

void cGame::update(double deltaTime)
{	
	// Check which Button was clicked and changes to correct state
	if (theGameState == MENU || theGameState == END || theGameState == WON)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
	}

	theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);


	//Resets the game every time the play button is pressed, regardless of which screen it is pressed from
	if (theGameState == MENU || theGameState == WON)
	{
		if (theButtonMgr->getBtn("play_btn")->getClicked())
		{
			thePlayer.setSpritePos({ 480, 370 });
			theScore = 0;
			theAsteroids.clear();
			for (int astro = 0; astro < 2; astro++)
			{
				theAsteroids.push_back(new cAsteroid);
				theAsteroids[astro]->setSpritePos({ 930, 23 + (astro * 50) });
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[3]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[3])->getTWidth(), theTextureMgr->getTexture(textureName[3])->getTHeight());
				theAsteroids[astro]->setActive(true);
			}
		}
	}

	//Allows for the win condition
	if (thePlayer.getSpritePos().x > 1024)
	{
		theGameState = WON;
	}

	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
		}
	}


	// Update the visibility and position of each bullet
	vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	while (bulletIterartor != theBullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = theBullets.erase(bulletIterartor);
		}
		else
		{
			(*bulletIterartor)->update(deltaTime);
			++bulletIterartor;
		}
	}
	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	{
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*asteroidIterator)->setActive(false);
				(*bulletIterartor)->setActive(false);
				theSoundMgr->getSnd("explosion")->play(0);
			}
		}
	}
	
	//Checks for collisions between the enemy and the player and sets the game state to the lose screen
	for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
	{
		if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(thePlayer.getBoundingRect())))
		{
			theGameState = END;
		}
	}

	// Update the player position
	thePlayer.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					//allows for you to click buttons
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_d:
				{
					//Checks that the game state is playing, then checks that the player is within the confines of a room or a door before allowing the player to move and adding 1 step
					if (theGameState == PLAYING)
					{
						if (thePlayer.getSpritePos().x < rightWall || (thePlayer.getSpritePos().y < DoorTwoRight & thePlayer.getSpritePos().y > DoorOneRight) || (thePlayer.getSpritePos().y < DoorFourRight & thePlayer.getSpritePos().y > DoorThreeRight) || (thePlayer.getSpritePos().y < DoorSixRight & thePlayer.getSpritePos().y > DoorFiveRight))
						{
							thePlayer.setSpritePos({ thePlayer.getSpritePos().x + 7, thePlayer.getSpritePos().y });
							theScore += 1;
						}
					}
				}
				break;
				case SDLK_a:
				{
					//Checks that the game state is playing, then checks that the player is within the confines of a room or a door before allowing the player to move and adding 1 step
					if (theGameState == PLAYING)
					{
						if (thePlayer.getSpritePos().x > leftWall || (thePlayer.getSpritePos().y < DoorTwoLeft & thePlayer.getSpritePos().y > DoorOneLeft) || (thePlayer.getSpritePos().y < DoorFourLeft & thePlayer.getSpritePos().y > DoorThreeLeft) || (thePlayer.getSpritePos().y < DoorSixLeft & thePlayer.getSpritePos().y > DoorFiveLeft))
						{
							thePlayer.setSpritePos({ thePlayer.getSpritePos().x - 7, thePlayer.getSpritePos().y });
							theScore += 1;
						}
					}
				}
				break;
				case SDLK_w:
				{
					//Checks that the game state is playing, then checks that the player is within the confines of a room or a door before allowing the player to move and adding 1 step
					if (theGameState == PLAYING)
					{
						if (thePlayer.getSpritePos().y > topWall || (thePlayer.getSpritePos().x < DoorTwoUp & thePlayer.getSpritePos().x > DoorOneUp) || (thePlayer.getSpritePos().x < DoorFourUp & thePlayer.getSpritePos().x > DoorThreeUp) || (thePlayer.getSpritePos().x < DoorSixUp & thePlayer.getSpritePos().x > DoorFiveUp))
						{
							thePlayer.setSpritePos({ thePlayer.getSpritePos().x, thePlayer.getSpritePos().y - 7 });
							theScore += 1;
						}
					}
				}
				break;
				case SDLK_s:
				{
					//Checks that the game state is playing, then checks that the player is within the confines of a room or a door before allowing the player to move and adding 1 step
					if (theGameState == PLAYING)
					{
						if (thePlayer.getSpritePos().y < bottomWall || (thePlayer.getSpritePos().x < DoorTwoDown & thePlayer.getSpritePos().x > DoorOneDown) || (thePlayer.getSpritePos().x < DoorFourDown & thePlayer.getSpritePos().x > DoorThreeDown) || (thePlayer.getSpritePos().x < DoorSixDown & thePlayer.getSpritePos().x > DoorFiveDown))
						{
							thePlayer.setSpritePos({ thePlayer.getSpritePos().x, thePlayer.getSpritePos().y + 7 });
							theScore += 1;
						}
					}
				}
				break;
				case SDLK_SPACE:
				{
					//Shoots a bullet whenever the player presses play
					theBullets.push_back(new cBullet);
					int numBullets = theBullets.size() - 1;
					theBullets[numBullets]->setSpriteScale({ 1.0f, 0.25f });
					theBullets[numBullets]->setSpritePos({ thePlayer.getBoundingRect().x + thePlayer.getSpriteCentre().x, thePlayer.getBoundingRect().y + thePlayer.getSpriteCentre().y });
					theBullets[numBullets]->setSpriteTranslation({ 2.0f, 2.0f });
					theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet"));
					theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet")->getTWidth(), theTextureMgr->getTexture("bullet")->getTHeight());
					theBullets[numBullets]->setBulletVelocity({ 2.0f, 2.0f });
					theBullets[numBullets]->setSpriteRotAngle(thePlayer.getSpriteRotAngle());
					theBullets[numBullets]->setActive(true);
				}
				theSoundMgr->getSnd("shot")->play(0);
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}
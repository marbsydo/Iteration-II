#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "LevelManager.hpp"
#include "Tiles.hpp"
#include "Display.hpp"
#include "IterationData.hpp"
#include "LevelData.hpp"
#include "EventData.hpp"
#include "LinkData.hpp"
#include "Timeline.hpp"
#include "EndGame.hpp"
#include "General.hpp"
#include "Events.hpp"

LevelManager::LevelManager(GameEngine* game) {
    levelData = new LevelData(game, this);
    iterationData = new IterationData(this);
    eventData = new EventData();
    linkData = new LinkData();
    timeline = new Timeline();
    endGame = new EndGame();
}
LevelManager::~LevelManager() {
    delete levelData;
    delete iterationData;
    delete eventData;
    delete linkData;
    delete timeline;
    delete endGame;
}

bool LevelManager::Load(std::string levelName) {
    return Load(levelName.c_str());
}

bool LevelManager::Load(const char* levelName) {
    std::string line;
    std::ifstream f(levelName);
    int x, y, lineNumber;
    int expectedWidth = 80, expectedHeight = 30;
    expectedWidth = 80;
    expectedHeight = 30;

    int timelimit = 40, critical = 20; // These values are read from the file

    // In the level file, there are descriptions for how objects are linked together
    // These links refer to objects by the order in which they appear in the level
    // We use these vectors to record the order in which these linkable objects are created
    // These vectors are then used to link up the appropriate objects
    std::vector<int> doors;
    std::vector<int> levers;
    std::vector<int> plates;
    std::vector<int> alarms;

    // Title and subtitles (0 = Title, 1...LEVEL_NUM_TITLES == Subtitles #1 -> #LEVEL_NUM_TITLES)
    std::string levelTitles[LEVEL_NUM_TITLES];


    bool invalidMap = false;
    if (f.is_open()) {
        x = y = lineNumber = 0;
        //while (getline(f, line) && !invalidMap && y < expectedHeight) {
        while (getline(f, line) && !invalidMap) {
            if (lineNumber == 0) {
                // Read expectedWidth
                if (::StringToInt(line, expectedWidth)) {
#ifdef DEBUG_VERBOSE
                    std::cout << "expectedWidth: " << expectedWidth << std::endl;
#endif
                    if (expectedWidth < LEVEL_WIDTH_MIN || expectedWidth > LEVEL_WIDTH_MAX) {
                        invalidMap = true;
                        std::cout << "ERROR: Level width must fall between " << LEVEL_WIDTH_MIN << " and " << LEVEL_WIDTH_MAX << std::endl;
                    }
                } else {
                    invalidMap = true;
                    std::cout << "ERROR: Expected level width on line " << lineNumber << std::endl;
                }
            } else if (lineNumber == 1) {
                // Read expectedHeight
                if (::StringToInt(line, expectedHeight)) {
#ifdef DEBUG_VERBOSE
                    std::cout << "expectedHeight: " << expectedHeight << std::endl;
#endif
                    if (expectedHeight < LEVEL_HEIGHT_MIN || expectedHeight > LEVEL_HEIGHT_MAX) {
                        invalidMap = true;
                        std::cout << "ERROR: Level height must fall between " << LEVEL_HEIGHT_MIN << " and " << LEVEL_HEIGHT_MAX << std::endl;
                    }
                } else {
                    invalidMap = true;
                    std::cout << "ERROR: Expected level height on line " << lineNumber << std::endl;
                }

                // Create the blank level
                levelData->CreateBlankLevel(expectedWidth, expectedHeight);
            } else if (lineNumber >= 2 && lineNumber < 2 + expectedHeight) {
                // Read levelData
                if ((int)line.length() == expectedWidth) {
                    for (x = 0; x < expectedWidth; x++) {
                        // Convert the read symbol into a TileType
                        switch (line[x]) {
                        /// Structure and Mechanisms
                        case '#': // Wall
                            levelData->SetTileDetails(x, y, TileType::Wall, true);
                            break;
                        case 'd': // Door
                            levelData->SetTileDetails(x, y, TileType::Floor, true);
                            doors.push_back(levelData->CreateDoor(x, y, false));
                            break;
                        case 'D': // Door (cannot be opened directly)
                            levelData->SetTileDetails(x, y, TileType::Floor, true);
                            doors.push_back(levelData->CreateDoor(x, y, true));
                            break;
                        case 'W': // Window
                            levelData->SetTileDetails(x, y, TileType::Wall, true);
                            levelData->CreateWindow(x, y);
                            break;
                        case 'l': // Lever (state = off)
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levers.push_back(levelData->CreateLever(x, y, STATE_LEVER_OFF));
                            break;
                        case 'L': // Lever (state = on)
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levers.push_back(levelData->CreateLever(x, y, STATE_LEVER_ON));
                            break;
                        case '_': // Pressure plate
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            plates.push_back(levelData->CreatePressurePlate(x, y));
                            break;
                        case '=': // Motion alarm
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            alarms.push_back(levelData->CreateMotionAlarm(x, y));
                            break;
                        /// Radiation
                        case 'r': // Radiation (weak)
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levelData->CreateRadiation(x, y, 0);
                            break;
                        case 'R': // Radiation (strong)
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levelData->CreateRadiation(x, y, 1);
                            break;
                        /// Main
                        case 'P': // Player
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levelData->CreatePlayerOriginal(x, y); //TODO: Only allow creation of one player (with control)
                            break;
                        case 'T': // Terminal
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levelData->CreateTerminal(x, y);
                            break;
                        case 'Y': // Time Machine
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            levelData->CreateTimeMachine(x, y);
                            break;
                        /// Other
                        case '.': // Floor
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                        break;
                        case ' ': // Restricted (non-walkable space that should never be accessible)
                            levelData->SetTileDetails(x, y, TileType::Restricted, false);
                            break;
                        default:
                            invalidMap = true;
                            std::cout << "ERROR: Unknown symbol '" << line[x] << "' at position (" << x << "," << y << ")" << std::endl;
                            levelData->SetTileDetails(x, y, TileType::Floor, false);
                            break;
                        }
                    }
                } else {
                    invalidMap = true;
                    std::cout << "ERROR: Invalid map. Incorrect width on row " << y << std::endl;
                    std::cout << "(Should have been width " << expectedWidth << ". Actual width was " << line.length() << ")" << std::endl;
                }
                y++;
            } else {
                // Read the rest of the file

                // Split the line by spaces to read the command
                std::vector<std::string> lineExp = ::Explode(line, ' ');

                // Check if it is a link command
                if (lineExp[0] == "Link:") {
                    if (lineExp.size() == 4) {
                        // This is a link command
                        std::vector<std::string> objectFromExp = ::Explode(lineExp[1], '#');
                        std::vector<std::string> objectToExp = ::Explode(lineExp[3], '#');

                        if (objectFromExp.size() != 2) {
                            std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                            std::cout << "(The first object is formatted wrong)" << std::endl;
                            invalidMap = true;
                        } else if (objectToExp.size() != 2) {
                            std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                            std::cout << "(The second object is formatted wrong)" << std::endl;
                            invalidMap = true;
                        } else {
                            // Objects are formatted okay
                            int objectFromIndex, objectToIndex;
                            if (::StringToInt(objectFromExp[1], objectFromIndex)) {
                                if (::StringToInt(objectToExp[1], objectToIndex)) {

                                    // Indexes in the level file start at 1, but start at 0 in game, so subtract one
                                    --objectFromIndex;
                                    --objectToIndex;

                                    if (objectFromIndex < 0) {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(First object index is negative)" << std::endl;
                                        invalidMap = true;
                                    }

                                    if (objectToIndex < 0) {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(Second object index is negative)" << std::endl;
                                        invalidMap = true;
                                    }

                                    // We know the object indexes, so let's get references to the GameObject::StaticLinkable
                                    bool indexTooGreat = false;

                                    // Get a reference to objectFrom
                                    GameObject::StaticLinkable* objectFrom;
                                    if (objectFromExp[0] == "Lever") {
                                        if (objectFromIndex >= (int)levers.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(levers[objectFromIndex]);
                                    } else if (objectFromExp[0] == "Door") {
                                        if (objectFromIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(doors[objectFromIndex]);
                                    } else if (objectFromExp[0] == "Plate") {
                                        if (objectFromIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(plates[objectFromIndex]);
                                    } else if (objectFromExp[0] == "Alarm") {
                                        if (objectFromIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(alarms[objectFromIndex]);
                                    } else {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(First object is not recognised)" << std::endl;
                                        invalidMap = true;
                                    }

                                    // Get a reference to objectTo
                                    GameObject::StaticLinkable* objectTo;
                                    if (objectToExp[0] == "Lever") {
                                        if (objectToIndex >= (int)levers.size())
                                            indexTooGreat = true;
                                        else
                                            objectTo = (GameObject::StaticLinkable*)levelData->GetObjectPointer(levers[objectToIndex]);
                                    } else if (objectToExp[0] == "Door") {
                                        if (objectToIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectTo = (GameObject::StaticLinkable*)levelData->GetObjectPointer(doors[objectToIndex]);
                                    } else if (objectFromExp[0] == "Plate") {
                                        if (objectFromIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(plates[objectFromIndex]);
                                    } else if (objectFromExp[0] == "Alarm") {
                                        if (objectFromIndex >= (int)doors.size())
                                            indexTooGreat = true;
                                        else
                                            objectFrom = (GameObject::StaticLinkable*)levelData->GetObjectPointer(alarms[objectFromIndex]);
                                    } else {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(Second object is not recognised)" << std::endl;
                                        invalidMap = true;
                                    }

                                    if (indexTooGreat) {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(An object index exceeded the number of objects)" << std::endl;
                                        invalidMap = true;
                                    }

                                    // Work out what the function is
                                    LinkFunction func;
                                    if (lineExp[2] == "=>") {
                                        func = LinkFunction::SetEqual;
                                    } else if (lineExp[2] == "~>") {
                                        func = LinkFunction::SetInverse;
                                    } else {
                                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                        std::cout << "(Function is not recognised)" << std::endl;
                                        invalidMap = true;
                                    }

                                    if (!invalidMap) {
                                        // Now we have all the information
                                        linkData->Add(objectFrom, func, objectTo);
                                        }

                                } else {
                                    std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                    std::cout << "(Invalid index for the second object)" << std::endl;
                                    invalidMap = true;
                                }
                            } else {
                                std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                                std::cout << "(Invalid index for the first object)" << std::endl;
                                invalidMap = true;
                            }
                        }
                    } else {
                        std::cout << "Invalid \"Link:\" command: " << line << std::endl;
                        std::cout << "(There are the wrong number of arguments)" << std::endl;
                        invalidMap = true;
                    }
                } else if (lineExp[0] == "Timelimit:") {
                    if (lineExp.size() == 2) {
                        // Read the Timelimit
                        if (::StringToInt(lineExp[1], timelimit)) {
#ifdef DEBUG_VERBOSE
                            std::cout << "Read timelimit as: " << timelimit << std::endl;
#endif // DEBUG_VERBOSE
                        } else {
                            std::cout << "Invalid \"Timelimit:\" command: " << line << std::endl;
                            std::cout << "(Invalid timelimit)" << std::endl;
                        }
                    } else {
                        std::cout << "Invalid \"Timelimit:\" command: " << line << std::endl;
                        std::cout << "(There are the wrong number of arguments)" << std::endl;
                    }
                } else if (lineExp[0] == "Critical:") {
                    if (lineExp.size() == 2) {
                        // Read the Critical
                        if (::StringToInt(lineExp[1], critical)) {
#ifdef DEBUG_VERBOSE
                            std::cout << "Read critical as: " << critical << std::endl;
#endif // DEBUG_VERBOSE
                        } else {
                            std::cout << "Invalid \"Critical:\" command: " << line << std::endl;
                            std::cout << "(Invalid critical)" << std::endl;
                        }
                    } else {
                        std::cout << "Invalid \"Critical:\" command: " << line << std::endl;
                        std::cout << "(There are the wrong number of arguments)" << std::endl;
                    }
                } else if (lineExp[0] == "Title:" || lineExp[0] == "Subtitle:" || lineExp[0] == "Subtitle1:" || lineExp[0] == "Subtitle2:" || lineExp[0] == "Subtitle3:") {
                    if (lineExp.size() >= 2) {
                        // NOTE: If LEVEL_NUM_TITLES is changed, this section will need updating!
                        // Work out the index within the levelTitles array for this particular title/subtitle
                        int levelTitleIndex = 0;
                        if (lineExp[0] == "Title:")
                            levelTitleIndex = 0;
                        if (lineExp[0] == "Subtitle:" || lineExp[0] == "Subtitle1:")
                            levelTitleIndex = 1;
                        if (lineExp[0] == "Subtitle2:")
                            levelTitleIndex = 2;
                        if (lineExp[0] == "Subtitle3:")
                            levelTitleIndex = 3;

                        // Concatenate the rest of lineExp
                        std::ostringstream os;
                        os << line.substr(lineExp[0].size() + 1, std::string::npos);
                        levelTitles[levelTitleIndex] = os.str();
                    } else {
                        std::cout << "Invalid \"Title:\" or \"Subtitle:\" command: " << line << std::endl;
                        std::cout << "(No string supplied)" << std::endl;
                    }
                }
            }
            lineNumber++;
        }
        f.close();

        if (y != expectedHeight) {
            invalidMap = true;
            std::cout << "ERROR: Invalid map. Incorrect number of rows" << std::endl;
            std::cout << "(Should have had " << expectedHeight << " rows)" << std::endl;
        }

    } else {
        invalidMap = true;
        std::cout << "ERROR: Cannot open file: " << levelName << std::endl;
    }

    // Check the timelimit and critical times are valid
    if (timelimit >= 1 && critical >= 1 && critical <= timelimit && timelimit <= 75 && critical <= 75) {
#ifdef DEBUG_VERBOSE
        std::cout << "timelimit and critical are valid" << std::endl;
#endif // DEBUG_VERBOSE
    } else {
        invalidMap = true;
        std::cout << "ERROR: Invalid values for timelimit and/or critical" << std::endl;
    }

    if (invalidMap) {
        // Delete all objects because we failed
        std::cout << "Destroying all generated game objects because level failed to load completely" << std::endl;
        levelData->DestroyAllObjects();
    } else {
        // Map was valid

        // Tell LinkData which objects are linkable
        for (int i = 0; i < (int)doors.size(); i++) {
            linkData->GiveReferenceToStaticLinkableObject((GameObject::StaticLinkable*)levelData->GetObjectPointer(doors[i]));
        }
        for (int i = 0; i < (int)levers.size(); i++) {
            linkData->GiveReferenceToStaticLinkableObject((GameObject::StaticLinkable*)levelData->GetObjectPointer(levers[i]));
        }
        for (int i = 0; i < (int)plates.size(); i++) {
            linkData->GiveReferenceToStaticLinkableObject((GameObject::StaticLinkable*)levelData->GetObjectPointer(plates[i]));
        }
        for (int i = 0; i < (int)alarms.size(); i++) {
            linkData->GiveReferenceToStaticLinkableObject((GameObject::StaticLinkable*)levelData->GetObjectPointer(alarms[i]));
        }

        // Pass the titles to levelData
        levelData->SetTitles(levelTitles);

        // Set the timelimit and critical that were read from file
        iterationData->SetTimeLimit(timelimit);
        iterationData->SetTimeMeltdown(critical);

        // Work out what characters should be displayed
        levelData->CalculateDisplayCharacters();

        // And update the links once before we start
        linkData->Update();
    }

    // Return whether we were successful
    return !invalidMap;
}

void LevelManager::Update(GameEngine* game) {

    if (timeChangedForwardFlag && timeChangedBackwardFlag) {
        std::cout << "WARNING: Time changed forward and backward in the same instant" << std::endl;
    }

    // If time has changed backward
    if (timeChangedBackwardFlag) {
        timeChangedBackwardFlag = false;

        // 1) Decrement time
        iterationData->time--;

        // 2) Execute all events for the current time
        eventData->ExecuteBackwardEvents(iterationData->GetTime());

        // Call UpdateTimeChanged() in all GameObjects
        UpdateTimeChanged();
        linkData->Update();
    }

    // Call Update() in all GameObjects
    int num = levelData->GetNumObjects();
    for (int index = 0; index < num; index++) {
        levelData->CallObjectUpdate(index);
    }

    // If time has changed forward
    if (timeChangedForwardFlag) {
        timeChangedForwardFlag = false;

        // 1) Execute all events for the current time
        eventData->ExecuteForwardEvents(iterationData->GetTime());

        // 2) Increment time
        iterationData->time++;

        // Call UpdateTimeChanged() in all GameObjects
        UpdateTimeChanged();
        linkData->Update();
    }

    // For when the game has ended
    if (endGame->Ended()) {
        bool keyEnter = game->controls->GetKeyDown(InputKey::Enter);

        if (keyEnter) {
            if (endGame->GetEndReason() == EndReason::Victory) {
                // Go to the next level
                game->controls->ResetKeyDelay();
                game->LevelLoadNext();
            }
            if (endGame->GetEndReason() == EndReason::Meltdown) {
                // Restart the current level
                game->controls->ResetKeyDelay();
                game->LevelLoadCurrent();
            }
        }
    }

    // For restarting current level
    if (game->controls->GetKeyDown(InputKey::Restart)) {
        game->LevelLoadCurrent();
    }
}

void LevelManager::UpdateTimeChanged() {
    // Call UpdateTimeChanged() in all GameObjects
    int num = levelData->GetNumObjects();
    for (int index = 0; index < num; index++) {
        levelData->CallObjectUpdateTimeChanged(index);
    }

    // Check if we have lost
    if (iterationData->GetTime() == iterationData->GetTimeLimit()) {

        // We have lost
        endGame->Meltdown();
    }
}

void LevelManager::UpdateDisplay(GameEngine* game) {

    // Reset all tiles to blank
    game->display->SetAll(TILE_BLANK);

    // Draw the level in the center
    int offsetX = (game->display->GetWidth() - levelData->GetWidth())/2;
    int offsetY = TITLE_HEIGHT + (LEVEL_HEIGHT_MAX - levelData->GetHeight())/2;//(game->display->GetHeight() - levelData->GetHeight())/2;

    // Draw all the walls and floors
    for (int y = 0; y < levelData->GetHeight(); y++) {
        for (int x = 0; x < levelData->GetWidth(); x++) {
            int c = levelData->GetTileDisplayCharacter(x, y);
            game->display->SetDisplayCharacter(x + offsetX, y + offsetY, c);
        }
    }

    // 2D array of int vectors

    // NOTE: In order to make clang happy, we use this:
    //   LEVEL_WIDTH_MAX * LEVEL_HEIGHT_MAX
    // Instead of this:
    //   levelData->GetWidth() * levelData->GetHeight();
    std::vector<int> objectQueue[LEVEL_WIDTH_MAX * LEVEL_HEIGHT_MAX];

    // Queue all the objects to be drawn in the 2D array of int vectors
    for (int index = 0; index < levelData->GetNumObjects(); index++) {
        int c = levelData->GetObjectDisplayCharacter(index);
        int x = levelData->GetObjectX(index);
        int y = levelData->GetObjectY(index);

        // Check the object lies within the level boundaries, else objectQueue will not be accessed correctly
        // Objects are sometimes given coordinates outside the boundaries (e.g. -1,-1) in order to make them "invisible"
        if (x >= 0 && x < levelData->GetWidth() && y >= 0 && y < levelData->GetHeight())
            objectQueue[x + y * levelData->GetWidth()].push_back(c);
    }

    // Loop through the 2D array of int vectors
    for (int y = 0; y < levelData->GetHeight(); y++) {
        for (int x = 0; x < levelData->GetWidth(); x++) {
            //2D//int s = objectQueue[x][y].size();
            int s = objectQueue[x + y * levelData->GetWidth()].size();
            // Check whether there is at least one object queued to be drawn at this position
            if (s == 1) {
                //2D//game->display->SetDisplayCharacter(x + offsetX, y + offsetY, (objectQueue[x][y]).at(0));
                game->display->SetDisplayCharacter(x + offsetX, y + offsetY, (objectQueue[x + y * levelData->GetWidth()]).at(0));
            } else if (s > 1) {
                // Choose one of the objects to draw based upon the gameClock

                // Scale the time in order to determine how long each object's display character is shown for
                int t = ((int)(game->gameClock->getElapsedTime().asSeconds() / SECONDS_PER_OBJECT));

                // This is the display character shown between every other tile
                int c = TILE_PLUS;

                // Multiply by 2 because every other character will be TILE_PLUS by default
                //2D//int objectToDraw = t % (objectQueue[x][y].size() * 2);
                int objectToDraw = t % (objectQueue[x + y * levelData->GetWidth()].size() * 2);

                // Set every other tile to the display character of the relevant object
                // Divide by two because objectToDraw was multiplied by two earlier
                if (objectToDraw % 2 == 0)
                    c = (objectQueue[x + y * levelData->GetWidth()]).at(objectToDraw/2);
                    //2D//c = (objectQueue[x][y]).at(objectToDraw/2);

                // Actually update display
                game->display->SetDisplayCharacter(x + offsetX, y + offsetY, c);
            }
        }
    }

    // Draw the timeline
    timeline->UpdateDisplay(game, this);

    // Draw the titles
    DrawTitles(game);

    // Draw the victory/failure things on top
    endGame->UpdateDisplay(game, this);
}

void LevelManager::DrawTitles(GameEngine* game) {
    std::string titles[LEVEL_NUM_TITLES];
    titles[0] = levelData->GetTitle();
    titles[1] = levelData->GetSubtitle(1);
    titles[2] = levelData->GetSubtitle(2);
    titles[3] = levelData->GetSubtitle(3);

    // Decorate title[0]
    std::ostringstream os;
    os << "/// " << titles[0] << " \\\\\\";
    titles[0] = os.str();

    int xm, x, y;

    xm = game->display->GetWidth() / 2;

    for (int i = 0; i < LEVEL_NUM_TITLES; i++) {
        if ((signed)titles[i].size() >= 0) {
            x = xm - titles[i].size() / 2;

            // Have a gap between the title (i==0) and the subtitles (i>0)
            if (i == 0)
                y = 1;
            else
                y = 2 + i;

            game->display->WriteText(x, y, titles[i].c_str());
        }
    }
}

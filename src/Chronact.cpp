#include "Chronact.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include "GameEngine.hpp"
#include "states/EntryState.hpp"
#include "Logger.hpp"
#include "Defs.hpp"

Chronact::Chronact() {}
Chronact::~Chronact() {}

void Chronact::Go() {
    // Load configs
    ReadConfig();

    // Enter main loop
    MainLoop();
}

void Chronact::MainLoop() {

    // Create game engine
    GameEngine* game = new GameEngine();
    game->Init("Iteration II", fullscreen, maintainAspectRatio, scale, useShaders);

    // Change game state to the entry state, which will load the appropriate game content
    game->ChangeState(EntryState::Instance());

    // Actually enter the main loop
    while(game->Running()) {
        game->ProcessInput();
        game->Update();
        game->RenderFrame();
        game->Pause();
        game->CheckFlags();
    }

    game->CleanUp();
}

void Chronact::ReadConfig() {
    std::cout << "Reading config.txt" << std::endl;

    // Set defaults
    useShaders = true;
    fullscreen = false;
    maintainAspectRatio = true;
    scale.x = 1.0f;
    scale.y = 1.0f;

    // Attempt to read file
    std::string line;
    std::ifstream myfile(CONFIG_FILENAME);
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            // Work out which setting this is for
            int equalsAt = -1;
            for (int i = 0; i < (int)line.length(); i++) {
                if (line[i] == '=') {
                    equalsAt = i;
                    break;
                }
            }

            // There was an equals so read the setting name and value
            if (equalsAt > 0) {
                std::string settingName = line.substr(0, equalsAt);
                std::string settingValue = line.substr(equalsAt + 1);

                if (settingName == "shaders")               useShaders = DecodeValueYesNo(settingValue);
                if (settingName == "maintain_aspect_ratio") maintainAspectRatio = DecodeValueYesNo(settingValue);
                if (settingName == "fullscreen")            fullscreen = DecodeValueYesNo(settingValue);
                if (settingName == "fullscreen_scale_x")    scale.x = DecodeValueFloat(settingValue);
                if (settingName == "fullscreen_scale_y")    scale.y = DecodeValueFloat(settingValue);
            }
        }
        myfile.close();
    } else {
        std::cout << "Cannot open config.txt: reverting to defaults" << std::endl;
    }

    // Force scale to 1:1 if not fullscreen because scaling does not resize the window
    if (!fullscreen) {
        scale.x = 1.0f;
        scale.y = 1.0f;
    }

    // Print out chosen configuration settings
    std::cout << std::endl << "Launching with the following settings:" << std::endl << std::endl;
    std::cout << "                Shaders = " << (useShaders ? "yes" : "no") << std::endl;
    std::cout << "  Maintain aspect ratio = " << (maintainAspectRatio ? "yes" : "no") << std::endl;
    std::cout << "             Fullscreen = " << (fullscreen ? "yes" : "no") << std::endl;
    std::cout << "     Fullscreen scale x = " << (fullscreen ? FloatToString(scale.x) : "N/A") << std::endl;
    std::cout << "     Fullscreen scale y = " << (fullscreen ? FloatToString(scale.y) : "N/A") << std::endl;
    std::cout << std::endl;
}

bool Chronact::DecodeValueYesNo(std::string s) {
    bool v = false;

    if (s == "yes")
        v = true;
    else if (s == "no")
        v = false;
    else {
        std::cout << "WARNING: Value was not 'yes' or 'no'. Defaulting to 'no'" << std::endl;
        v = false;
    }

    return v;
}

float Chronact::DecodeValueFloat(std::string s) {
    return std::atof(s.c_str());
}

std::string Chronact::FloatToString(float f) {
    std::ostringstream os;
    os << f;
    return os.str();
}

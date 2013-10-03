#include "Chronact.hpp"

#include "Logger.hpp"

Chronact::Chronact() {
}

Chronact::~Chronact() {
    // Clear used resources
    delete display;
    delete window;
}

bool Chronact::Init() {
    display = new Display();
    window = new sf::RenderWindow(sf::VideoMode(display->GetPixelWidth(), display->GetPixelHeight(), 32), "Iteration II");

    // Verify the window was created
    if (!window)
        return false;

    return true;
}

void Chronact::RenderFrame() {
    window->clear();
    display->Render(window);
    window->display();
}

void Chronact::ProcessInput() {
    sf::Event event;

    // Loop through all window events
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }
}

void Chronact::MainLoop() {
    // Loop until our window is closed
    while (window->isOpen()) {
        deltaTime = clock.restart();
        ProcessInput();
        Update();
        RenderFrame();
    }
}

void Chronact::Go() {
    printf("Running Chronact engine\n");

    if (!Init())
        throw EngineInitError();

    MainLoop();
}

void Chronact::Update() {
    switch (room) {
    case Room::Title:
        RoomTitle();
        break;
    case Room::Blank:
        RoomBlank();
        break;
    }
}

void Chronact::RoomTitle() {
    display->DrawBackground();
    display->DrawTitle();
}

void Chronact::RoomBlank() {
    display->DrawBackground();
}

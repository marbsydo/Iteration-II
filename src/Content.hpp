#ifndef CONTENT_HPP_INCLUDED
#define CONTENT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

class GameEngine;

enum class ContentType {Title, Level, Scene};

struct ContentItem {
    ContentType type;
    std::string filename;
};

class Content {
public:
    Content(GameEngine* game);
    void Reset();

    GameEngine* game;

    bool AtValidContent();
    void Next();
    void Load();
    void LoadOutline();

    // Content list
    std::vector<ContentItem> contentList;
    int contentPosition;
    void AddContent(ContentItem content);
    ContentType GetCurrentContentType();
    std::string GetCurrentContentFilename();
};

#endif // CONTENT_HPP_INCLUDED

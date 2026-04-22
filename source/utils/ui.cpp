#include "ui.hpp"
#include "system.hpp"

#include <switch.h>
#include <Aether/Aether.hpp>
#include <vector>

Aether::Colour TEXT_COLOUR = Aether::Colour{255, 255, 255, 255};
Aether::Colour MUTED_TEXT_COLOUR = Aether::Colour{170, 170, 170, 255};

Aether::Colour SEPARATOR_COLOUR = Aether::Colour{50, 50, 60, 255};
Aether::Colour BACKGROUND_COLOUR = Aether::Colour{20, 20, 25, 255};
Aether::Colour DEBUG_COLOUR = Aether::Colour{5, 5, 10, 255};

unsigned int TOP_SEPARATOR_Y = 101;
unsigned int BOTTOM_SEPARATOR_Y = 647;

unsigned int MIN_SEPARATOR_X = 40;
unsigned int MAX_SEPARATOR_X = 1200;

std::vector<Aether::ListComment*> debugOutputs;

std::pair<Aether::Rectangle*, Aether::Rectangle*> addSeparators(Aether::Screen* screen){
    // a slightly blue grey
    Aether::Colour SEPARATOR_COLOUR = Aether::Colour{50, 50, 60, 255};

    // create top separator
    Aether::Rectangle* topSeparator = new Aether::Rectangle(40, 101, 1200, 1);
    topSeparator->setColour(SEPARATOR_COLOUR);

    // create bottom separator
    Aether::Rectangle* bottomSeparator = new Aether::Rectangle(30, 647, 1200, 1);
    bottomSeparator->setColour(SEPARATOR_COLOUR);

    // add the separators to the screen
    screen->addElement(topSeparator);
    screen->addElement(bottomSeparator);

    return {topSeparator, bottomSeparator};
}

Aether::Text* addTime(Aether::Screen* screen){
    // create the text
    Aether::Text* timeText = new Aether::Text(60, 26, getCurrentTime(), 32);
    // set the colour
    timeText->setColour(TEXT_COLOUR);
    // add it to the screen
    screen->addElement(timeText);

    // return the pointer for modification
    return timeText;
}

Aether::Rectangle* addBackground(Aether::Screen* screen){
    // create the background
    Aether::Rectangle* background = new Aether::Rectangle(0, 0, 1280, 720);
    // set the colour
    background->setColour(BACKGROUND_COLOUR);
    // add it to the screen
    screen->addElement(background);

    return background;
}

Aether::Text* addFirmware(Aether::Screen* screen){
    // create the text element
    Aether::Text* systemFirmwareText = new Aether::Text(60, 66, getSystemFirmware(), 18);
    // set the colour
    systemFirmwareText->setColour(MUTED_TEXT_COLOUR);
    // add it to the screen
    screen->addElement(systemFirmwareText);

    return systemFirmwareText;
}

Aether::ControlBar* addControlBar(Aether::Screen* screen){
    // create the control bar element
    Aether::ControlBar* controlBar = new Aether::ControlBar();
    // add A as a control option
    controlBar->addControl(Aether::Button::A, "Select");
    // add B as a control option
    controlBar->addControl(Aether::Button::B, "Exit");
    // set control mode colours
    controlBar->setEnabledColour(TEXT_COLOUR);
    controlBar->setDisabledColour(MUTED_TEXT_COLOUR);
    // add it to the screen
    screen->addElement(controlBar);

    return controlBar;
}

Aether::Text* addScreenTitle(Aether::Screen* screen, std::string screenName){
    // create the text element
    Aether::Text* screenTitle = new Aether::Text(640, 36, screenName, 36);
    screenTitle->setX(screenTitle->x()-screenTitle->w()/2); // centre it
    // set its colour
    screenTitle->setColour(TEXT_COLOUR);
    // add it to the screen
    screen->addElement(screenTitle);

    return screenTitle;
}

void updateTime(Aether::Text* timeText){
    // get the current time
    std::string currentTime = getCurrentTime();
    // set the label's text to the current time
    timeText->setString(currentTime);
}

TemplateElements createScreenTemplate(Aether::Screen* screen, std::string screenName){
    // add the background
    Aether::Rectangle* background = addBackground(screen);
    // add the separators
    auto [topSeparator, bottomSeparator] = addSeparators(screen);
    // add the time text
    Aether::Text* timeText = addTime(screen);
    // add the firmware text
    Aether::Text* firmwareText = addFirmware(screen);
    // add the control bar
    Aether::ControlBar* controlBar = addControlBar(screen);
    // add the title
    Aether::Text* screenTitle = addScreenTitle(screen, screenName);

    // return the elements for easy modification
    return {
        topSeparator,
        bottomSeparator,
        timeText,
        firmwareText,
        controlBar,
        background,
        screenTitle
    };
}

Aether::MessageBox* askUserQuestion(Aether::Screen* screen, std::string question, std::string optionA, std::function<void()> functionA, std::string optionB, std::function<void()> functionB){
    // create the messagebox
    Aether::MessageBox* messageBox = new Aether::MessageBox();

    // set up the buttons and their callbacks
    messageBox->addLeftButton(optionA, [messageBox, functionA, screen](){
        functionA();
        messageBox->emptyBody();
        messageBox->close();
        screen->removeElement(messageBox);
    });

    messageBox->addRightButton(optionB, [messageBox, functionB, screen](){
        functionB();
        messageBox->emptyBody();
        messageBox->close();
        screen->removeElement(messageBox);
    });

    messageBox->setRectangleColour(BACKGROUND_COLOUR);
    messageBox->setLineColour(SEPARATOR_COLOUR);

    // set the question
    int bw, bh;
    messageBox->getBodySize(&bw, &bh);
    Aether::Element* body = new Aether::Element(0, 0, bw, bh);
    Aether::TextBlock* text = new Aether::TextBlock(50, 40, question, 24, bw-100);
    text->setColour(TEXT_COLOUR);
    body->addElement(text);
    messageBox->setBodySize(bw, text->y() + text->h() + 40);
    messageBox->setBody(body);

    
    // add it to the screen
    screen->addElement(messageBox);
    screen->setFocused(messageBox);

    return messageBox;
}

std::pair<Aether::Rectangle*, Aether::List*> addDebugBox(Aether::Screen* screen){
    // create the background rect for the debug box
    Aether::Rectangle* backgroundBox = new Aether::Rectangle(800, TOP_SEPARATOR_Y+20, 400, 506, 16);
    backgroundBox->setColour(DEBUG_COLOUR);

    // create the list element
    Aether::List* debugBox = new Aether::List(800, TOP_SEPARATOR_Y+20, 400, 506, Aether::Padding::Default);

    // add elements to the screen
    screen->addElement(backgroundBox);
    screen->addElement(debugBox);

    return {backgroundBox, debugBox};
}

std::pair<Aether::Rectangle*, Aether::List*> generateDebugBox(){
    // create the background rect for the debug box
    Aether::Rectangle* backgroundBox = new Aether::Rectangle(800, TOP_SEPARATOR_Y+20, 400, 506, 16);
    backgroundBox->setColour(DEBUG_COLOUR);

    // create the list element
    Aether::List* debugBox = new Aether::List(800, TOP_SEPARATOR_Y+20, 400, 506, Aether::Padding::Default);

    // return the pointers
    return {backgroundBox, debugBox};
}

Aether::ListComment* debugPrint(const std::string &text, Aether::List* debugBox){
    // create the comment
    Aether::ListComment* listComment = new Aether::ListComment(text);
    listComment->setTextColour(MUTED_TEXT_COLOUR);
    debugBox->addElement(listComment);

    // output the string to the console too
    log(text);

    // return it
    return listComment;
}

void clearDebugPrints(Aether::List* debugBox){
    // clear the list
    debugBox->removeAllElements();
}

std::string getTextInput(const std::string& hint){
    SwkbdConfig kbd;
    swkbdCreate(&kbd, 0);
    swkbdConfigMakePresetDefault(&kbd);
    swkbdConfigSetGuideText(&kbd, hint.c_str());

    char buf[256] = {0};
    swkbdShow(&kbd, buf, sizeof(buf));
    swkbdClose(&kbd);

    return std::string(buf);
}
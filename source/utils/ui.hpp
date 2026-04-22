#pragma once
#include <Aether/Aether.hpp>
#include <switch.h>

std::pair<Aether::Rectangle*, Aether::Rectangle*> addSeparators(Aether::Screen* screen);
Aether::Text* addTime(Aether::Screen* screen);
Aether::Text* addFirmware(Aether::Screen* screen);
Aether::ControlBar* addControlBar(Aether::Screen* screen);
Aether::Rectangle* addBackground(Aether::Screen* screen);
Aether::Text* addScreenTitle(Aether::Screen* screen, std::string screenName);
Aether::MessageBox* askUserQuestion(Aether::Screen* screen, std::string question, std::string optionA, std::function<void()> functionA, std::string optionB, std::function<void()> functionB);
std::pair<Aether::Rectangle*, Aether::List*> addDebugBox(Aether::Screen* screen);
std::pair<Aether::Rectangle*, Aether::List*> generateDebugBox();
Aether::ListComment* debugPrint(const std::string &text, Aether::List* debugBox);
std::string getTextInput(const std::string& hint="");

void clearDebugPrints(Aether::List* debugBox);

void updateTime(Aether::Text* timeText);

struct TemplateElements {
    Aether::Rectangle* topSeparator;
    Aether::Rectangle* bottomSeparator;
    Aether::Text* timeText;
    Aether::Text* firmwareText;
    Aether::ControlBar* controlBar;
    Aether::Rectangle* background;
    Aether::Text* screenTitle;
};

TemplateElements createScreenTemplate(Aether::Screen* screen, std::string screenName);
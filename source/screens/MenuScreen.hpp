#pragma once

#include <switch.h>
#include <Aether/Aether.hpp>

class MenuScreen : public Aether::Screen{
    private:
        TemplateElements myElements;
        Aether::List* debugBox;
        std::string pendingAction;
        const std::string LOG_PATH = "sdmc:/ttdata/webtesthostdetails.csv";
        unsigned int lastTimeUpdate = 0;
        Aether::Window* window;

    public:
        MenuScreen(Aether::Window* window);
        ~MenuScreen();

        bool pingCheck();
        void setHostDetails();
        std::pair<const std::string, const std::string> getHostDetails();
        std::string formPostRequest(const std::string& dest_ip, const std::string& body);
        
        void onLoad() override;
        void onUnload() override;
        void update(unsigned int dt) override;
};
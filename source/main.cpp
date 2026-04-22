// libnx
#include <switch.h>

// UI library
#include <Aether/Aether.hpp>
#include <utils/ui.hpp>

#include <utils/system.hpp>

// screens
#include <screens/MenuScreen.hpp>

int main(){
    // create a window
    Aether::Window* window = new Aether::Window("NX-Web-Test", 1280, 720, [](const std::string message, const bool important){
        log("[Aether] INFO - "+message);
    }); // the last argument is a lambda for handling aether specific output

    log("[NXWEB] INFO - Window created");

    MenuScreen* menu = new MenuScreen(window);

    log("[NXWEB] INFO - Menu screen created");

    socketInitializeDefault();
    log("[NXWEB] INFO - Socket service enabled");
    
    window->showScreen(menu);
    // window->pushScreen(); // push the screen to the stack

    log("[NXWEB] INFO - Screen showed");

    while (window->loop()){

    }


    log("[NXWEB] INFO - Main loop exited, ending program flow");

    socketExit();
    log("[NXWEB] INFO - Socket service disabled");

    delete window;
    return 0;
}
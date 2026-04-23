#include <switch.h>
#include <Aether/Aether.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../utils/ui.hpp"
#include "../utils/system.hpp"
#include "MenuScreen.hpp"

MenuScreen::MenuScreen(Aether::Window* window) : window(window){
    // do literally nothing
}

MenuScreen::~MenuScreen(){
    // stop the compiler from crying again (I don't know why I'm even defining these functions in the header atp it's just muscle memory)
}

void MenuScreen::setHostDetails(){
    const std::string host_ip = getTextInput("IP address of the host:");
    const std::string host_port_str = getTextInput("Port of the host:");

    if (host_ip.length() > 0 && host_port_str.length() > 0){
        writeCSV(this->LOG_PATH, std::vector<std::string> {host_ip, host_port_str});
        debugPrint("Data successfully written!", this->debugBox);
    }
    else{
        debugPrint("No data was overwritten.", this->debugBox);
    }

}

std::pair<const std::string, const std::string> MenuScreen::getHostDetails(){
    std::vector<std::string> data = readCSVRow(this->LOG_PATH, 0);
    
    if (data.size() == 2){
        return std::pair<std::string, std::string> {data[0], data[1]};
    }
    else{
        return std::pair<std::string, std::string> {"", ""};
    }
}

std::string MenuScreen::formPostRequest(const std::string& dest_ip, const std::string& body){
    std::string json = "{\"message\": \"" + body + "\"}";

    std::string request = 
        "POST / HTTP/1.1\r\n"
        "Host: " + dest_ip + "\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(json.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + json;
    
    return request;
}

bool MenuScreen::parsePort(const std::string& port_str, unsigned int& out_port){
    errno = 0;

    char* end;
    unsigned long value = std::strtoul(port_str.c_str(), &end, 10);

    if (end == port_str.c_str()){
        // no digits were found
        return false;
    }
    if (*end != '\0'){
        // junk data
        return false;
    }
    if (errno == ERANGE || value > UINT16_MAX){
        // too large
        return false;
    }

    out_port = static_cast<unsigned int>(value);
    return true;
}

bool MenuScreen::pingCheck(){
    const std::string data_to_send = getTextInput("Data to send:");

    auto [host_ip, host_port_str] = getHostDetails();

    // prevent crashes when attempting to connect to null IPs or ports
    if (host_ip.length() < 1){
        debugPrint("No host IP address provided.", this->debugBox);
        return false;
    }

    if (host_port_str.length() < 1){
        debugPrint("No host port provided.", this->debugBox);
        return false;
    }
    
    unsigned int host_port;
    bool success = parsePort(host_port_str, host_port);

    if (!success){
        debugPrint("Invalid port, expected unsigned int.", this->debugBox);
        return false;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(host_port);

    inet_pton(AF_INET, host_ip.c_str(), &server.sin_addr);

    // set socket to be non-blocking so connect returns instantly
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    connect(sock, (sockaddr*)&server, sizeof(server)); // returns an EINPROGRESS

    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(sock, &write_fds);

    // set timeouts
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // if returned -ve error code, connection failed, now waits for timeout of 5 seconds
    if (select(sock + 1, nullptr, &write_fds, nullptr, &timeout) <= 0){
        debugPrint("Connection failed.", this->debugBox);
        close(sock);
        return false;
    }

    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);

    if (err != 0){
        debugPrint("Connection failed.", this->debugBox);
        close(sock);
        return false;
    }

    fcntl(sock, F_SETFL, flags);

    // set send and rcv timeouts
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    // form the raw get request
    std::string request = formPostRequest(host_ip, data_to_send);
    
    // send the data to the router
    send(sock, request.c_str(), request.size(), 0);
    
    // create the rcv buffer
    char buf[1024];
    int bytes;

    while ((bytes = recv(sock, buf, sizeof(buf) - 1, 0)) > 0){
        buf[bytes] = '\0';

        std::string msg(buf);

        debugPrint(msg, debugBox);
    }

    // close the connection
    close(sock);

    // all has succeeded
    return true;
}

void MenuScreen::onLoad(){
    // create the template
    myElements = createScreenTemplate(this, "NX HTTP Test");

    auto [bg, debugBox] = addDebugBox(this);
    this->debugBox = debugBox;

    // create a button
    Aether::BorderButton* sendPayloadButton = new Aether::BorderButton(1280/2 - 192, 720/2, 500, 80, 3, "Send Payload", 22, [this](){
        // ping the host
        bool success = pingCheck();

        if (success){
            debugPrint("Request successful.", this->debugBox);
        }
        else{
            debugPrint("Request failed.", this->debugBox);
        }
    });

    // set the text colour
    sendPayloadButton->setTextColour(Aether::Colour{255, 255, 255, 255});
    // set the border colour
    sendPayloadButton->setBorderColour(Aether::Colour{50, 50, 60, 255});

    // centre it
    sendPayloadButton->setXY(sendPayloadButton->x() - sendPayloadButton->w()/2, (sendPayloadButton->y() - sendPayloadButton->h()/2) - 100);

    // create another button
    Aether::BorderButton* setHostDetailsButton = new Aether::BorderButton(1280/2 - 192, 720/2, 500, 80, 3, "Set Host Details", 22, [this](){
        // ask the user to input the host details
        setHostDetails();
    });

    setHostDetailsButton->setTextColour(Aether::Colour(255, 255, 255, 255));
    setHostDetailsButton->setBorderColour(Aether::Colour{50, 50, 60, 255});

    setHostDetailsButton->setXY(setHostDetailsButton->x() - setHostDetailsButton->w()/2, setHostDetailsButton->y() - setHostDetailsButton->h()/2);

    // push the elements to the screen
    this->addElement(sendPayloadButton);
    this->addElement(setHostDetailsButton);

    // focus the top button
    this->setFocused(sendPayloadButton);
}

void MenuScreen::onUnload(){
    // stop compiler from cryingc hcek ou ma new thingngg!!1!!1!!!11!!!1
}

void MenuScreen::update(unsigned int dt){
    static unsigned int accumulatedTime = 0;
    accumulatedTime += dt;

    if (accumulatedTime >= 1000){
        updateTime(this->myElements.timeText);
        accumulatedTime -= 1000;
    }
}
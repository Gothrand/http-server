#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <regex>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include "utils.h"
#include "server.h"

void signal_callback_handler(const int signum)
{
    std::cout << "Caught signal " << signum << std::endl;
    exit(signum);
}

int main()
{
    // create a socket and get that socket's file descriptor
    signal(SIGINT, signal_callback_handler);
    HTTPServer server;

    // begin main server loop
    server.run_forever();

    return 0;
}

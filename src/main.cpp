#include <iostream>
#include <string>
#include <regex>
#include <signal.h>
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
    signal(SIGINT, signal_callback_handler);
    HTTPServer server;

    // begin main server loop
    server.run_forever();

    return 0;
}

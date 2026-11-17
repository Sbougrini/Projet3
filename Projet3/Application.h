#pragma once
#include "CommandLine.h"

class Application
{
public:
    Application() = delete;
    static int run(int argc, char *argv[]);

private:
    static int runEmbed(const Command& command);
    static int runExtract(const Command& command);
};

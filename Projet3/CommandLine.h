#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class Mode
{
    HELP = 0,
    EMBED,
    EXTRACT
};

struct Command
{
    Mode mode = Mode::HELP;
    std::string source;
    std::string destination;
    std::string message;
    std::string key;
};

class CommandLine
{
public:
    CommandLine() = delete;
    static bool verifyCommandLine(int argc, char *argv[], Command &command,
                                  std::string &error);
    static void printHelp();

private:
    static bool verifyHelp(const std::vector<std::string> &arguments, Command &command,
                           std::string &error);
    static bool verifyEmbed(const std::vector<std::string> &arguments, Command &command,
                            std::string &error);
    static bool verifyExtract(const std::vector<std::string> &arguments, Command &command,
                              std::string &error);
    static bool verifyKey(const std::vector<std::string> &arguments, Command &command,
                          std::string &error, std::size_t keyIndex);

    static constexpr std::size_t NO_COMMAND_ARGUMENT_COUNT = 1;
    static constexpr std::size_t HELP_ARGUMENT_COUNT = 2;
    static constexpr std::size_t EXTRACT_ARGUMENT_COUNT = 3;
    static constexpr std::size_t EMBED_ARGUMENT_COUNT = 5;
    static constexpr std::size_t KEY_OPTION_ARGUMENT_COUNT = 2;
    static constexpr char KEY_OPTION[] = "--key";
};
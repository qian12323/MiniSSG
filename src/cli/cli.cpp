#include "cli/cli.h"
#include "cli/command.h"

#include <iostream>
#include <cstring>

namespace minissg
{
namespace cli
{

extern const Command cmdBuild;
extern const Command cmdNew;
extern const Command cmdServe;

static const Command* kCommands[] = { &cmdBuild, &cmdNew, &cmdServe };
static const int kCommandCount = sizeof(kCommands) / sizeof(kCommands[0]);

void printUsage()
{
    std::cout << "MiniSSG - A minimal static site generator\n\nUsage:\n";

    for (int i = 0; i < kCommandCount; ++i)
    {
        auto& c = *kCommands[i];
        std::string line = "  minissg " + std::string(c.usage);
        if (line.size() < 38) line += std::string(38 - line.size(), ' ');
        std::cout << line << c.desc << "\n";
    }

    std::cout << "  minissg help" << std::string(24, ' ') << "Show this help\n\n"
              << "Options:\n"
              << "  -c  Path to config file (default: config.yaml)\n"
              << "  -p  Port for dev server (default: 8080)\n";
}

int run(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    std::string name = argv[1];

    if (name == "help" || name == "--help" || name == "-h")
    {
        printUsage();
        return 0;
    }

    for (int i = 0; i < kCommandCount; ++i)
    {
        if (name == kCommands[i]->name)
        {
            kCommands[i]->run(argc - 1, argv + 1);
            return 0;
        }
    }

    std::cerr << "Unknown command: " << name << "\n";
    printUsage();
    return 1;
}

} // namespace cli
} // namespace minissg

#include "cli/cli.h"

#include <iostream>
#include <string>
#include <cstring>
#include "builder/builder.h"

namespace minissg
{
namespace cli
{

void printUsage()
{
    std::cout <<
        "MiniSSG - A minimal static site generator\n\n"
        "Usage:\n"
        "  minissg build [-c config]     Build the site\n"
        "  minissg new \"Title\"            Create a new post\n"
        "  minissg serve [-p port]        Start dev server\n"
        "  minissg help                   Show this help\n\n"
        "Options:\n"
        "  -c  Path to config file (default: config.yaml)\n"
        "  -p  Port for dev server (default: 8080)\n";
}

int run(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "help" || cmd == "--help" || cmd == "-h")
    {
        printUsage();
        return 0;
    }

    if (cmd == "build")
    {
        std::string configPath = "config.yaml";
        for (int i = 2; i < argc; ++i)
        {
            if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0) && i + 1 < argc)
                configPath = argv[++i];
        }

        auto config = loadConfig(configPath);
        build(config);
        return 0;
    }

    if (cmd == "new")
    {
        if (argc < 3)
        {
            std::cerr << "Error: 'new' requires a title.\n";
            std::cerr << "  minissg new \"My Post Title\"\n";
            return 1;
        }

        std::string title = argv[2];
        std::string configPath = "config.yaml";
        std::string sourceDir = "posts";

        for (int i = 3; i < argc; ++i)
        {
            if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0) && i + 1 < argc)
                configPath = argv[++i];
        }

        auto config = loadConfig(configPath);
        if (!config.sourceDir.empty())
            sourceDir = config.sourceDir;

        extern void commandNew(const std::string& title, const std::string& sourceDir);
        commandNew(title, sourceDir);
        return 0;
    }

    if (cmd == "serve")
    {
        int port = 8080;
        std::string configPath = "config.yaml";

        for (int i = 2; i < argc; ++i)
        {
            if ((std::strcmp(argv[i], "-p") == 0 || std::strcmp(argv[i], "--port") == 0) && i + 1 < argc)
                port = std::stoi(argv[++i]);
            else if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0) && i + 1 < argc)
                configPath = argv[++i];
        }

        auto config = loadConfig(configPath);
        std::string outputDir = config.outputDir.empty() ? "output" : config.outputDir;

        extern void commandServe(const std::string& rootDir, int port);
        commandServe(outputDir, port);
        return 0;
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    printUsage();
    return 1;
}

} // namespace cli
} // namespace minissg

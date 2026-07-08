#include "cli/command.h"

#include <cstring>
#include <iostream>
#include "builder/builder.h"

namespace minissg
{
namespace cli
{

static void run(int argc, char* argv[])
{
    const char* configPath = "config.yaml";
    for (int i = 1; i < argc; ++i)
    {
        if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0) && i + 1 < argc)
            configPath = argv[++i];
    }

    auto cfg = loadConfig(configPath);
    build(cfg);
}

extern const Command cmdBuild = {"build", "Build the site", "build [-c config]", run};

} // namespace cli
} // namespace minissg

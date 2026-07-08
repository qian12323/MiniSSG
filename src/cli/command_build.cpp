#include "cli/cli.h"
#include "builder/builder.h"

namespace minissg
{
namespace cli
{

void cmdBuild(const std::string& configPath)
{
    auto cfg = loadConfig(configPath);
    build(cfg);
}

} // namespace cli
} // namespace minissg

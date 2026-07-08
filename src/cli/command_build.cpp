#include "cli/cli.h"
#include "builder/builder.h"

namespace minissg
{
namespace cli
{

void cmdBuild(const std::string& configPath, bool fixHeadings, bool autoNumber)
{
    auto cfg = loadConfig(configPath);
    build(cfg, fixHeadings, autoNumber);
}

} // namespace cli
} // namespace minissg

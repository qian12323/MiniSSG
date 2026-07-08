#include "cli/cli.h"
#include "builder/builder.h"

#include <iostream>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

namespace minissg
{
namespace cli
{

static std::string extractSlug(const std::string& filename)
{
    std::string name = filename;
    auto dot = name.rfind(".md");
    if (dot != std::string::npos) name = name.substr(0, dot);

    if (name.size() > 11 && name[4] == '-' && name[7] == '-' && name[10] == '-')
        name = name.substr(11);

    return name;
}

void cmdClean(bool dryRun, const std::string& configPath)
{
    auto cfg = loadConfig(configPath);
    std::string src = cfg.sourceDir.empty() ? "posts" : cfg.sourceDir;
    std::string out = cfg.outputDir.empty() ? "output" : cfg.outputDir;

    std::unordered_set<std::string> expected;
    if (fs::exists(src))
    {
        for (auto& entry : fs::directory_iterator(src))
        {
            if (entry.path().extension() != ".md") continue;
            expected.insert(extractSlug(entry.path().filename().string()) + ".html");
        }
    }

    if (!fs::exists(out))
    {
        std::cout << "Output directory does not exist: " << out << "\n";
        return;
    }

    int removed = 0;
    for (auto& entry : fs::directory_iterator(out))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".html") continue;

        std::string name = entry.path().filename().string();
        if (name == "index.html") continue;

        if (expected.find(name) == expected.end())
        {
            if (dryRun)
            {
                std::cout << "Would remove: " << entry.path().string() << "\n";
                ++removed;
            }
            else
            {
                fs::remove(entry.path());
                std::cout << "Removed: " << entry.path().string() << "\n";
                ++removed;
            }
        }
    }

    if (removed == 0)
        std::cout << "Nothing to clean.\n";
    else if (!dryRun)
        std::cout << "Cleaned " << removed << " stale file(s).\n";
}

} // namespace cli
} // namespace minissg

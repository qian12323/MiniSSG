#include "cli/cli.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace minissg
{
namespace cli
{

struct EmbeddedFile { const char* path; const char* content; };
extern const EmbeddedFile kThemeFiles[];
extern const int kThemeFileCount;

void cmdNewSite(const std::string& siteName)
{
    fs::path root = siteName;

    if (fs::exists(root))
    {
        std::cerr << "Error: directory '" << siteName << "' already exists.\n";
        return;
    }

    fs::create_directories(root);
    fs::create_directories(root / "posts");
    fs::create_directories(root / "output");

    for (int i = 0; i < kThemeFileCount; ++i)
    {
        auto& f = kThemeFiles[i];
        fs::path dest = root / f.path;
        fs::create_directories(dest.parent_path());

        std::ofstream out(dest, std::ios::binary);
        out << f.content;
        if (!out)
        {
            std::cerr << "Error: failed to write " << dest.string() << "\n";
            return;
        }
    }

    std::cout << "Site created: " << root.string() << "\n";
    std::cout << "  cd " << siteName << " && minissg build\n";
}

} // namespace cli
} // namespace minissg

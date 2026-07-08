#include "cli/command.h"

#include <ctime>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

#include "builder/builder.h"

namespace fs = std::filesystem;

namespace minissg
{
namespace cli
{

static std::string currentDate()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

static std::string slugify(const std::string& title)
{
    std::string s;
    for (char c : title)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ')
            s += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        else if (c == '-' || c == '_')
            s += c;
    }
    for (auto& c : s)
        if (c == ' ') c = '-';

    std::string result;
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '-' && (i == 0 || s[i - 1] == '-'))
            continue;
        result += s[i];
    }
    while (!result.empty() && result.back() == '-')
        result.pop_back();

    return result;
}

static void run(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: 'new' requires a title.\n";
        std::cerr << "  minissg new \"My Post Title\"\n";
        return;
    }

    std::string title = argv[1];

    const char* configPath = "config.yaml";
    for (int i = 2; i < argc; ++i)
    {
        if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0) && i + 1 < argc)
            configPath = argv[++i];
    }

    auto cfg = loadConfig(configPath);
    std::string sourceDir = cfg.sourceDir.empty() ? "posts" : cfg.sourceDir;

    fs::create_directories(sourceDir);

    std::string filename = currentDate() + "-" + slugify(title) + ".md";
    std::string path = sourceDir + "/" + filename;

    if (fs::exists(path))
    {
        std::cerr << "File already exists: " << path << "\n";
        return;
    }

    std::ofstream out(path);
    out << "---\n"
        << "title: \"" << title << "\"\n"
        << "date: " << currentDate() << "\n"
        << "tags: []\n"
        << "---\n\n";

    std::cout << "Created: " << path << "\n";
}

extern const Command cmdNew = {"new", "Create a new post", "new \"Title\" [-c config]", run};

} // namespace cli
} // namespace minissg

#include "cli/cli.h"

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

namespace
{

std::string currentDate()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

std::string slugify(const std::string& title)
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

} // anonymous namespace

void cmdNew(const std::string& title, const std::string& category, const std::string& configPath)
{
    auto cfg = loadConfig(configPath);
    std::string sourceDir = cfg.sourceDir.empty() ? "posts" : cfg.sourceDir;
    std::string targetDir = sourceDir + "/" + category;

    // 检查分类目录是否存在，不存在则不自动创建
    if (!fs::exists(targetDir))
    {
        std::cerr << "Error: category '" << category
                  << "' does not exist in " << sourceDir << "/\n";
        std::cerr << "       Create it first: mkdir " << targetDir << "\n";
        if (fs::exists(sourceDir))
        {
            std::cerr << "       Available: ";
            bool first = true;
            for (auto& d : fs::directory_iterator(sourceDir))
            {
                if (!d.is_directory()) continue;
                if (!first) std::cerr << ", ";
                std::cerr << d.path().filename().string();
                first = false;
            }
            std::cerr << "\n";
        }
        return;
    }

    std::string filename = currentDate() + "-" + slugify(title) + ".md";
    std::string path = targetDir + "/" + filename;

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

} // namespace cli
} // namespace minissg

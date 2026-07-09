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

static bool isProtected(const std::string& rel)
{
    return rel == "index.html"
        || rel == "tags.html"
        || rel == "categories.html"
        || rel.rfind("tags/", 0) == 0
        || rel.rfind("categories/", 0) == 0
        || rel.rfind("assets/", 0) == 0;
}

void cmdClean(bool dryRun, const std::string& configPath)
{
    auto cfg = loadConfig(configPath);
    std::string src = cfg.sourceDir.empty() ? "posts" : cfg.sourceDir;
    std::string out = cfg.outputDir.empty() ? "output" : cfg.outputDir;

    // 扫描源目录，构建预期的输出路径集合
    std::unordered_set<std::string> expected;
    if (fs::exists(src))
    {
        fs::path srcRoot = src;
        for (auto& entry : fs::recursive_directory_iterator(src))
        {
            if (!entry.is_regular_file()) continue;

            std::string rel = fs::relative(entry.path(), srcRoot).string();

            if (entry.path().extension() == ".md")
            {
                // .md 文件 → output/{category}/{slug}.html
                std::string slug = extractSlug(entry.path().filename().string());
                auto sep = rel.find('/');
                std::string category = (sep != std::string::npos)
                    ? rel.substr(0, sep) : "other";
                expected.insert(category + "/" + slug + ".html");
            }
            else
            {
                // 静态文件 → 原样复制，路径不变
                expected.insert(rel);
            }
        }
    }

    if (!fs::exists(out))
    {
        std::cout << "Output directory does not exist: " << out << "\n";
        return;
    }

    // 扫描输出目录，删除不在预期集合中的文件
    fs::path outRoot = out;
    int removed = 0;
    for (auto& entry : fs::recursive_directory_iterator(out))
    {
        if (!entry.is_regular_file()) continue;

        std::string rel = fs::relative(entry.path(), outRoot).string();
        if (isProtected(rel)) continue;

        if (expected.find(rel) == expected.end())
        {
            if (dryRun)
            {
                std::cout << "Would remove: " << rel << "\n";
                ++removed;
            }
            else
            {
                fs::remove(entry.path());
                std::cout << "Removed: " << rel << "\n";
                ++removed;
            }
        }
    }

    // 清理空目录（倒序删除，避免迭代器失效）
    if (!dryRun)
    {
        std::vector<fs::path> emptyDirs;
        for (auto& entry : fs::recursive_directory_iterator(out))
        {
            if (entry.is_directory() && fs::is_empty(entry.path())
                && !isProtected(fs::relative(entry.path(), outRoot).string()))
                emptyDirs.push_back(entry.path());
        }
        std::sort(emptyDirs.rbegin(), emptyDirs.rend());
        for (auto& d : emptyDirs)
        {
            fs::remove(d);
            std::cout << "Removed empty dir: " << fs::relative(d, outRoot).string() << "\n";
        }
    }

    if (removed == 0)
        std::cout << "Nothing to clean.\n";
    else if (!dryRun)
        std::cout << "Cleaned " << removed << " stale file(s).\n";
}

} // namespace cli
} // namespace minissg

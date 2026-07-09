#include "cli/cli.h"

#include <CLI/CLI.hpp>
#include <cstring>
#include "builder/builder.h"

namespace minissg
{
namespace cli
{

int run(int argc, char* argv[])
{
    // 先提取 config 路径，加载配置作为 CLI 默认值
    std::string configPath = "config.yaml";
    bool needConfig = true;
    for (int i = 1; i < argc; ++i)
    {
        if ((std::strcmp(argv[i], "-c") == 0 || std::strcmp(argv[i], "--config") == 0)
            && i + 1 < argc)
        {
            configPath = argv[++i];
            break;
        }
        if (std::strcmp(argv[i], "newsite") == 0 || std::strcmp(argv[i], "help") == 0
            || std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0)
        {
            needConfig = false;
            break;
        }
    }
    SiteConfig cfg;
    if (needConfig) cfg = loadConfig(configPath);

    CLI::App app{"MiniSSG - A minimal static site generator"};

    app.add_option("-c,--config", configPath, "Config file path")->capture_default_str();
    app.fallthrough();

    auto* buildCmd = app.add_subcommand("build", "Build the site")->alias("b");
    buildCmd->add_option("--fix-headings", cfg.fixHeadings, "Auto-correct heading hierarchy (yes/no)");
    buildCmd->add_option("--fix-headings-number", cfg.autoNumber, "Auto-number headings (yes/no)");
    buildCmd->callback([&] { cmdBuild(configPath, cfg.fixHeadings, cfg.autoNumber); });

    auto* newCmd = app.add_subcommand("new", "Create a new post");
    std::string title;
    std::string category = "other";
    newCmd->add_option("title", title, "Post title")->required();
    newCmd->add_option("--categ", category, "Category (subdirectory name)")->capture_default_str();
    newCmd->callback([&] { cmdNew(title, category, configPath); });

    auto* serveCmd = app.add_subcommand("run", "Start dev server")->alias("r");
    int port = 8080;
    serveCmd->add_option("-p,--port", port, "Port number")->capture_default_str();
    serveCmd->callback([&] { cmdServe(port, configPath); });

    auto* cleanCmd = app.add_subcommand("clean", "Remove stale output files")->alias("c");
    bool dryRun = false;
    cleanCmd->add_flag("-n,--dry-run", dryRun, "Show what would be removed");
    cleanCmd->callback([&] { cmdClean(dryRun, configPath); });

    auto* newsiteCmd = app.add_subcommand("newsite", "Create a new site")->alias("ns");
    std::string siteName;
    newsiteCmd->add_option("-n,--name", siteName, "Site directory name")->required();
    newsiteCmd->callback([&] { cmdNewSite(siteName); });

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);
    return 0;
}

} // namespace cli
} // namespace minissg

#include "cli/cli.h"

#include <CLI/CLI.hpp>

namespace minissg
{
namespace cli
{

int run(int argc, char* argv[])
{
    CLI::App app{"MiniSSG - A minimal static site generator"};

    std::string configPath = "config.yaml";
    app.add_option("-c,--config", configPath, "Config file path")->capture_default_str();
    app.fallthrough();

    auto* buildCmd = app.add_subcommand("build", "Build the site");
    buildCmd->callback([&] { cmdBuild(configPath); });

    auto* newCmd = app.add_subcommand("new", "Create a new post");
    std::string title;
    std::string category = "other";
    newCmd->add_option("title", title, "Post title")->required();
    newCmd->add_option("--categ", category, "Category (subdirectory name)")->capture_default_str();
    newCmd->callback([&] { cmdNew(title, category, configPath); });

    auto* serveCmd = app.add_subcommand("serve", "Start dev server");
    int port = 8080;
    serveCmd->add_option("-p,--port", port, "Port number")->capture_default_str();
    serveCmd->callback([&] { cmdServe(port, configPath); });

    auto* cleanCmd = app.add_subcommand("clean", "Remove stale output files");
    bool dryRun = false;
    cleanCmd->add_flag("-n,--dry-run", dryRun, "Show what would be removed");
    cleanCmd->callback([&] { cmdClean(dryRun, configPath); });

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);
    return 0;
}

} // namespace cli
} // namespace minissg

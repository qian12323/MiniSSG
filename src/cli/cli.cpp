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

    auto* buildCmd = app.add_subcommand("build", "Build the site");
    buildCmd->add_option("-c,--config", configPath, "Config file path")->capture_default_str();
    buildCmd->callback([&] { cmdBuild(configPath); });

    auto* newCmd = app.add_subcommand("new", "Create a new post");
    std::string title;
    newCmd->add_option("title", title, "Post title")->required();
    newCmd->add_option("-c,--config", configPath, "Config file path")->capture_default_str();
    newCmd->callback([&] { cmdNew(title, configPath); });

    auto* serveCmd = app.add_subcommand("serve", "Start dev server");
    int port = 8080;
    serveCmd->add_option("-p,--port", port, "Port number")->capture_default_str();
    serveCmd->add_option("-c,--config", configPath, "Config file path")->capture_default_str();
    serveCmd->callback([&] { cmdServe(port, configPath); });

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);
    return 0;
}

} // namespace cli
} // namespace minissg

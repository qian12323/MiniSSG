#include "builder/builder.h"

#include <yaml-cpp/yaml.h>

namespace minissg
{

SiteConfig loadConfig(const std::string& path)
{
    SiteConfig cfg;
    YAML::Node root;
    try { root = YAML::LoadFile(path); }
    catch (...) { return cfg; }

    if (root["title"])       cfg.title       = root["title"].as<std::string>();
    if (root["description"]) cfg.description = root["description"].as<std::string>();
    if (root["baseUrl"])     cfg.baseUrl     = root["baseUrl"].as<std::string>();
    if (root["sourceDir"])   cfg.sourceDir   = root["sourceDir"].as<std::string>();
    if (root["outputDir"])   cfg.outputDir   = root["outputDir"].as<std::string>();
    if (root["themeDir"])    cfg.themeDir    = root["themeDir"].as<std::string>();
    if (root["fixHeadings"]) cfg.fixHeadings = root["fixHeadings"].as<bool>();
    if (root["autoNumber"])  cfg.autoNumber  = root["autoNumber"].as<bool>();

    return cfg;
}

} // namespace minissg

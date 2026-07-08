#include "builder/builder.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "parser/parser.h"
#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

SiteConfig loadConfig(const std::string& path)
{
    SiteConfig cfg;
    YAML::Node root = YAML::LoadFile(path);

    if (root["title"])       cfg.title       = root["title"].as<std::string>();
    if (root["description"]) cfg.description = root["description"].as<std::string>();
    if (root["baseUrl"])     cfg.baseUrl     = root["baseUrl"].as<std::string>();
    if (root["sourceDir"])   cfg.sourceDir   = root["sourceDir"].as<std::string>();
    if (root["outputDir"])   cfg.outputDir   = root["outputDir"].as<std::string>();
    if (root["themeDir"])    cfg.themeDir    = root["themeDir"].as<std::string>();

    return cfg;
}

void build(const SiteConfig& config)
{
    fs::create_directories(config.outputDir);

    std::string postTpl  = loadTemplate(config.themeDir + "/post.html");
    std::string indexTpl = loadTemplate(config.themeDir + "/index.html");

    std::vector<Article> articles;

    for (auto& entry : fs::directory_iterator(config.sourceDir))
    {
        if (entry.path().extension() != ".md") continue;

        std::string path = entry.path().string();
        std::cout << "Parsing: " << path << std::endl;

        auto art = parseArticle(path);
        articles.push_back(art);

        auto html = renderPost(art, postTpl);
        std::string outPath = config.outputDir + "/" + art.slug + ".html";

        std::ofstream out(outPath);
        out << html;
        std::cout << "  -> " << outPath << std::endl;
    }

    std::sort(articles.begin(), articles.end(),
        [](const Article& a, const Article& b) { return a.date > b.date; });

    auto indexHtml = renderIndex(articles, indexTpl);

    auto rep = [&indexHtml](const std::string& key, const std::string& val) {
        size_t pos = 0;
        while ((pos = indexHtml.find(key, pos)) != std::string::npos)
        {
            indexHtml.replace(pos, key.size(), val);
            pos += val.size();
        }
    };
    rep("{{title}}",       config.title);
    rep("{{description}}", config.description);

    std::ofstream idx(config.outputDir + "/index.html");
    idx << indexHtml;
    std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
}

} // namespace minissg

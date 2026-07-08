#include "builder/builder.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <map>

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
    fs::path srcRoot = config.sourceDir;

    for (auto& entry : fs::recursive_directory_iterator(config.sourceDir))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".md")
            continue;

        std::string path = entry.path().string();
        std::cout << "Parsing: " << path << std::endl;

        auto art = parseArticle(path);

        std::string rel = fs::relative(entry.path(), srcRoot).string();
        auto sep = rel.find('/');
        if (sep != std::string::npos)
            art.category = rel.substr(0, sep);

        std::string outRel = rel;
        outRel.replace(outRel.rfind(".md"), 3, ".html");
        std::string outPath = config.outputDir + "/" + outRel;

        fs::create_directories(fs::path(outPath).parent_path());

        auto html = renderPost(art, postTpl);
        std::ofstream out(outPath);
        out << html;
        std::cout << "  -> " << outPath << std::endl;

        articles.push_back(art);
    }

    std::sort(articles.begin(), articles.end(),
        [](const Article& a, const Article& b) { return a.date > b.date; });

    std::map<std::string, std::vector<Article>> groups;
    for (auto& a : articles)
        groups[a.category].push_back(a);

    for (auto& [cat, vec] : groups)
        std::sort(vec.begin(), vec.end(),
            [](const Article& a, const Article& b) { return a.date > b.date; });

    std::string listHtml;
    for (auto& [cat, vec] : groups)
    {
        std::string name = cat.empty() ? "Other" : cat;
        listHtml += "<h2>" + name + "</h2>\n<ul>\n";
        for (auto& a : vec)
        {
            std::string link = a.category.empty() ? a.slug + ".html"
                                                  : a.category + "/" + a.slug + ".html";
            listHtml += "  <li><a href=\"" + link + "\">" + a.title
                      + "</a> — <span>" + a.date + "</span></li>\n";
        }
        listHtml += "</ul>\n";
    }

    auto rep = [&](const std::string& key, const std::string& val) {
        size_t pos = 0;
        while ((pos = indexTpl.find(key, pos)) != std::string::npos)
        {
            indexTpl.replace(pos, key.size(), val);
            pos += val.size();
        }
    };
    rep("{{title}}",       config.title);
    rep("{{description}}", config.description);
    rep("{{posts}}",       listHtml);

    std::ofstream idx(config.outputDir + "/index.html");
    idx << indexTpl;
    std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
}

} // namespace minissg

#include "builder/builder.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

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

static std::string articleLink(const Article& a)
{
    std::string href = a.category + "/" + a.slug + ".html";
    return "<a href=\"/" + href + "\">" + a.title + "</a>";
}

static std::string makeLink(const Article& a)
{
    return a.category + "/" + a.slug + ".html";
}

static void replace(std::string& tpl, const std::string& key, const std::string& val)
{
    size_t pos = 0;
    while ((pos = tpl.find(key, pos)) != std::string::npos)
    {
        tpl.replace(pos, key.size(), val);
        pos += val.size();
    }
}

void build(const SiteConfig& config)
{
    fs::create_directories(config.outputDir);

    std::string postTpl       = loadTemplate(config.themeDir + "/post.html");
    std::string indexTpl      = loadTemplate(config.themeDir + "/index.html");
    std::string tagsTpl       = loadTemplate(config.themeDir + "/tags.html");
    std::string tagTpl        = loadTemplate(config.themeDir + "/tag.html");
    std::string categoriesTpl = loadTemplate(config.themeDir + "/categories.html");
    std::string categoryTpl   = loadTemplate(config.themeDir + "/category.html");

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
        else
            art.category = "other";

        std::string outRel = art.category + "/" + art.slug + ".html";
        std::string outPath = config.outputDir + "/" + outRel;

        fs::create_directories(fs::path(outPath).parent_path());

        auto html = renderPost(art, postTpl);
        replace(html, "{{siteTitle}}", config.title);

        std::ofstream out(outPath);
        out << html;
        std::cout << "  -> " << outPath << std::endl;

        articles.push_back(art);
    }

    std::sort(articles.begin(), articles.end(),
        [](const Article& a, const Article& b) { return a.date > b.date; });

    // ---- index.html: 按时间倒序 ----
    {
        std::string list;
        for (auto& a : articles)
        {
            list += "<div class=\"post-item\">"
                    "<span class=\"post-date\">" + a.date + "</span>"
                    + articleLink(a) +
                    "</div>\n";
        }
        replace(indexTpl, "{{title}}",       config.title);
        replace(indexTpl, "{{description}}", config.description);
        replace(indexTpl, "{{posts}}",       list);

        std::ofstream out(config.outputDir + "/index.html");
        out << indexTpl;
        std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
    }

    // ---- tags.html: 摘要 (最多2篇) + /tags/{tag}.html 详情 ----
    {
        std::map<std::string, std::vector<Article>> tagMap;
        for (auto& a : articles)
            for (auto& t : a.tags)
                tagMap[t].push_back(a);

        for (auto& [tag, vec] : tagMap)
            std::sort(vec.begin(), vec.end(),
                [](const Article& a, const Article& b) { return a.date > b.date; });

        std::string summary;
        for (auto& [tag, vec] : tagMap)
        {
            int total = static_cast<int>(vec.size());
            summary += "<div class=\"tag-group\">\n"
                       "<h2>" + tag + " <span class=\"tag-count\">("
                       + std::to_string(total) + ")</span></h2>\n<ul>\n";

            int shown = std::min(total, 2);
            for (int i = 0; i < shown; ++i)
                summary += "  <li>" + articleLink(vec[i])
                         + "<span class=\"post-date\">" + vec[i].date + "</span></li>\n";
            if (total > 2)
                summary += "  <li class=\"view-all\"><a href=\"/tags/" + tag + ".html\">View all →</a></li>\n";
            summary += "</ul>\n</div>\n";

            std::string detail = tagTpl;
            std::string postsHtml;
            for (auto& a : vec)
                postsHtml += "  <li>" + articleLink(a)
                           + "<span class=\"post-date\">" + a.date + "</span></li>\n";

            replace(detail, "{{tag}}",       tag);
            replace(detail, "{{siteTitle}}", config.title);
            replace(detail, "{{posts}}",     postsHtml);

            std::string dir = config.outputDir + "/tags";
            fs::create_directories(dir);
            std::ofstream out(dir + "/" + tag + ".html");
            out << detail;
        }

        replace(tagsTpl, "{{title}}",  config.title);
        replace(tagsTpl, "{{groups}}", summary);

        std::ofstream out(config.outputDir + "/tags.html");
        out << tagsTpl;
        std::cout << "Generated: " << config.outputDir << "/tags.html" << std::endl;
    }

    // ---- categories.html: 摘要 (最多2篇) + /categories/{cat}.html 详情 ----
    {
        std::map<std::string, std::vector<Article>> catMap;
        for (auto& a : articles)
            catMap[a.category].push_back(a);

        for (auto& [cat, vec] : catMap)
            std::sort(vec.begin(), vec.end(),
                [](const Article& a, const Article& b) { return a.date > b.date; });

        std::string summary;
        for (auto& [cat, vec] : catMap)
        {
            int total = static_cast<int>(vec.size());
            summary += "<div class=\"cat-group\">\n"
                       "<h2>" + cat + " <span class=\"cat-count\">("
                       + std::to_string(total) + ")</span></h2>\n<ul>\n";

            int shown = std::min(total, 2);
            for (int i = 0; i < shown; ++i)
                summary += "  <li>" + articleLink(vec[i])
                         + "<span class=\"post-date\">" + vec[i].date + "</span></li>\n";
            if (total > 2)
                summary += "  <li class=\"view-all\"><a href=\"/categories/" + cat + ".html\">View all →</a></li>\n";
            summary += "</ul>\n</div>\n";

            std::string detail = categoryTpl;
            std::string postsHtml;
            for (auto& a : vec)
                postsHtml += "  <li>" + articleLink(a)
                           + "<span class=\"post-date\">" + a.date + "</span></li>\n";

            replace(detail, "{{category}}",   cat);
            replace(detail, "{{siteTitle}}",  config.title);
            replace(detail, "{{posts}}",      postsHtml);

            std::string dir = config.outputDir + "/categories";
            fs::create_directories(dir);
            std::ofstream out(dir + "/" + cat + ".html");
            out << detail;
        }

        replace(categoriesTpl, "{{title}}",  config.title);
        replace(categoriesTpl, "{{groups}}", summary);

        std::ofstream out(config.outputDir + "/categories.html");
        out << categoriesTpl;
        std::cout << "Generated: " << config.outputDir << "/categories.html" << std::endl;
    }
}

} // namespace minissg

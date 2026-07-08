#include "builder/builder.h"
#include "build_utils.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "parser/parser.h"
#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

void build(const SiteConfig& config)
{
    fs::create_directories(config.outputDir);

    std::string postTpl = loadTemplate(config.themeDir + "/post.html");

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

    buildIndex(articles, config);
    buildTags(articles, config);
    buildCategories(articles, config);
}

} // namespace minissg

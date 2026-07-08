#include "builder/builder.h"
#include "build_utils.h"

#include <fstream>
#include <iostream>
#include <map>
#include <filesystem>

#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

void buildCategories(const std::vector<Article>& articles, const SiteConfig& config)
{
    std::string categoriesTpl = loadTemplate(config.themeDir + "/categories.html");
    std::string categoryTpl   = loadTemplate(config.themeDir + "/category.html");

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

} // namespace minissg

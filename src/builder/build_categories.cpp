#include "builder/builder.h"
#include "builder/build_utils.h"

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

    // 迭代顺序：other 放最后
    std::vector<std::string> catOrder;
    for (auto& [cat, vec] : catMap)
        if (cat != "other") catOrder.push_back(cat);
    if (catMap.count("other")) catOrder.push_back("other");

    std::string summary;
    for (auto& cat : catOrder)
    {
        auto& vec = catMap[cat];
        int total = static_cast<int>(vec.size());

        summary += "<div class=\"cat-group\">\n"
                   "<h2><a href=\"/categories/" + cat + ".html\">" + cat + "</a> <span class=\"cat-count\">("
                   + std::to_string(total) + ")</span></h2>\n";

        int shown = std::min(total, 2);
        for (int i = 0; i < shown; ++i)
        {
            auto& a = vec[i];
            summary += "<div class=\"index-card\">";
            summary += "<div class=\"card-date\">" + a.date + "</div>";
            summary += "<div class=\"card-body\"><h3>" + articleLink(a) + "</h3>";
            if (!a.excerpt.empty())
                summary += "<p class=\"card-excerpt\">" + a.excerpt + "</p>";
            summary += "<div class=\"card-footer\">"
                        "<span>🗂️ 分类: <a href=\"/categories/"
                      + a.category + ".html\">" + a.category + "</a></span>";
            std::string ts;
            for (size_t j = 0; j < a.tags.size(); ++j) {
                if (j > 0) ts += ", ";
                ts += a.tags[j];
            }
            summary += "<span class=\"card-tags\">" + ts + "</span>";
            summary += "</div></div>";
            if (!a.coverImage.empty()) {
                std::string src = a.coverImage;
                if (src[0] == '.') src = a.category + src.substr(1);
                else if (src[0] != '/' && src.find("://") == std::string::npos)
                    src = a.category + "/" + src;
                summary += "<img class=\"card-cover\" src=\"" + src + "\" alt=\"\">";
            } else
                summary += "<div class=\"card-cover card-cover-empty\"></div>";
            summary += "</div>\n";
        }

        if (total > 2)
            summary += "<div class=\"view-all\"><a href=\"/categories/" + cat + ".html\">View all →</a></div>\n";
        summary += "</div>\n";

        std::string detail = categoryTpl;
        std::string postsHtml;
        for (auto& a : vec)
        {
            postsHtml += "<div class=\"index-card\">";
            postsHtml += "<div class=\"card-date\">" + a.date + "</div>";
            postsHtml += "<div class=\"card-body\"><h3>" + articleLink(a) + "</h3>";
            if (!a.excerpt.empty())
                postsHtml += "<p class=\"card-excerpt\">" + a.excerpt + "</p>";
            postsHtml += "<div class=\"card-footer\">"
                        "<span>🗂️ 分类: <a href=\"/categories/"
                      + a.category + ".html\">" + a.category + "</a></span>";
            std::string ts;
            for (size_t j = 0; j < a.tags.size(); ++j) {
                if (j > 0) ts += ", ";
                ts += a.tags[j];
            }
            postsHtml += "<span class=\"card-tags\">" + ts + "</span>";
            postsHtml += "</div></div>";
            if (!a.coverImage.empty()) {
                std::string src = a.coverImage;
                if (src[0] == '.') src = a.category + src.substr(1);
                else if (src[0] != '/' && src.find("://") == std::string::npos)
                    src = a.category + "/" + src;
                postsHtml += "<img class=\"card-cover\" src=\"" + src + "\" alt=\"\">";
            } else
                postsHtml += "<div class=\"card-cover card-cover-empty\"></div>";
            postsHtml += "</div>\n";
        }

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

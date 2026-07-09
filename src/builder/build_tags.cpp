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

void buildTags(const std::vector<Article>& articles, const SiteConfig& config)
{
    std::string tagsTpl = loadTemplate(config.themeDir + "/tags.html");
    std::string tagTpl  = loadTemplate(config.themeDir + "/tag.html");

    // 按标签分组
    std::map<std::string, std::vector<Article>> tagMap;
    for (auto& a : articles)
        for (auto& t : a.tags)
            tagMap[t].push_back(a);

    for (auto& [tag, vec] : tagMap)
        std::sort(vec.begin(), vec.end(),
            [](const Article& a, const Article& b) { return a.date > b.date; });

    // 摘要页 + 详情页
    std::string summary;
    for (auto& [tag, vec] : tagMap)
    {
        int total = static_cast<int>(vec.size());

        summary += "<div class=\"tag-group\">\n"
                   "<h2><a href=\"/tags/" + tag + ".html\">" + tag + "</a> <span class=\"tag-count\">("
                   + std::to_string(total) + ")</span></h2>\n";

        // 卡片形式展示最新 2 篇
        int shown = std::min(total, 2);
        for (int i = 0; i < shown; ++i)
        {
            auto& a = vec[i];
            summary += "<div class=\"index-card\">";

            summary += "<div class=\"card-date\">" + a.date + "</div>";

            summary += "<div class=\"card-body\">"
                    "<h3>" + articleLink(a) + "</h3>";

            if (!a.excerpt.empty())
                summary += "<p class=\"card-excerpt\">" + a.excerpt + "</p>";

            summary += "<div class=\"card-footer\">"
                    "<span>🗂️ 分类: <a href=\"/categories/"
                  + a.category + ".html\">" + a.category + "</a></span>";

            std::string tagStr;
            for (size_t j = 0; j < a.tags.size(); ++j)
            {
                if (j > 0) tagStr += ", ";
                tagStr += a.tags[j];
            }
            summary += "<span class=\"card-tags\">" + tagStr + "</span>";
            summary += "</div></div>";

            if (!a.coverImage.empty())
            {
                std::string src = a.coverImage;
                if (src[0] == '.')
                    src = a.category + src.substr(1);
                else if (src[0] != '/' && src.find("://") == std::string::npos)
                    src = a.category + "/" + src;
                summary += "<img class=\"card-cover\" src=\"" + src + "\" alt=\"\">";
            }
            else
                summary += "<div class=\"card-cover card-cover-empty\"></div>";

            summary += "</div>\n";
        }

        if (total > 2)
            summary += "<div class=\"view-all\">"
                       "<a href=\"/tags/" + tag + ".html\">View all →</a></div>\n";
        summary += "</div>\n";

        // 详情页：该标签下全部文章（卡片形式）
        std::string detail = tagTpl;
        std::string postsHtml;
        for (auto& a : vec)
        {
            postsHtml += "<div class=\"index-card\">";
            postsHtml += "<div class=\"card-date\">" + a.date + "</div>";
            postsHtml += "<div class=\"card-body\">"
                        "<h3>" + articleLink(a) + "</h3>";
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

} // namespace minissg

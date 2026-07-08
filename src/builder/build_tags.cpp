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

        // 摘要：每个标签只展示最新 2 篇
        summary += "<div class=\"tag-group\">\n"
                   "<h2>" + tag + " <span class=\"tag-count\">("
                   + std::to_string(total) + ")</span></h2>\n<ul>\n";

        int shown = std::min(total, 2);
        for (int i = 0; i < shown; ++i)
            summary += "  <li>" + articleLink(vec[i])
                     + "<span class=\"post-date\">" + vec[i].date + "</span></li>\n";

        if (total > 2)
            summary += "  <li class=\"view-all\">"
                       "<a href=\"/tags/" + tag + ".html\">View all →</a></li>\n";
        summary += "</ul>\n</div>\n";

        // 详情页：该标签下全部文章
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

} // namespace minissg

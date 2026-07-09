#include "builder/builder.h"
#include "builder/build_utils.h"

#include <fstream>
#include <iostream>
#include "renderer/renderer.h"

namespace minissg
{

void buildIndex(const std::vector<Article>& articles, const SiteConfig& config)
{
    std::string tpl = loadTemplate(config.themeDir + "/index.html");

    std::string list;
    for (auto& a : articles)
    {
        list += "<div class=\"index-card\">";

        list += "<div class=\"card-date\">" + a.date + "</div>";

        list += "<div class=\"card-body\">"
                "<h3>" + articleLink(a) + "</h3>";

        if (!a.excerpt.empty())
            list += "<p class=\"card-excerpt\">" + a.excerpt + "</p>";

        list += "<div class=\"card-footer\">"
                "<span>🗂️ 分类: <a href=\"/categories/"
              + a.category + ".html\">" + a.category + "</a></span>";

        std::string tagStr;
        for (size_t j = 0; j < a.tags.size(); ++j)
        {
            if (j > 0) tagStr += ", ";
            tagStr += a.tags[j];
        }
        list += "<span class=\"card-tags\">" + tagStr + "</span>";

        list += "</div>";

        list += "</div>";

        if (!a.coverImage.empty())
        {
            std::string src = a.coverImage;
            if (src[0] == '.')
                src = a.category + src.substr(1);
            else if (src[0] != '/' && src.find("://") == std::string::npos)
                src = a.category + "/" + src;
            list += "<img class=\"card-cover\" src=\"" + src + "\" alt=\"\">";
        }
        else
            list += "<div class=\"card-cover card-cover-empty\"></div>";

        list += "</div>\n";
    }

    replace(tpl, "{{title}}",       config.title);
    replace(tpl, "{{description}}", config.description);
    replace(tpl, "{{posts}}",       list);

    std::ofstream out(config.outputDir + "/index.html");
    out << tpl;
    std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
}

} // namespace minissg

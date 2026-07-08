#include "builder/builder.h"
#include "build_utils.h"

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
        list += "<div class=\"post-item\">"
                "<span class=\"post-date\">" + a.date + "</span>"
                + articleLink(a) +
                "</div>\n";
    }

    replace(tpl, "{{title}}",       config.title);
    replace(tpl, "{{description}}", config.description);
    replace(tpl, "{{posts}}",       list);

    std::ofstream out(config.outputDir + "/index.html");
    out << tpl;
    std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
}

} // namespace minissg

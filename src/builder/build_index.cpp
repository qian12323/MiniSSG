#include "builder/builder.h"
#include "builder/build_utils.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>

#include "renderer/renderer.h"

namespace minissg
{

void buildIndex(const std::vector<Article>& articles, const SiteConfig& config)
{
    std::string tpl = loadTemplate(config.themeDir + "/index.html");

    // 文章卡片列表
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

    // 侧边栏：分类 + 标签
    std::map<std::string, int> cats, tagMap;
    for (auto& a : articles)
    {
        cats[a.category]++;
        for (auto& t : a.tags) tagMap[t]++;
    }

    std::string sidebar;
    sidebar += "<div class=\"side-card\"><h3>分类</h3><ul>";
    for (auto& [c, n] : cats)
        sidebar += "<li><a href=\"/categories/" + c + ".html\">" + c + "</a> <span>(" + std::to_string(n) + ")</span></li>";
    sidebar += "</ul></div>";

    sidebar += "<div class=\"side-card\"><h3>标签</h3><ul>";
    for (auto& [t, n] : tagMap)
        sidebar += "<li><a href=\"/tags/" + t + ".html\">" + t + "</a> <span>(" + std::to_string(n) + ")</span></li>";
    sidebar += "</ul></div>";

    replace(tpl, "{{title}}",       config.title);
    replace(tpl, "{{description}}", config.description);
    replace(tpl, "{{posts}}",       list);
    replace(tpl, "{{sidebar}}",     sidebar);

    // 日期数据供 heatmap 使用
    std::map<std::string, int> dateCounts;
    for (auto& a : articles) dateCounts[a.date]++;

    std::string dates = "[";
    for (size_t i = 0; i < articles.size(); ++i)
    {
        if (i > 0) dates += ",";
        dates += "\"" + articles[i].date + "\"";
    }
    dates += "]";
    replace(tpl, "{{postDates}}", dates);

    std::string cntJson = "{";
    bool first = true;
    for (auto& [d, n] : dateCounts)
    {
        if (!first) cntJson += ",";
        first = false;
        cntJson += "\"" + d + "\":" + std::to_string(n);
    }
    cntJson += "}";
    replace(tpl, "{{postCounts}}", cntJson);

    // 过滤图谱数据：标签关系 + 分类词云
    std::string tagGraph = "{\"nodes\":[";
    bool firstTag = true;
    for (auto& [tag, n] : tagMap)
    {
        if (!firstTag) tagGraph += ","; firstTag = false;
        tagGraph += "{\"id\":\"" + tag + "\",\"count\":" + std::to_string(n) + "}";
    }
    tagGraph += "],\"edges\":[";
    firstTag = true;
    for (auto& a : articles)
        for (size_t i = 0; i < a.tags.size(); ++i)
            for (size_t j = i+1; j < a.tags.size(); ++j) {
                if (!firstTag) tagGraph += ","; firstTag = false;
                tagGraph += "{\"source\":\"" + a.tags[i] + "\",\"target\":\"" + a.tags[j] + "\"}";
            }
    tagGraph += "]}";
    replace(tpl, "{{filterTagGraph}}", tagGraph);

    std::string catCloud = "[";
    bool firstCat = true;
    for (auto& [cat, n] : cats) {
        if (!firstCat) catCloud += ","; firstCat = false;
        catCloud += "{\"id\":\"" + cat + "\",\"count\":" + std::to_string(n) + "}";
    }
    catCloud += "]";
    replace(tpl, "{{filterCatCloud}}", catCloud);

    std::ofstream out(config.outputDir + "/index.html");
    out << tpl;
    std::cout << "Generated: " << config.outputDir << "/index.html" << std::endl;
}

} // namespace minissg

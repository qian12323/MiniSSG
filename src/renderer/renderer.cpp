#include "renderer/renderer.h"

#include <fstream>
#include <sstream>
#include <regex>

namespace minissg
{

namespace
{

std::string replace(std::string str, const std::string& key, const std::string& val)
{
    size_t pos = 0;
    while ((pos = str.find(key, pos)) != std::string::npos)
    {
        str.replace(pos, key.size(), val);
        pos += val.size();
    }
    return str;
}

std::string joinTags(const std::vector<std::string>& tags)
{
    std::string result;
    for (size_t i = 0; i < tags.size(); ++i)
    {
        if (i > 0) result += ", ";
        result += tags[i];
    }
    return result;
}

} // anonymous namespace

std::string loadTemplate(const std::string& path)
{
    std::ifstream in(path);
    if (!in) return {};
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

std::string renderPost(const Article& article, const std::string& tpl)
{
    std::string html = tpl;
    html = replace(html, "{{title}}",   article.title);
    html = replace(html, "{{date}}",    article.date);
    html = replace(html, "{{content}}", article.htmlContent);
    html = replace(html, "{{tags}}",    joinTags(article.tags));
    html = replace(html, "{{slug}}",    article.slug);
    return html;
}

std::string renderIndex(const std::vector<Article>& articles, const std::string& tpl)
{
    std::string list;
    for (auto& a : articles)
        list += "<li><a href=\"" + a.slug + ".html\">" + a.title + "</a> — " + a.date + "</li>\n";

    std::string html = tpl;
    html = replace(html, "{{posts}}", list);
    return html;
}

} // namespace minissg

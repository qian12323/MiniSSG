#include "builder/build_utils.h"

namespace minissg
{

std::string articleLink(const Article& a)
{
    std::string href = a.category + "/" + a.slug + ".html";
    return "<a href=\"/" + href + "\">" + a.title + "</a>";
}

void replace(std::string& tpl, const std::string& key, const std::string& val)
{
    size_t pos = 0;
    while ((pos = tpl.find(key, pos)) != std::string::npos)
    {
        tpl.replace(pos, key.size(), val);
        pos += val.size();
    }
}

} // namespace minissg

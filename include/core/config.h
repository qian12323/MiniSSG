#pragma once

#include <string>

namespace minissg
{

struct SiteConfig
{
    std::string title;                  //站点名
    std::string description;
    std::string baseUrl;
    std::string sourceDir;
    std::string outputDir;
};

} // namespace minissg

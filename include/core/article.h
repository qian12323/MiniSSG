#pragma once

#include <string>
#include <vector>

namespace minissg
{

struct Article
{
    std::string title;                  // 文章标题
    std::string date;                   // 发布日期
    std::string slug;                   // URL 友好名
    std::vector<std::string> tags;      // 标签

    std::string rawContent;             // 原始markdown
    std::string htmlContent;            // 解析后的HTML
};

} // namespace minissg

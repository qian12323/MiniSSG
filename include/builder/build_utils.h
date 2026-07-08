#pragma once

#include <string>
#include <vector>
#include "core/article.h"
#include "core/config.h"

namespace minissg
{

// 生成文章链接 HTML：<a href="/category/slug.html">标题</a>
std::string articleLink(const Article& a);

// 模板占位符替换，如 {{title}} → "My Blog"
void replace(std::string& tpl, const std::string& key, const std::string& val);

// 各页面生成（articles 已按日期倒序排好）
void buildIndex(const std::vector<Article>& articles, const SiteConfig& config);
void buildTags(const std::vector<Article>& articles, const SiteConfig& config);
void buildCategories(const std::vector<Article>& articles, const SiteConfig& config);

} // namespace minissg

#pragma once

#include <string>
#include <vector>
#include "core/article.h"
#include "core/config.h"

namespace minissg
{

std::string articleLink(const Article& a);

void replace(std::string& tpl, const std::string& key, const std::string& val);

void buildIndex(const std::vector<Article>& articles, const SiteConfig& config);

void buildTags(const std::vector<Article>& articles, const SiteConfig& config);

void buildCategories(const std::vector<Article>& articles, const SiteConfig& config);

} // namespace minissg

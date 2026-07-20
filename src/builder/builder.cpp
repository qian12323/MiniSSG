#include "builder/builder.h"
#include "builder/build_utils.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <map>
#include <regex>

#include "parser/parser.h"
#include "parser/heading_corrector.h"
#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

namespace
{

// 重写文章间链接：.md → 相对于当前文章输出的正确路径
std::string fixIntraLinks(const std::string& html,
                          const std::map<std::string, Article>& slugMap,
                          const std::string& curCategory)
{
    std::regex hrefRe(R"(href=\"([^\"]+)\")");
    std::string result;
    size_t lastPos = 0;

    auto it = std::sregex_iterator(html.begin(), html.end(), hrefRe);
    auto end = std::sregex_iterator();

    for (; it != end; ++it)
    {
        result += html.substr(lastPos, it->position() - lastPos);

        std::string href = (*it)[1].str();

        // 只处理 .md 链接
        if (href.size() >= 3 && href.substr(href.size() - 3) == ".md")
        {
            auto lastSlash = href.rfind('/');
            std::string filename = (lastSlash == std::string::npos) ? href : href.substr(lastSlash + 1);
            std::string name = filename.substr(0, filename.size() - 3);

            if (name.size() > 11 && name[4] == '-' && name[7] == '-' && name[10] == '-')
                name = name.substr(11);

            auto mapIt = slugMap.find(name);
            if (mapIt != slugMap.end())
            {
                auto& tgt = mapIt->second;
                int curDepth = 1;
                for (char c : curCategory) if (c == '/') ++curDepth;
                std::string relPath;
                for (int i = 0; i < curDepth; ++i) relPath += "../";
                relPath += tgt.category + "/" + tgt.slug + ".html";

                result += "href=\"" + relPath + "\"";
                lastPos = it->position() + it->length();
                continue;
            }
        }

        result += html.substr(it->position(), it->length());
        lastPos = it->position() + it->length();
    }

    result += html.substr(lastPos);
    return result;
}

} // anonymous namespace

void build(const SiteConfig& config, bool fixHeadings, bool autoNumber)
{
    fs::create_directories(config.outputDir);

    fs::path srcRoot = config.sourceDir;
    std::string postTpl = loadTemplate(config.themeDir + "/post.html");

    // 1. 第一遍：扫描 .md 文件，构建文章列表和 slug 索引
    std::vector<Article> articles;
    std::map<std::string, Article> slugMap;

    for (auto& entry : fs::recursive_directory_iterator(config.sourceDir))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".md")
            continue;

        std::string path = entry.path().string();
        std::cout << "Parsing: " << path << std::endl;

        auto art = parseArticle(path);
        if (fixHeadings)
            art.htmlContent = correctHeadings(art.htmlContent, art.rawContent, path, autoNumber);

        // 从相对路径提取分类目录名（支持多级，过滤 fig-* 目录）
        std::string rel = fs::relative(entry.path(), srcRoot).string();
        {
            std::string filtered;
            size_t start = 0, end;
            while ((end = rel.find('/', start)) != std::string::npos)
            {
                std::string seg = rel.substr(start, end - start);
                if (seg.size() < 4 || seg.substr(0, 4) != "fig-")
                    filtered += seg + "/";
                start = end + 1;
            }
            rel = filtered + rel.substr(start);
        }
        auto lastSep = rel.rfind('/');
        if (lastSep != std::string::npos && lastSep > 0)
            art.category = rel.substr(0, lastSep);
        else
            art.category = "other";

        art.htmlContent = fixIntraLinks(art.htmlContent, slugMap, art.category);
        articles.push_back(art);
        slugMap[art.slug] = art;
    }

    // 再次遍历，修复先前因 slugMap 不全而遗漏的链接
    for (auto& art : articles)
        art.htmlContent = fixIntraLinks(art.htmlContent, slugMap, art.category);

    // 按日期倒序
    std::sort(articles.begin(), articles.end(),
        [](const Article& a, const Article& b) { return a.date > b.date; });

    // 2. 写出文章页
    for (auto& art : articles)
    {
        std::string outPath = config.outputDir + "/"
                            + art.category + "/" + art.slug + ".html";
        fs::create_directories(fs::path(outPath).parent_path());

        auto html = renderPost(art, postTpl);
        replace(html, "{{siteTitle}}", config.title);
        replace(html, "{{autoNumber}}", autoNumber ? "true" : "false");
        replace(html, "{{category}}", art.category);

        std::ofstream out(outPath);
        out << html;
        std::cout << "  -> " << outPath << std::endl;
    }

    // 3. 生成聚合页
    buildIndex(articles, config);
    buildTags(articles, config);
    buildCategories(articles, config);

    // 4. 复制静态资源（非 .md 文件），按所属文章分类前缀存放
    for (auto& entry : fs::recursive_directory_iterator(config.sourceDir))
    {
        if (!entry.is_regular_file() || entry.path().extension() == ".md")
            continue;

        std::string rel = fs::relative(entry.path(), srcRoot).string();
        std::string outPath = config.outputDir + "/" + rel;

        // 检查是否在 fig-{slug} 目录下，若是则用所属文章的分类前缀
        auto sep = rel.rfind('/');
        if (sep != std::string::npos)
        {
            std::string dir = rel.substr(0, sep);
            auto figPos = dir.rfind("fig-");
            if (figPos != std::string::npos && (figPos == 0 || dir[figPos - 1] == '/'))
            {
                std::string slug = dir.substr(figPos + 4);
                auto artIt = slugMap.find(slug);
                if (artIt != slugMap.end())
                {
                    std::string cat = artIt->second.category;
                    if (rel.size() < cat.size() + 1 || rel.substr(0, cat.size() + 1) != cat + "/")
                        outPath = config.outputDir + "/" + cat + "/" + rel;
                }
            }
        }

        fs::create_directories(fs::path(outPath).parent_path());
        fs::copy_file(entry.path(), outPath, fs::copy_options::overwrite_existing);
    }

    // 5. 复制主题静态资源
    fs::path themeDir = config.themeDir;
    if (fs::exists(themeDir))
    {
        for (auto& entry : fs::recursive_directory_iterator(themeDir))
        {
            if (!entry.is_regular_file() || entry.path().extension() == ".html")
                continue;
            std::string rel = fs::relative(entry.path(), themeDir).string();
            std::string outPath = config.outputDir + "/" + rel;
            fs::create_directories(fs::path(outPath).parent_path());
            fs::copy_file(entry.path(), outPath,
                          fs::copy_options::overwrite_existing);
        }
    }
}

} // namespace minissg

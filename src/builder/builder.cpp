#include "builder/builder.h"
#include "builder/build_utils.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "parser/parser.h"
#include "parser/heading_corrector.h"
#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

void build(const SiteConfig& config, bool fixHeadings, bool autoNumber)
{
    fs::create_directories(config.outputDir);

    std::string postTpl = loadTemplate(config.themeDir + "/post.html");

    // 1. 扫描、解析、写出文章页
    std::vector<Article> articles;
    fs::path srcRoot = config.sourceDir;

    for (auto& entry : fs::recursive_directory_iterator(config.sourceDir))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != ".md")
        {
            // 非 md 文件原样复制（图片等静态资源）
            std::string rel = fs::relative(entry.path(), srcRoot).string();
            std::string outPath = config.outputDir + "/" + rel;
            fs::create_directories(fs::path(outPath).parent_path());
            fs::copy_file(entry.path(), outPath,
                          fs::copy_options::overwrite_existing);
            continue;
        }

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

        // 输出路径保持分类目录结构：output/{category}/{slug}.html
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

        articles.push_back(art);
    }

    // 按日期倒序，供聚合页使用
    std::sort(articles.begin(), articles.end(),
        [](const Article& a, const Article& b) { return a.date > b.date; });

    // 2. 生成聚合页
    buildIndex(articles, config);
    buildTags(articles, config);
    buildCategories(articles, config);

    // 3. 复制主题静态资源（非 .html 文件）
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

#include "builder/builder.h"
#include "builder/build_utils.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "parser/parser.h"
#include "renderer/renderer.h"

namespace fs = std::filesystem;

namespace minissg
{

void build(const SiteConfig& config)
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

        // 从相对路径提取分类目录名
        std::string rel = fs::relative(entry.path(), srcRoot).string();
        auto sep = rel.find('/');
        if (sep != std::string::npos)
            art.category = rel.substr(0, sep);
        else
            art.category = "other";

        // 输出路径保持分类目录结构：output/{category}/{slug}.html
        std::string outPath = config.outputDir + "/"
                            + art.category + "/" + art.slug + ".html";
        fs::create_directories(fs::path(outPath).parent_path());

        auto html = renderPost(art, postTpl);
        replace(html, "{{siteTitle}}", config.title);

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
}

} // namespace minissg

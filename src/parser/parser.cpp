#include "parser/parser.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include <yaml-cpp/yaml.h>
#include <cmark.h>

namespace minissg
{

namespace
{

std::string readFile(const std::string& path)
{
    std::ifstream in(path);
    if (!in) return {};
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

std::string extractSlug(const std::string& filePath)
{
    auto pos = filePath.find_last_of("/\\");
    std::string name = (pos == std::string::npos) ? filePath : filePath.substr(pos + 1);

    auto dot = name.rfind(".md");
    if (dot != std::string::npos) name = name.substr(0, dot);

    if (name.size() > 11 && name[4] == '-' && name[7] == '-' && name[10] == '-')
        name = name.substr(11);

    return name;
}

struct SplitResult
{
    std::string frontmatter;
    std::string body;
};

SplitResult splitFrontmatter(const std::string& content)
{
    if (content.size() < 4 || content.substr(0, 4) != "---\n")
        return {"", content};

    auto end = content.find("\n---", 4);
    if (end == std::string::npos)
        return {"", content};

    std::string fm = content.substr(4, end - 4);
    std::string body = content.substr(end + 4);
    if (body.size() > 0 && body[0] == '\n') body = body.substr(1);

    return {fm, body};
}

Article parseFrontmatter(const std::string& yaml)
{
    Article art;
    if (yaml.empty()) return art;

    YAML::Node root = YAML::Load(yaml);

    if (root["title"])   art.title = root["title"].as<std::string>();
    if (root["date"])    art.date  = root["date"].as<std::string>();

    if (root["tags"] && root["tags"].IsSequence())
        for (auto&& t : root["tags"])
            art.tags.push_back(t.as<std::string>());

    return art;
}

std::string renderMarkdown(const std::string& md)
{
    if (md.empty()) return {};

    auto doc = cmark_parse_document(md.c_str(), md.size(), CMARK_OPT_DEFAULT);
    if (!doc) return {};

    char* html = cmark_render_html(doc, CMARK_OPT_UNSAFE);
    std::string result(html ? html : "");

    if (html) free(html);
    cmark_node_free(doc);

    return result;
}

} // anonymous namespace

Article parseArticle(const std::string& filePath)
{
    Article art;
    art.slug = extractSlug(filePath);

    std::string content = readFile(filePath);
    if (content.empty()) return art;

    auto [fm, body] = splitFrontmatter(content);

    art = parseFrontmatter(fm);
    art.slug       = extractSlug(filePath);
    art.rawContent = body;
    art.htmlContent = renderMarkdown(body);

    return art;
}

} // namespace minissg

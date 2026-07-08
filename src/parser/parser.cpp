#include "parser/parser.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include <yaml-cpp/yaml.h>
#include <md4c-html.h>

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

static void mdCallback(const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    static_cast<std::string*>(userdata)->append(text, size);
}

std::string renderMarkdown(const std::string& md)
{
    if (md.empty()) return {};

    std::string html;
    unsigned flags = MD_FLAG_TABLES | MD_FLAG_STRIKETHROUGH | MD_FLAG_TASKLISTS;
    md_html(md.c_str(), static_cast<MD_SIZE>(md.size()), mdCallback, &html, flags, 0);

    return html;
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

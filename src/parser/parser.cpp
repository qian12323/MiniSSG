#include "parser/parser.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include <yaml-cpp/yaml.h>
#include <md4c-html.h>
#include <regex>
#include <iomanip>

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

// URL 解码 %XX
static std::string urlDecode(const std::string& s)
{
    std::string result;
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '%' && i + 2 < s.size() && std::isxdigit(s[i+1]) && std::isxdigit(s[i+2]))
        {
            int v;
            std::istringstream(s.substr(i+1, 2)) >> std::hex >> v;
            result += static_cast<char>(v);
            i += 2;
        }
        else
        {
            result += s[i];
        }
    }
    return result;
}

// 修正 HTML 中图片路径的 URL 编码
static std::string fixImageSrc(const std::string& html)
{
    std::regex imgSrc(R"(src=\"([^\"]+)\")");
    std::string result;
    size_t last = 0;
    auto it = std::sregex_iterator(html.begin(), html.end(), imgSrc);
    auto end = std::sregex_iterator();
    size_t offset = 0;
    std::string r = html;
    for (; it != end; ++it)
    {
        std::string decoded = urlDecode((*it)[1].str());
        if (decoded == (*it)[1].str()) continue;
        size_t pos = it->position() + offset + 5; // after src="
        size_t len = (*it)[1].length();
        r.replace(pos, len, decoded);
        offset += decoded.size() - len;
    }
    return r;
}

std::string renderMarkdown(const std::string& md)
{
    if (md.empty()) return {};

    std::string html;
    unsigned flags = MD_FLAG_TABLES | MD_FLAG_STRIKETHROUGH | MD_FLAG_TASKLISTS;
    md_html(md.c_str(), static_cast<MD_SIZE>(md.size()), mdCallback, &html, flags, 0);

    return html;
}

// 提取引言：正文开头第一个非标题段落
std::string extractExcerpt(const std::string& raw)
{
    std::istringstream stream(raw);
    std::string line;
    std::string excerpt;
    while (std::getline(stream, line))
    {
        if (line.empty()) continue;
        if (line[0] == '#')
        {
            if (!excerpt.empty()) break;
            continue;
        }
        if (line[0] == '|') continue;
        if (!excerpt.empty()) excerpt += " ";
        // 只取纯文本，跳过图片和链接中的 URL
        if (line[0] == '!' && line.size() > 1 && line[1] == '[') continue;
        excerpt += line;
        if (excerpt.size() > 1200) break;
    }

    auto strip = [](std::string& s, const std::string& pat) {
        size_t p = 0;
        while ((p = s.find(pat, p)) != std::string::npos) s.erase(p, pat.size());
    };
    strip(excerpt, "**"); strip(excerpt, "__"); strip(excerpt, "~~");
    strip(excerpt, "`"); strip(excerpt, "*");

    if (excerpt.size() > 1180)
    {
        excerpt = excerpt.substr(0, 877);
        auto pos = excerpt.rfind(' ');
        if (pos != std::string::npos) excerpt = excerpt.substr(0, pos);
        excerpt += "...";
    }
    return excerpt;
}

// 提取封面：正文第一张图片
std::string extractCoverImage(const std::string& html)
{
    std::regex img("<img[^>]*src=\"([^\")]+)\"[^>]*>");
    std::smatch m;
    if (std::regex_search(html, m, img))
        return urlDecode(m[1].str());
    return {};
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
    art.htmlContent = fixImageSrc(renderMarkdown(body));
    art.excerpt = extractExcerpt(body);
    art.coverImage = extractCoverImage(art.htmlContent);

    return art;
}

} // namespace minissg

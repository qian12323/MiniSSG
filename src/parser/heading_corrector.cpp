#include "parser/heading_corrector.h"

#include <iostream>
#include <regex>
#include <vector>
#include <string>

namespace minissg
{

namespace
{

constexpr auto C_WARN  = "\033[1;33m";
constexpr auto C_RESET = "\033[0m";

struct HeadingInfo
{
    int rawLvl;
    size_t blockStart;   // <hN> 起始位置
    size_t blockEnd;     // </hN> 结束位置
    size_t openLen;      // <hN> 长度
    size_t closeLen;     // </hN> 长度
    int newLvl = 0;
    std::string number;  // 编号前缀，如 "2.1. "
};

void warnSkip(const std::string& path, int from, int to)
{
    std::cerr << C_WARN << "Warning: " << C_RESET << path
              << " - heading skips from H" << from
              << " to H" << to
              << ", missing H" << (from + 1) << "\n";
}

void warnRestart(const std::string& path, int newLvl, int prevStart)
{
    std::cerr << C_WARN << "Warning: " << C_RESET << path
              << " - hierarchy restarts from H" << newLvl
              << " after previous section started at H"
              << prevStart << "\n";
}

} // anonymous namespace

std::string correctHeadings(const std::string& html, const std::string& filePath, bool autoNumber)
{
    std::regex reOpen("<h([1-6])(?:\\s[^>]*)?>");
    std::vector<HeadingInfo> hs;

    auto it = std::sregex_iterator(html.begin(), html.end(), reOpen);
    auto end = std::sregex_iterator();
    for (; it != end; ++it)
    {
        HeadingInfo h;
        h.rawLvl = std::stoi((*it)[1].str());
        h.blockStart = it->position();
        h.openLen = it->length();

        std::string closeTag = "</h" + std::to_string(h.rawLvl) + ">";
        auto cp = html.find(closeTag, h.blockStart + h.openLen);
        if (cp == std::string::npos) continue;

        h.closeLen = closeTag.size();
        h.blockEnd = cp + h.closeLen;

        hs.push_back(h);
    }

    if (hs.empty()) return html;

    hs[0].newLvl = 1;
    int curLvl = 1;
    int prevFragStart = hs[0].rawLvl;

    for (size_t i = 1; i < hs.size(); ++i)
    {
        if (hs[i].rawLvl >= hs[i-1].rawLvl)
        {
            ++curLvl;
            hs[i].newLvl = std::min(6, curLvl);

            if (hs[i].rawLvl - hs[i-1].rawLvl > 1)
                warnSkip(filePath, hs[i-1].rawLvl, hs[i].rawLvl);
        }
        else
        {
            curLvl = 1;
            hs[i].newLvl = 1;

            if (hs[i].rawLvl < prevFragStart)
                warnRestart(filePath, hs[i].rawLvl, prevFragStart);

            prevFragStart = hs[i].rawLvl;
        }
    }

    // 自动编号：层级计数器，高一级重置低级
    if (autoNumber)
    {
        int cnt[6] = {0};
        for (auto& h : hs)
        {
            for (int i = h.newLvl; i < 6; ++i) cnt[i] = 0;
            cnt[h.newLvl - 1]++;

            std::string num;
            for (int i = 0; i < h.newLvl; ++i)
            {
                if (cnt[i] == 0) continue;
                if (!num.empty()) num += ".";
                num += std::to_string(cnt[i]);
            }
            num += ". ";
            h.number = num;
        }
    }

    // 替换标签 + 编号（从后往前）
    std::string result = html;
    for (int i = static_cast<int>(hs.size()) - 1; i >= 0; --i)
    {
        auto& h = hs[i];
        if (h.rawLvl == h.newLvl && h.number.empty()) continue;

        std::string newOpen = "<h" + std::to_string(h.newLvl) + ">";
        std::string newClose = "</h" + std::to_string(h.newLvl) + ">";
        size_t textEnd = h.blockEnd - h.closeLen;
        size_t textStart = h.blockStart + h.openLen;
        std::string text = result.substr(textStart, textEnd - textStart);

        std::string block = newOpen + h.number + text + newClose;
        result.replace(h.blockStart, h.blockEnd - h.blockStart, block);
    }

    return result;
}

} // namespace minissg

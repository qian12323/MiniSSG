# MiniSSG

一个极简的静态站点生成器，用于个人博客。

## 快速开始

```bash
# 新建站点
minissg ns -n myblog && cd myblog

# 写文章
minissg new "我的第一篇文章"

# 构建
minissg b

# 本地预览
minissg r -p 8080
```

## 特点

- **数据与视图解耦** —— Markdown 解析为纯数据对象，HTML 模板用 `{{placeholder}}` 占位，互不依赖
- **Frontmatter 支持** —— YAML 头部定义标题、日期、标签
- **自动标题纠错** —— 从 `##` 开始写也没问题，自动归一到 `h1`；跳级会告警
- **自动编号** —— h1-h3 按层级编号，h4+ 不编
- **暗色模式** —— 跟系统自动切换，也可手动
- **分类** —— 按子目录自动归类，`other` 兜底
- **标签** —— 聚合页 + 关系图谱 + 多重过滤
- **热力图** —— 12 个月发文日历，点击按日期筛选
- **侧边栏过滤** —— 按标题搜索、标签图谱、分类词云，多选取交集
- **代码高亮** —— highlight.js，190+ 语言，亮/暗主题跟随
- **公式渲染** —— MathJax，`$...$` 行内 + `$$...$$` 块级
- **图片预览** —— 点击放大、缩放、旋转、全屏
- **目录悬浮** —— 右侧粘性目录，滚动高亮当前位置
- **引言 + 封面** —— 首页卡片展示每篇文章的引言和封面图
- **单二进制** —— 编译期嵌入两套主题，`newsite` 一键生成完整站点

## 命令

| 命令 | 缩写 | 说明 |
|------|------|------|
| `build` | `b` | 构建站点 |
| `new "标题"` | — | 新建文章，`--categ` 指定分类 |
| `run` | `r` | 启动本地服务器，`-p` 指定端口 |
| `clean` | `c` | 清理孤儿文件，`-n` 试运行 |
| `newsite` | `ns` | 新建站点目录，`-n` 指定名称 |

## 配置

```yaml
# config.yaml
title: My Blog
description: A personal blog
sourceDir: posts
outputDir: output
themeDir: themes/minimal
fixHeadings: true
autoNumber: true
```

## 构建

```bash
cmake -B build -S .
cmake --build build -j4
```

依赖通过 CMake FetchContent 自动拉取，无需手动安装。

## 文档

- [架构设计](docs/ARCHITECTURE.md)
- [使用指南](docs/USER_GUIDE.md)

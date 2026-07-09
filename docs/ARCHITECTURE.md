# MiniSSG — Architecture

A minimal static site generator for personal blogs. C++17, ~20 source files, all dependencies fetched via CMake `FetchContent`.

## Overview

**Pipeline**: Markdown files → parse → data objects → render with templates → static HTML.

**Design principle**: data and presentation are fully decoupled. Parsing produces plain `Article` structs. Templates are plain HTML with `{{placeholder}}` placeholders. Rendering is pure string substitution. No logic in templates, no HTML in C++.

## Core Concepts

### Article Data Model

```cpp
struct Article {
    string title, date, slug, category;
    vector<string> tags;
    string rawContent;    // original markdown
    string htmlContent;   // rendered HTML
    string excerpt;       // intro text for index cards
    string coverImage;    // first <img> src
};
```

Parser fills it from frontmatter + markdown. Builder reads it to generate pages. Renderer uses it for template substitution. None of these layers know about each other.

### Template System

Zero logic. Builder does `replace(tpl, "{{title}}", art.title)` for each placeholder. That's it. Everything else — TOC, dark mode, image zoom, code highlight — is client-side JS in the theme.

### Heading Correction

Scans the rendered HTML, groups headings into increasing fragments, normalizes each fragment to start from `<h1>`. Warns about level skips and hierarchy regressions. Auto-numbering (h1-h3) is a post-process step.

### Theme Architecture

```
themes/{name}/
├── post.html, index.html, tags.html, ...
└── assets/
    ├── style.css      # CSS variables for dark/light
    ├── theme.js       # dark mode toggle
    ├── toc.js         # TOC generation + scroll spy
    └── ...
```

Builder copies all non-`.html` files into `output/assets/`. Templates reference them with absolute paths (`/assets/style.css`). Switch themes by changing `themeDir` in config.

### Static Asset Pipeline

Images and other files placed alongside `.md` articles are copied to the output directory preserving directory structure. Relative paths in Markdown work transparently.

## Dependencies

| Library | Role |
|---------|------|
| **md4c** | Markdown → HTML (GFM: tables, strikethrough) |
| **yaml-cpp** | Frontmatter + config parsing |
| **CLI11** | CLI framework (subcommands, help, validation) |
| **highlight.js** | Code syntax highlighting (CDN, 190+ langs) |
| **MathJax** | LaTeX rendering (CDN) |
| **vis-network** | Tag relationship graph (CDN) |
| **Viewer.js** | Image preview/zoom (CDN) |

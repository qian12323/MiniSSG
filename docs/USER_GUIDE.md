# MiniSSG — User Guide

A minimal static site generator for personal blogs.

## Quick Start

```bash
# Create a new site
minissg ns -n myblog
cd myblog

# Write posts in posts/ directory
minissg new "My First Post" --categ other

# Build the site
minissg b

# Preview locally
minissg r -p 8080
```

Open `http://localhost:8080` in browser.

## Directory Structure

```
myblog/
├── config.yaml           # Site config
├── posts/                # Markdown articles
│   ├── other/            # Uncategorized (default)
│   │   └── hello.md
│   ├── cpp/              # Category: cpp
│   └── python/           # Category: python
├── themes/               # Theme templates
│   ├── default/          # Reference theme
│   └── minimal/          # Production theme
└── output/               # Generated static site
```

## Article Format

Each article is a Markdown file with YAML frontmatter:

```markdown
---
title: "My First Post"
date: 2026-07-14
tags: [c++, tutorial]
---

Content starts here. This paragraph before any heading
is treated as the **introduction** (shown on index page).

## Section One

Regular Markdown content.

### Sub-section

More content.

![Screenshot](fig_my-first-post/screenshot.png)
```

### Frontmatter Fields

| Field | Required | Description |
|-------|----------|-------------|
| `title` | Yes | Article title |
| `date` | Yes | Publication date (YYYY-MM-DD) |
| `tags` | No | List of tags for categorization |

### Images

Place images in a subdirectory named `fig_{slug}` next to your article:

```
posts/cpp/
├── cpp-tips.md
└── fig_cpp-tips/
    ├── screenshot.png
    └── diagram.jpg
```

Reference in Markdown: `![alt](fig_cpp-tips/screenshot.png)`

### Headings

- Start from `##` or `#` — auto-correction normalizes the hierarchy
- Consecutive same-level headings (e.g., `##` followed by another `##`) each start a new section
- Jumping levels (e.g., `##` → `####`) triggers a warning but is auto-corrected

## Configuration

`config.yaml`:

```yaml
title: My Blog
description: A personal blog powered by MiniSSG
baseUrl: https://example.com
sourceDir: posts
outputDir: output
themeDir: themes/minimal

fixHeadings: true    # Auto-correct heading hierarchy
autoNumber: true     # Auto-number headings (h1-h3 only)
```

## Commands

### `build` (alias: `b`)

Build the static site:

```bash
minissg b
minissg b -c custom-config.yaml
minissg b --fix-headings no
minissg b --fix-headings-number no
```

### `new`

Create a new article:

```bash
minissg new "Post Title"                    # → posts/other/
minissg new "Post Title" --categ cpp        # → posts/cpp/
```

If the category doesn't exist, a list of available categories is shown.

### `run` (alias: `r`)

Start a local development server:

```bash
minissg r -p 8080
```

Serves `output/` directory. Ctrl+C to stop.

### `clean` (alias: `c`)

Remove stale output files (orphan HTML, images, etc.):

```bash
minissg c          # Remove stale files
minissg c -n       # Dry run: show what would be removed
```

Protected files (index.html, tags.html, aggregated pages, theme assets) are never deleted.

### `newsite` (alias: `ns`)

Create a new site from scratch:

```bash
minissg ns -n myblog
```

Generates complete directory structure with both themes, default config, and a sample article.

## Generated Pages

| File | Description |
|------|-------------|
| `index.html` | Homepage: timeline + heatmap + search + filter sidebar |
| `tags.html` | Tags overview: 2 latest articles per tag + relationship graph |
| `categories.html` | Categories overview: 2 latest articles per category |
| `tags/{tag}.html` | Tag detail: all articles with that tag |
| `categories/{cat}.html` | Category detail: all articles in that category |
| `{category}/{slug}.html` | Article page: content + TOC + images |

## Features

### Auto Heading Correction

Enabled by default (`fixHeadings: true` in config.yaml). Normalizes heading hierarchy:

```
## Section      →  <h1>Section</h1>
### Sub         →  <h2>Sub</h2>
## New Section  →  <h1>New Section</h1>   (new fragment)
```

Warns about skipped levels and hierarchy restarts.

### Auto Numbering

Enabled by default (`autoNumber: true`). Numbers h1-h3:

```
1. Introduction
  1.1. Background
    1.1.1. Details
  1.2. Motivation
2. Methods
```

h4-h6 are never numbered.

### Dark Mode

Auto-detected from system preference. Manual toggle (🌙/☀️) in navigation bar. Theme files use CSS variables for all colors.

### Heatmap

Shows post activity over 12 months (GitHub-style). Click a day to filter articles. Year selector dropdown.

### Filter Sidebar

- **Search**: Type to filter articles by title
- **Tag graph**: mini relationship graph — click nodes to filter by tag (multi-select)
- **Category cloud**: click categories to filter (multi-select)
- **Reset button**: clear all filters

### Tag Relationship Graph

On the Tags page. Force-directed layout showing tag co-occurrence. Click a node to filter articles by tag. Zoom controls: ↺ reset, + zoom in, - zoom out.

### Code Highlighting

highlight.js with auto theme switching. Supports 190+ languages.

### Math Formulas

LaTeX via MathJax: `$E=mc^2$` (inline) and `$$...$$` (block).

### Image Viewer

Click any image to preview with zoom, rotate, and fullscreen (Viewer.js).

## Switching Themes

```yaml
# config.yaml
themeDir: themes/minimal   # or themes/default
```

To create a custom theme, copy `themes/minimal/` and modify. Templates use `{{placeholder}}` syntax. Builder replaces them at build time.

## Creating a Custom Theme

Required template files:

| File | Required placeholders |
|------|---------------------|
| `post.html` | `{{title}} {{date}} {{tags}} {{content}} {{siteTitle}} {{category}} {{autoNumber}}` |
| `index.html` | `{{title}} {{description}} {{posts}} {{sidebar}}` |
| `tags.html` | `{{title}} {{groups}} {{graphData}}` |
| `tag.html` | `{{tag}} {{siteTitle}} {{posts}}` |
| `categories.html` | `{{title}} {{groups}}` |
| `category.html` | `{{category}} {{siteTitle}} {{posts}}` |

Place additional assets (CSS, JS) in `assets/` subdirectory. They are copied to `output/assets/` at build time.

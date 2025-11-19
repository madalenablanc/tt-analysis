# Converting Markdown to Presentation Slides with Pandoc

## Quick Start

### 1. Check if Pandoc is Installed

```bash
pandoc --version
```

If not installed:
- **macOS:** `brew install pandoc`
- **Linux:** `sudo apt-get install pandoc` or `sudo yum install pandoc`
- **Windows:** Download from https://pandoc.org/installing.html

---

## Conversion Options

### Option 1: PDF Beamer Slides (LaTeX-based, Professional)

```bash
cd /Users/utilizador/cernbox/tau_analysis/MuTau_channel

pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --slide-level=2 \
  -V theme:Madrid \
  -V colortheme:dolphin
```

**Themes available:**
- `Madrid`, `Berlin`, `Copenhagen`, `Warsaw`, `Singapore`

**Color themes:**
- `dolphin`, `beaver`, `crane`, `seahorse`, `whale`

---

### Option 2: PowerPoint (PPTX)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -o PRESENTATION_TAU_ID_ISSUE.pptx \
  --slide-level=2
```

This creates a PowerPoint file you can edit in Microsoft Office or LibreOffice.

---

### Option 3: HTML Slides (reveal.js - Interactive)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t revealjs \
  -s \
  -o PRESENTATION_TAU_ID_ISSUE.html \
  --slide-level=2 \
  -V theme:black \
  -V transition:slide
```

**Themes available:**
- `black`, `white`, `league`, `beige`, `sky`, `night`, `serif`, `simple`, `solarized`

**Transitions:**
- `none`, `fade`, `slide`, `convex`, `concave`, `zoom`

Open `PRESENTATION_TAU_ID_ISSUE.html` in any browser and use arrow keys to navigate.

---

### Option 4: HTML Slides (Slidy)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t slidy \
  -s \
  -o PRESENTATION_TAU_ID_ISSUE_slidy.html \
  --slide-level=2
```

Simpler HTML slides, good for quick sharing.

---

### Option 5: HTML Slides (S5)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t s5 \
  -s \
  -o PRESENTATION_TAU_ID_ISSUE_s5.html \
  --slide-level=2
```

---

## Customization Options

### Add Title Slide

Create a metadata block at the top of your Markdown:

```markdown
---
title: "Tau ID Issue Investigation"
subtitle: "MuTau Channel Analysis"
author: "Your Name"
date: "November 18, 2024"
institute: "CERN"
---

# Investigation: Factor of 2 Discrepancy
...
```

Then convert:

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --slide-level=2
```

---

### Control Slide Level

The `--slide-level` option controls which heading level creates new slides:

- `--slide-level=1`: Each `# Heading` starts a new slide
- `--slide-level=2`: Each `## Heading` starts a new slide (recommended for this file)
- `--slide-level=3`: Each `### Heading` starts a new slide

---

### Add Syntax Highlighting Theme

For code blocks:

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --slide-level=2 \
  --highlight-style=tango
```

**Highlight styles:**
- `pygments`, `tango`, `espresso`, `zenburn`, `kate`, `monochrome`, `breezedark`, `haddock`

---

### Include Table of Contents

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --slide-level=2 \
  --toc \
  --toc-depth=1
```

---

## Recommended Commands

### Best for Professional Presentation (PDF)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --slide-level=2 \
  -V theme:Madrid \
  -V colortheme:dolphin \
  --highlight-style=tango \
  -V fontsize:10pt
```

---

### Best for Interactive Web Viewing (HTML)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t revealjs \
  -s \
  -o PRESENTATION_TAU_ID_ISSUE.html \
  --slide-level=2 \
  -V theme:sky \
  -V transition:slide \
  --highlight-style=zenburn
```

Then open in browser and use:
- **Arrow keys** to navigate
- **F** for fullscreen
- **ESC** for overview
- **S** for speaker notes

---

### Best for Editing (PowerPoint)

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -o PRESENTATION_TAU_ID_ISSUE.pptx \
  --slide-level=2 \
  --reference-doc=custom_template.pptx  # Optional: use your template
```

---

## Advanced: Custom Template

### Create Custom Beamer Template

```bash
pandoc -D beamer > custom_beamer_template.tex
```

Edit `custom_beamer_template.tex` to customize, then:

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --template=custom_beamer_template.tex \
  --slide-level=2
```

---

## Troubleshooting

### PDF Generation Fails

**Error:** "pdflatex not found"

**Solution:** Install LaTeX:
- **macOS:** `brew install --cask mactex-no-gui`
- **Linux:** `sudo apt-get install texlive-latex-base texlive-latex-extra`

### Unicode Characters Issue

Add to pandoc command:

```bash
--pdf-engine=xelatex
```

Full command:

```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o PRESENTATION_TAU_ID_ISSUE.pdf \
  --pdf-engine=xelatex \
  --slide-level=2
```

### Tables Don't Fit

Add to metadata or command:

```bash
-V classoption:aspectratio=169  # 16:9 widescreen
```

---

## Quick Reference

| Format | Command | Use Case |
|--------|---------|----------|
| **PDF** | `-t beamer -o file.pdf` | Print, formal presentation |
| **PowerPoint** | `-o file.pptx` | Edit in Office, collaboration |
| **HTML (reveal.js)** | `-t revealjs -s -o file.html` | Interactive web, remote viewing |
| **HTML (Slidy)** | `-t slidy -s -o file.html` | Simple web slides |

---

## One-Liner Examples

### PDF (Beamer)
```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md -t beamer -o slides.pdf --slide-level=2 -V theme:Madrid
```

### PowerPoint
```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md -o slides.pptx --slide-level=2
```

### HTML (reveal.js)
```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md -t revealjs -s -o slides.html --slide-level=2 -V theme:sky
```

### HTML (Slidy)
```bash
pandoc PRESENTATION_TAU_ID_ISSUE.md -t slidy -s -o slides.html --slide-level=2
```

---

## My Recommendation

For your use case (physics analysis presentation), I recommend:

```bash
# For advisor meeting (professional PDF):
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t beamer \
  -o TAU_ID_INVESTIGATION.pdf \
  --slide-level=2 \
  -V theme:Madrid \
  -V colortheme:dolphin \
  -V fontsize:10pt \
  --highlight-style=tango

# For sharing online (interactive HTML):
pandoc PRESENTATION_TAU_ID_ISSUE.md \
  -t revealjs \
  -s \
  -o TAU_ID_INVESTIGATION.html \
  --slide-level=2 \
  -V theme:white \
  -V transition:slide \
  --highlight-style=github
```

The PDF is best for formal presentations and printing. The HTML is great for sharing a link or viewing on different devices.
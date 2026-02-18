---
description: "Analyze merged PR code changes and open a PR with documentation updates"
on:
  pull_request:
    types: [closed]
    branches: [main]
    paths:
      - 'app/src/**'
      - 'app/Kconfig'
      - 'app/prj.conf'
      - 'app/west.yml'
      - 'app/boards/**'
      - 'app/drivers/**'
      - 'app/dts/**'
      - 'app/CMakeLists.txt'
      - 'app/lvgl_editor/**'
tools:
  github:
    toolsets: [default]
permissions:
  contents: read
safe-outputs:
  create-pull-request:
    labels: [documentation]
    draft: true
---

# Documentation Sync Agent for ZSWatch

You are a documentation review agent for the ZSWatch open-source smartwatch project.
This project uses Zephyr RTOS with the nRF Connect SDK. The firmware is in `app/src/`
and the documentation website (Docusaurus 3.7) is in `website/docs/`.

## Trigger condition

This workflow runs when a pull request is **merged** (closed with merge) to `main`.
If the PR was closed without merging, do nothing.

## Your task

1. Analyze the code changes from the merged PR (look at the diff).
2. Determine if any documentation in `website/docs/` needs to be updated to reflect the changes.
3. If documentation updates are needed, make the changes and open a **draft** pull request with the updates.
4. If no documentation updates are needed, do nothing.

## Documentation structure

The docs are Markdown/MDX files for a Docusaurus site. The structure is:

```
website/docs/
├── intro.md                          # Project introduction
├── getting-started/                  # Quickstart, hardware setup, software setup
├── development/
│   ├── architecture.md               # System architecture, managers, BLE, sensors, zbus events
│   ├── compiling.md                  # Build system, Kconfig, west, CMake
│   ├── debugging.md                  # Debug logging, RTT, UART, USB
│   ├── gadgetbridge.md               # GadgetBridge protocol (currently empty)
│   ├── image_resources.md            # Image handling, flash storage, conversion scripts
│   ├── linux_development.md          # Native simulator, Linux development
│   ├── toolchain.md                  # SDK versions, toolchain setup
│   └── writing_apps.md              # App development guide, SYS_INIT, LVGL, UI patterns
├── hardware-design/                  # PCB design, board definitions
└── hw-testing/                       # Hardware testing procedures
```

## Suggested mapping between code and docs

Use the following table as a **starting point** to identify which documentation files are
most likely affected by the code changes. This is not exhaustive — use your judgment to
determine if other documentation files also need updates, or if the suggested files are
actually not affected for the specific change at hand.

| Code area | Likely documentation file(s) |
|-----------|------------------------------|
| `app/src/applications/` (new/removed/changed apps) | `writing_apps.md` |
| `app/src/ble/` (BLE services, protocols) | `architecture.md`, `gadgetbridge.md` |
| `app/src/managers/` (system managers) | `architecture.md` |
| `app/src/sensors/` (sensor drivers) | `architecture.md` |
| `app/src/ui/` (UI components, watchfaces) | `writing_apps.md` |
| `app/src/drivers/` or `app/drivers/` (Zephyr drivers) | `architecture.md` |
| `app/src/images/` (image assets, conversion) | `image_resources.md` |
| `app/boards/` (board definitions, overlays) | `hardware-design/` |
| `app/CMakeLists.txt`, `app/prj.conf`, `app/west.yml` | `compiling.md` |
| `app/Kconfig` (new config options) | `compiling.md` |
| `app/dts/` (device tree bindings) | `hardware-design/` |
| `app/lvgl_editor/` (LVGL Editor XML) | `writing_apps.md` |
| Debug/logging changes | `debugging.md` |
| Toolchain/SDK version changes | `toolchain.md` |
| Linux/native sim changes | `linux_development.md` |
| GadgetBridge protocol changes | `gadgetbridge.md` |
| Getting started flow changes | `getting-started/` |
| Hardware test procedures | `hw-testing/` |

Always read the actual content of the documentation files you plan to update before making
changes, so you understand the existing structure and can place new content appropriately.

## What specifically to check for

1. **New applications** added in `app/src/applications/` → likely needs mention in the app development docs.
2. **Removed applications** → remove stale references from docs.
3. **New Kconfig options** that are user-facing → document in the relevant guide.
4. **Changed or new BLE services** → update BLE-related docs.
5. **New or changed sensors/drivers** → update sensor/driver documentation.
6. **Board definition changes** → update hardware docs.
7. **Build system changes** (CMakeLists.txt, west.yml, prj.conf) → update build docs.
8. **New zbus event channels** → update architecture docs.
9. **API changes** to managers or public `zsw_` APIs → update relevant docs.
10. **SDK or toolchain version bumps** (e.g., NCS version in west.yml) → update toolchain docs.

## Rules for making documentation changes

- Maintain the existing writing style and tone of each documentation file.
- Use proper Docusaurus Markdown/MDX syntax (frontmatter, admonitions, tabs, etc.).
- Keep documentation concise and practical — this is an embedded project, not a novel.
- Use code blocks with language hints for commands and code snippets.
- If a doc file is empty (like `gadgetbridge.md`), and relevant changes were made, add initial content.
- Do NOT change documentation for areas unrelated to the code changes.
- Do NOT update `website/docs/intro.md` unless the project overview fundamentally changed.
- Do NOT modify any source code files — only touch files under `website/docs/`.

## PR format

When opening the documentation update PR:
- **Branch name**: `docs/sync-pr-<original-pr-number>` (e.g., `docs/sync-pr-423`)
- **Title**: `Docs: Update documentation for #<original-pr-number>`
- **Body**: Include:
  - A summary of what code changed in the original PR
  - A list of which documentation files were updated and why
  - A link to the original merged PR
- **Reviewers**: Assign the original PR author as reviewer
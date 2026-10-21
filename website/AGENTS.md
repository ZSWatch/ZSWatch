# zswatch.dev website: agent instructions

Applies to everything under `website/`. The repository root `AGENTS.md` still
applies (Git rules, firmware facts the docs describe).

[zswatch.dev](https://zswatch.dev): docs, blog and browser tools for the watch.
Docusaurus 3.7, React 19, Tailwind CSS 3, Mermaid diagrams. npm
(`package-lock.json`), Node ≥ 18 (tested with 24).

## Layout

| Path | What it is |
| --- | --- |
| `docs/` | Documentation. Sidebar is generated from folders; `_category_.json` sets name and order, `sidebar_position` orders pages. |
| `docs/getting-started/` | Quickstart, hardware, phone setup, companion app, Watch Tools. |
| `docs/development/` | Architecture, compiling, toolchain, debugging, Linux/native_sim, writing apps, image resources, building the docs. |
| `docs/firmware/` | Firmware updates and recovery. |
| `docs/hardware-design/`, `docs/hw-testing/` | Hardware docs. |
| `blog/` | One folder per post: `YYYY-MM-DD-slug/index.md` with its images. Authors in `authors.yml`, tags in `tags.yml`. |
| `src/pages/` | `index.js` (home), `update.js` (FW Update), `tools.js` (Watch Tools), `privacy.md`. |
| `src/components/FirmwareUpdate/` | Firmware update from the browser over Web Bluetooth (MCUmgr/SMP). |
| `src/components/WatchTools/` | Backgrounds, live logs, BLE traffic and crash reports. Read its `README.md` first. |
| `src/components/ZoomableMermaid/` | Mermaid diagrams with zoom. |
| `src/components/GithubNavbar/`, `HomepageFeatures/`, `SubscriptionForm/` | Navbar stars, home cards, newsletter form (used in blog posts). |
| `src/theme/` | Swizzled `Root` and custom navbar item types. |
| `src/css/` | `custom.css`, `watch-gallery.css`. |
| `static/` | Images (`static/img/` → `/img/...`), videos, watchface backgrounds, `CNAME`. |
| `tests/` | Node tests for Watch Tools (mocked GATT). |

## Commands

```sh
cd website
npm install
npm start                    # dev server with hot reload
npm run build                # production build into build/
npm run serve                # serve that build
npm run test:watch-tools     # Watch Tools unit tests
npm run clear                # clear the Docusaurus cache when builds act stale
```

`onBrokenLinks: 'throw'`: a broken link or anchor fails the build, so run
`npm run build` before finishing any docs change, not just `npm start`.

The GitHub API (stars, firmware artifacts) is rate limited without a token.
Copy `.env.example` to `.env.local` (gitignored), set
`REACT_APP_GITHUB_TOKEN`, then `source .env.local && npm start`. The token
reaches the code as `customFields.githubToken`. Never commit or print it.

Deployment is manual (`npm run deploy` to the `gh-pages` branch). Don't deploy
unless asked.

## Writing docs

- Docs describe the firmware in `app/`. Check claims (board names, Kconfig
  symbols, shell commands, build commands, NCS version) against the code
  before writing them, and update the matching page when a firmware change
  alters behavior described here (`development/architecture.md` covers
  managers, BLE, sensors and zbus).
- After a PR merges to `main`, the agentic workflow
  `.github/workflows/docs-pr-analyze.md` may open a draft PR with doc updates.
- Use Docusaurus admonitions (`:::info`, `:::warning`), fenced code blocks
  with a language, and Mermaid for diagrams.
- Short, direct prose. Keep pages focused; link to other pages instead of
  repeating them.
- Put images next to blog posts, or in `static/img/` for docs. Compress large
  photos.

## Code

- Function components with hooks; CSS modules (`styles.module.css`) per
  component, Tailwind utilities where the surrounding code uses them.
- The site has a dark default and a light mode; check both.
- Watch Tools and FirmwareUpdate talk to a real watch over Web Bluetooth (SMP +
  Nordic UART, Gadgetbridge `GB({...})` JSON). Keep their protocol behavior in
  sync with the firmware, and test transfers on a real watch: the unit tests
  mock GATT.
- Pages must work on a phone-width screen.

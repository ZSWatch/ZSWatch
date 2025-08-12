# Website

This website is built using [Docusaurus](https://docusaurus.io/), a modern static website generator.

### Installation

```
$ yarn
```

### Environment Setup

The GitHub API without a token has limited number of calls, hence for development you may want to provide your own token.

1. Copy the example environment file:
   ```
   $ cp .env.example .env.local
   ```

2. Get a GitHub personal access token:
   - Go to GitHub Settings > Developer settings > Personal access tokens
   - Generate a new token with `public_repo` scope
   - Copy the token

3. Edit `.env.local` and replace `your_github_token_here` with your actual token:
   ```
   REACT_APP_GITHUB_TOKEN=your_actual_token_here
   ```

Note: The `.env.local` file is git-ignored and won't be committed to version control.

### Local Development

```
$ yarn start
```

This command starts a local development server and opens up a browser window. Most changes are reflected live without having to restart the server.

**Important**: If you're using environment variables (like `REACT_APP_GITHUB_TOKEN`), you need to source the `.env.local` file first:

```
$ source .env.local && npm start
```

This ensures Docusaurus can access the environment variables during server startup.

### Build

```
$ yarn build
```

This command generates static content into the `build` directory and can be served using any static contents hosting service.

### Deployment

Using SSH:

```
$ USE_SSH=true yarn deploy
```

Not using SSH:

```
$ GIT_USER=<Your GitHub username> yarn deploy
```

If you are using GitHub pages for hosting, this command is a convenient way to build the website and push to the `gh-pages` branch.

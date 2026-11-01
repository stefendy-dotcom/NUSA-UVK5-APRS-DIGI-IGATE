# NUSA GitHub Automation Bot

The official GitHub App bot identity for future automated operations
in this repository is **`stefendy-nusa[bot]`**.

Authenticated workflow writes use the GitHub App installation token
obtained from GitHub Actions variable `NUSA_APP_CLIENT_ID` and secret
`NUSA_APP_PRIVATE_KEY`. Merely changing `git config user.name` does
not replace GitHub App authentication.

Existing historical commits and firmware releases are preserved.

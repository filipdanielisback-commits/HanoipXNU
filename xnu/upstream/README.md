# Upstream XNU checkout

The upstream source is maintained by Apple at:

- https://github.com/apple-oss-distributions/xnu

Do not place a moving `main` checkout in a reproducible build. The project must pin a tag and immutable commit SHA in `../REVISION.md` before integration work depends on it.

The upstream repository is not copied into this project by this placeholder. A developer should maintain a separate checkout/worktree or use a documented source-fetch step that verifies the pinned commit and its license notices.

No iOS 18-specific source revision has been confirmed by this project yet.

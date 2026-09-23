# Release tag messages

Release tags use signed annotated tags whose names match `v$(cat VERSION)`. Their messages summarize the corresponding [`CHANGELOG.md`](../CHANGELOG.md) entry for someone viewing the tag without duplicating the full release history.

Use this form:

```text
v<VERSION>: <concise user-facing summary>

Added:
- <Important user-visible addition.>

Changed:
- <Important user-visible behavior change.>

Safety and validation:
- <Automated tests and builds that passed.>
- <Hardware acceptance that passed or remains pending.>

Full history: CHANGELOG.md
```

Keep only the change sections that apply. Existing tags commonly use `Added:` and `Changed:`; use the heading from the matching changelog entry when another category better describes the release. Write bullets in user-visible terms and keep the first line useful when displayed by itself. Wrap long prose to match the surrounding documentation.

For firmware behavior changes, state the validation status precisely. Do not describe hardware acceptance as complete while any required checks remain pending. Keep detailed results and historical facts in `CHANGELOG.md` rather than copying the entire entry into the tag message.

Save the completed message as `TAG_MESSAGE.md` in the repository root. This is a temporary, ignored file: review it for unfilled placeholders and accurate validation status, then delete it after tagging or overwrite it for the next release.

From the repository root, create the signed tag from the release commit with:

```sh
git tag -s "v$(cat VERSION)" -F TAG_MESSAGE.md
```

The `-F` option supplies the tag message from the file without opening an editor. To make a final edit before creating the tag, add `-e`:

```sh
git tag -s "v$(cat VERSION)" -F TAG_MESSAGE.md -e
```

Git uses `.git/TAG_EDITMSG` as the temporary editor buffer; it is distinct from the root-level `TAG_MESSAGE.md` release draft. Review the completed message, then push the explicitly named tag after the release checks in [custom-firmware.md](custom-firmware.md) pass.

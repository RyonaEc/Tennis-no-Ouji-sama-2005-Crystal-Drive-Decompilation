"""Upload the local objdiff report to ByteMatch.

The build needs the retail ROM, which cannot live in CI, so progress reaches
bytematch.dev through its upload API instead of a GitHub Actions artifact.
Create a token on the project's Manage page (it is shown once) and export it:

    export BYTEMATCH_TOKEN=...
    python tools/upload_report.py

Reads build/<VERSION>/report.json by default and tags the upload with the
current commit so ByteMatch can link source lines.
"""
import argparse, gzip, json, os, pathlib, subprocess, sys, urllib.error, urllib.request

OWNER = 'RyonaEc'
REPO = 'Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation'
API = 'https://bytematch.dev/api/v1/projects/{owner}/{repo}/reports'


def git_commit():
    try:
        return subprocess.run(['git', 'rev-parse', 'HEAD'], capture_output=True,
                              text=True, check=True).stdout.strip()
    except Exception:
        return None


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('report', nargs='?', default='build/ATOJ/report.json')
    ap.add_argument('--version', default='ATOJ')
    ap.add_argument('--commit', default=None, help='commit the report was built from')
    ap.add_argument('--owner', default=OWNER)
    ap.add_argument('--repo', default=REPO)
    ap.add_argument('--dry-run', action='store_true')
    args = ap.parse_args()

    path = pathlib.Path(args.report)
    if not path.is_file():
        sys.exit(f'no report at {path}; run `python -m ninja report` first')

    try:
        report = json.loads(path.read_text(encoding='utf-8'))
    except json.JSONDecodeError as exc:
        sys.exit(f'{path} is not valid JSON: {exc}')

    measures = report.get('measures') or {}
    matched = measures.get('matched_functions')
    total = measures.get('total_functions')
    print(f'report: {path} ({matched}/{total} functions)')

    token = os.environ.get('BYTEMATCH_TOKEN')
    if not token and not args.dry_run:
        sys.exit('set BYTEMATCH_TOKEN (Manage page -> upload token)')

    commit = args.commit or git_commit()
    url = (API.format(owner=args.owner, repo=args.repo)
           + f'?version={args.version}' + (f'&commit={commit}' if commit else ''))
    print(f'POST {url}')
    print(f'commit {commit or "(none)"}')

    if args.dry_run:
        print('dry run: not uploading')
        return

    # The report grows with every matched unit and passed the server's 1 MiB
    # request limit at 1058 units, so send it compressed.
    body = gzip.compress(path.read_bytes(), 9)
    req = urllib.request.Request(
        url,
        data=body,
        # A plain urllib User-Agent is refused by the site's Cloudflare rule
        # (403, error 1010), so name the tool.
        headers={'Authorization': f'Bearer {token}',
                 'Content-Type': 'application/json',
                 'Content-Encoding': 'gzip',
                 'User-Agent': 'bytematch-upload/1.0'},
        method='POST',
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            body = resp.read().decode('utf-8', 'replace')
            print(f'{resp.status} {resp.reason}')
            if body.strip():
                print(body[:500])
    except urllib.error.HTTPError as exc:
        detail = exc.read().decode('utf-8', 'replace')[:500]
        sys.exit(f'{exc.code} {exc.reason}: {detail}')
    except urllib.error.URLError as exc:
        sys.exit(f'upload failed: {exc.reason}')


if __name__ == '__main__':
    main()

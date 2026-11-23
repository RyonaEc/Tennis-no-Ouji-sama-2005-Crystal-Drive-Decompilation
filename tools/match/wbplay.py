"""Drive the running melonDS workbench: look at the screen, press, touch, rally.

The workbench is not a video feed - `screen_capture` returns the current frame
(both screens as PNG) on every call, and `input_send` holds buttons and/or a
touch point for N frames. That is enough to see the game and to act in it, at
the cost of one request round trip per observation: roughly a second per
decision, against a game that reacts in frames. So this is fine for menus,
gates and code coverage, and not fine for winning a rally.

    python tools/match/wbplay.py status
    python tools/match/wbplay.py shot [--out build/wb]     # writes <out>_top.png,
                                                          # <out>_bottom.png
    python tools/match/wbplay.py press A 30               # hold A for 30 frames
    python tools/match/wbplay.py press A,Right 20
    python tools/match/wbplay.py tap 160 120 [frames]     # touch (x, y)
    python tools/match/wbplay.py rally [rounds] [frames]  # canned swing loop

The touch screen is its own input: some screens (the rough/smooth toss call)
cannot be cleared with buttons at all, which is how an earlier session stalled
in front of a match it thought it had entered. Send `tap` there.

Requires the workbench running with `--api`; see `notes/workbench.md` for the
`DSWB_API_INFO` and `queue.py` traps that `tools/match/coverage.py` also handles.
"""
import base64
import os
import pathlib
import sys

RPC = pathlib.Path('M:/melonDS-workbench/pydswb')
API = pathlib.Path('M:/melonDS-workbench/workbench/api.json')


def client():
    here = pathlib.Path(__file__).resolve().parent
    sys.path[:] = [p for p in sys.path if not p or pathlib.Path(p).resolve() != here]
    sys.path.insert(0, str(RPC))
    os.environ.setdefault('DSWB_API_INFO', str(API))
    from dswb import Client
    return Client.connect(agent='buffy')


def save(r, out):
    for side in ('top', 'bottom'):
        blob = getattr(r, f'{side}_png', None)
        if isinstance(blob, str):
            blob = base64.b64decode(blob)
        if blob:
            p = pathlib.Path(f'{out}_{side}.png')
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(blob)
            print(f'wrote {p} ({len(blob)} B)')


def main():
    args = sys.argv[1:]
    if not args:
        raise SystemExit(__doc__.strip().splitlines()[2])
    cmd, rest = args[0], args[1:]
    c = client()

    if cmd == 'status':
        s = c.status_get()
        print(f'frame {s.frame}  state {s.state}  fps {s.fps}  jit {s.jit_enabled}  '
              f'recording {s.recording.enabled}  write_history {s.recording.write_history}')
        return 0

    if cmd == 'shot':
        out = 'build/wb'
        if rest and rest[0] == '--out':
            out = rest[1]
        save(c.screen_capture(), out)
        return 0

    if cmd == 'press':
        buttons = [b for b in rest[0].split(',') if b]
        frames = int(rest[1]) if len(rest) > 1 else 30
        r = c.input_send(buttons=buttons, frames=frames)
        print(f'sent {buttons} for {frames} frames; frame now {c.status_get().frame}')
        return 0

    if cmd == 'tap':
        from dswb import TouchPoint
        x, y = int(rest[0]), int(rest[1])
        frames = int(rest[2]) if len(rest) > 2 else 8
        c.input_send(touch=TouchPoint(x=x, y=y), frames=frames)
        print(f'tapped ({x},{y}) for {frames} frames; frame now {c.status_get().frame}')
        return 0

    if cmd == 'rally':
        rounds = int(rest[0]) if rest else 20
        frames = int(rest[1]) if len(rest) > 1 else 90
        # The court is 3D and the ball leaves the screen, so blind play cannot
        # rally; the point is to keep swings and movement happening so the match
        # logic, animation and AI keep running for the coverage record.
        pattern = [['A'], ['Right', 'A'], ['B'], ['Left', 'B'], ['A'], ['Down', 'A']]
        for i in range(rounds):
            c.input_send(buttons=pattern[i % len(pattern)], frames=frames)
        print(f'{rounds} swings sent; frame now {c.status_get().frame}')
        return 0

    raise SystemExit(f'unknown command {cmd!r}')


if __name__ == '__main__':
    sys.exit(main())

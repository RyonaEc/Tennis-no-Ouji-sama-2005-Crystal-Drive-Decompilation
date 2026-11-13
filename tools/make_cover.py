"""Compose a 1500x500 cover image from the game icon.

ByteMatch asks for a wide cover (~1500 x 500) on the project page.  This draws
a dark green vignette and centres the 32x32 game icon over it, so the cover is
generated from the retail artwork instead of being hand-made.

Usage: python tools/make_cover.py <rom.nds> [out.png]
"""
import struct, sys, zlib

from extract_icon import read_banner, pixels

W, H = 1500, 500
ICON_SCALE = 5            # 32 * 5 = 160 px square


def chunk(tag, data):
    return (struct.pack('>I', len(data)) + tag + data
            + struct.pack('>I', zlib.crc32(tag + data) & 0xffffffff))


def background(x, y):
    """Vertical gradient plus a soft radial highlight behind the icon."""
    t = y / (H - 1)
    top = (18, 44, 30)
    bottom = (6, 12, 10)
    r = round(top[0] + (bottom[0] - top[0]) * t)
    g = round(top[1] + (bottom[1] - top[1]) * t)
    b = round(top[2] + (bottom[2] - top[2]) * t)

    dx = (x - W / 2) / (W / 2)
    dy = (y - H / 2) / (H / 2)
    d = (dx * dx + dy * dy) ** 0.5
    if d < 1.0:
        k = (1.0 - d) ** 2 * 0.35
        r = min(255, round(r + (60 - r) * k + 40 * k))
        g = min(255, round(g + (150 - g) * k + 80 * k))
        b = min(255, round(b + (80 - b) * k + 30 * k))
    return (r, g, b, 255)


def main():
    rom = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) > 2 else 'assets/cover.png'
    bitmap, palette = read_banner(rom)
    icon = pixels(bitmap, palette)

    icon_px = 32 * ICON_SCALE
    ox = (W - icon_px) // 2
    oy = (H - icon_px) // 2

    rows = []
    for y in range(H):
        row = [background(x, y) for x in range(W)]
        if oy <= y < oy + icon_px:
            iy = (y - oy) // ICON_SCALE
            for x in range(ox, ox + icon_px):
                ix = (x - ox) // ICON_SCALE
                px = icon[iy][ix]
                if px[3]:
                    row[x] = px
        rows.append(row)

    raw = bytearray()
    for row in rows:
        raw.append(0)
        for px in row:
            raw.extend(px)
    png = (b'\x89PNG\r\n\x1a\n'
           + chunk(b'IHDR', struct.pack('>IIBBBBB', W, H, 8, 6, 0, 0, 0))
           + chunk(b'IDAT', zlib.compress(bytes(raw), 9))
           + chunk(b'IEND', b''))
    open(out, 'wb').write(png)
    print(f'wrote {out} ({W}x{H}, {len(png)} bytes)')


if __name__ == '__main__':
    main()

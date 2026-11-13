"""Extract the 32x32 game icon from a DS ROM's banner and write it as a PNG.

The retail ROM is never committed, so this runs locally: it reads the banner
from the header, decodes the 4bpp icon bitmap against its 16-colour BGR555
palette, scales it up with nearest-neighbour and writes a PNG.  Used to produce
assets/icon.png for the project page.

Usage: python tools/extract_icon.py <rom.nds> [out.png] [scale]
"""
import struct, sys, zlib


def read_banner(rom_path):
    rom = open(rom_path, 'rb').read()
    banner_off = struct.unpack_from('<I', rom, 0x68)[0]
    bitmap = rom[banner_off + 0x20: banner_off + 0x20 + 512]
    palette_raw = rom[banner_off + 0x220: banner_off + 0x220 + 32]

    palette = []
    for i in range(16):
        v = struct.unpack_from('<H', palette_raw, i * 2)[0]
        r = (v & 0x1f) * 255 // 31
        g = ((v >> 5) & 0x1f) * 255 // 31
        b = ((v >> 10) & 0x1f) * 255 // 31
        palette.append((r, g, b))
    return bitmap, palette


def pixels(bitmap, palette):
    """Yield (r, g, b, a) for a 32x32 icon; palette index 0 is transparent."""
    out = []
    for y in range(32):
        row = []
        for x in range(32):
            # DS bitmaps are stored as 8x8 tiles, 4 tiles across, 4bpp
            tile = (y // 8) * 4 + (x // 8)
            off = tile * 32 + (y % 8) * 4 + (x % 8) // 2
            byte = bitmap[off]
            idx = (byte >> (4 * (x & 1))) & 0xF
            if idx == 0:
                row.append((0, 0, 0, 0))
            else:
                r, g, b = palette[idx]
                row.append((r, g, b, 255))
        out.append(row)
    return out


def write_png(path, rows, scale=1):
    h = len(rows) * scale
    w = len(rows[0]) * scale
    raw = bytearray()
    for row in rows:
        line = bytearray([0])
        for px in row:
            line.extend(bytes(px) * scale)
        for _ in range(scale):
            raw.extend(line)
    def chunk(tag, data):
        return (struct.pack('>I', len(data)) + tag + data
                + struct.pack('>I', zlib.crc32(tag + data) & 0xffffffff))
    png = (b'\x89PNG\r\n\x1a\n'
           + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0))
           + chunk(b'IDAT', zlib.compress(bytes(raw), 9))
           + chunk(b'IEND', b''))
    open(path, 'wb').write(png)
    return w, h


def main():
    rom = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) > 2 else 'assets/icon.png'
    scale = int(sys.argv[3]) if len(sys.argv) > 3 else 8
    bitmap, palette = read_banner(rom)
    rows = pixels(bitmap, palette)
    w, h = write_png(out, rows, scale)
    used = len({p[:3] for row in rows for p in row if p[3]})
    print(f'wrote {out} ({w}x{h}, {used} colours)')


if __name__ == '__main__':
    main()

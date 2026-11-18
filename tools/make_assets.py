"""Build the ByteMatch icon and cover from supplied artwork.

ByteMatch asks for a 256x256 icon and a wide (~1500x500) cover on the project
page, and serves both from this repository, so they are committed as PNGs:

    assets/icon.png    256 x 256
    assets/cover.png   1500 x 500

The artwork itself is not committed. Pass the two images you want to use: a
square-ish key art for the icon and a wide logo (ideally with transparency) for
the cover.

Usage: python tools/make_assets.py <key art> <logo> [--icon-out p] [--cover-out p]
"""
import argparse
from pathlib import Path

from PIL import Image

ICON = (256, 256)
COVER = (1500, 500)
MARGIN = 20


def make_icon(source: Image.Image) -> Image.Image:
    """Crop the key art to a square at the right edge, then scale it to the icon size.

    A box scan carries the printed spine down its left edge; anchoring the crop
    to the right drops it and keeps the cover art itself.
    """
    image = source.convert("RGB")
    side = min(image.size)
    left = image.width - side
    top = (image.height - side) // 2
    return image.crop((left, top, image.width, top + side)).resize(ICON, Image.LANCZOS)


def strip_white(logo: Image.Image, threshold: int = 236) -> Image.Image:
    """Make the white background transparent, keeping whites inside the artwork.

    Only near-white pixels reachable from the border are cleared, so the white
    highlights inside the lettering stay opaque. The flood fill runs from every
    edge pixel with an explicit stack rather than recursion.
    """
    logo = logo.convert("RGBA")
    pixels = logo.load()
    width, height = logo.size
    seen = bytearray(width * height)

    def is_white(x, y):
        r, g, b, _ = pixels[x, y]
        return r >= threshold and g >= threshold and b >= threshold

    stack = [(x, 0) for x in range(width)] + [(x, height - 1) for x in range(width)]
    stack += [(0, y) for y in range(height)] + [(width - 1, y) for y in range(height)]
    while stack:
        x, y = stack.pop()
        if not (0 <= x < width and 0 <= y < height):
            continue
        index = y * width + x
        if seen[index]:
            continue
        seen[index] = 1
        if not is_white(x, y):
            continue
        pixels[x, y] = (255, 255, 255, 0)
        stack.extend(((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)))
    return logo


def make_cover(logo: Image.Image) -> Image.Image:
    """Centre the logo on a white cover, keeping its aspect ratio."""
    cover = Image.new("RGB", COVER, (255, 255, 255))
    scale = min((COVER[0] - 2 * MARGIN) / logo.width, (COVER[1] - 2 * MARGIN) / logo.height)
    size = (round(logo.width * scale), round(logo.height * scale))
    art = logo.convert("RGBA").resize(size, Image.LANCZOS)
    cover.paste(art, ((COVER[0] - size[0]) // 2, (COVER[1] - size[1]) // 2), art)
    return cover


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("keyart", help="icon source image")
    parser.add_argument("logo", help="cover source image, alpha allowed")
    parser.add_argument("--icon-out", default="assets/icon.png")
    parser.add_argument("--cover-out", default="assets/cover.png")
    parser.add_argument("--logo-out", default="assets/logo.png",
                        help="transparent logo for the README header")
    parser.add_argument("--no-logo", action="store_true", help="skip the README logo")
    args = parser.parse_args()

    icon = make_icon(Image.open(args.keyart))
    source = Image.open(args.logo)
    cover = make_cover(source)
    outputs = [(icon, args.icon_out), (cover, args.cover_out)]
    if not args.no_logo:
        outputs.append((strip_white(source), args.logo_out))
    for image, path in outputs:
        out = Path(path)
        out.parent.mkdir(parents=True, exist_ok=True)
        image.save(out, optimize=True)
        print(f"{out.as_posix()}: {image.width}x{image.height}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

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
    args = parser.parse_args()

    icon = make_icon(Image.open(args.keyart))
    cover = make_cover(Image.open(args.logo))
    for image, path in ((icon, args.icon_out), (cover, args.cover_out)):
        out = Path(path)
        out.parent.mkdir(parents=True, exist_ok=True)
        image.save(out, optimize=True)
        print(f"{out.as_posix()}: {image.width}x{image.height}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

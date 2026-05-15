# EMONG

EMONG is a GNOME-like desktop environment project that keeps Cinnamon's visual
style and runs on the Cinnamon backend stack, including Muffin.

This repository is the working tree for that transition. See
[docs/EMONG.md](docs/EMONG.md) for the current direction and initial scope.

## Status

EMONG is in early planning and extraction work. The first milestone is to
define the Cinnamon-aligned shell surface, theme pipeline, and Muffin session
integration points.

## Contributing

For now, changes should focus on the EMONG architecture notes, theming work,
and Muffin/Cinnamon integration scaffolding.

## Build

Use `./build.sh` from the repo root. It bootstraps `.venv/` for the theme
compiler and then runs Meson configure + compile.

## Notes

This tree still contains substantial GNOME Shell heritage. The intent is to
adapt the behavior to EMONG while replacing Mutter-facing pieces with Muffin
where the backend boundary allows it.

## Documentation

 * [Building and Running][building]
 * [Debugging][debugging]

[building]: docs/building-and-running.md
[debugging]: docs/debugging.md

## API Reference

This will be replaced with EMONG-specific docs as the new shell surfaces are
carved out.

## License

EMONG is distributed under the terms of the GNU General Public License,
version 2 or later. See the [COPYING][license] file for details.

[license]: COPYING

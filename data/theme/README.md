## Summary

EMONG theme work should live in the source SCSS files. The generated CSS is
built automatically by meson + ninja and then packaged into the runtime
gresource bundle. Install `sassc` before working on the theme.

## How to tweak the theme

EMONG's visual direction is Cinnamon-inspired: clear panel hierarchy, softer
surface contrast, restrained borders, and readable shadows. Keep changes in
the SASS sources so the palette and spacing rules stay centralized.

Most changes will happen in the [_common.scss][common] file. That is where the
shared widget selectors live. The other files are mostly supporting pieces and
should stay small unless EMONG needs a specific shell treatment:

| File                     | Description       |
| ------------------------ | ----------------- |
  | [_colors.scss][colors]   | global color definitions. Keep the EMONG palette small and derive as much as possible from the accent set. |
  | [_drawing.scss][drawing] | drawing helper mixins/functions used by the widget definitions. |
  | [_common.scss][common]   | actual definitions of style for each widget. This is where most EMONG theme changes belong. |

You can read about SASS on its [web page][sass-web]. Once you make your
changes to [_common.scss][common], run ninja to generate the final CSS files.

[common]: data/theme/gnome-shell-sass/_common.scss
[colors]: data/theme/gnome-shell-sass/_colors.scss
[drawing]: data/theme/gnome-shell-sass/_drawing.scss
[sass-web]: http://sass-lang.com/documentation/

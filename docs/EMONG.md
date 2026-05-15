# EMONG Direction

EMONG is the working name for a GNOME-like desktop stack with Cinnamon visuals
and Muffin as the compositor/window-manager backend.

## Goals

* Cinnamon-style visual language and theming
* Muffin backend integration
* XLibre/X11-first session support
* Keep the shell surface lightweight and modular
* Build new EMONG-specific branding and settings flow

## Non-goals

* Forking GNOME Shell further
* Requiring GNOME design process for EMONG changes
* Binding the project to a single upstream aesthetic forever

## Initial Work

1. Replace GNOME Shell branding with EMONG branding.
2. Define the Cinnamon theme assets and color tokens.
3. Identify the shell components that need a Muffin-facing replacement layer.
4. Keep the current codebase functional while the new direction lands.

## Next Milestone

Create an EMONG shell theme package and a small launcher/panel prototype that
matches Cinnamon conventions.

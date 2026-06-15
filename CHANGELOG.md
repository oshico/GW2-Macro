# Changelog — Macro Keybind Manager

## v2026.06.15.1400

### What changed

- **Massively expanded the list of supported game binds.** You can now use just about everything Nexus exposes — targeting, UI panels, camera controls, mounts, masteries, map, screenshots, toys, build and equipment templates, and a bunch of misc actions like AoE loot or stow/draw weapons. The only things left out are character movement (forward/strafe/etc.), spectator binds, and squad markers, since those don't really belong in a macro tool. About Face did make the cut though.

- **Picked a category first, then the action.** Instead of one giant flat dropdown, the editor now has two combos: pick a category (Skills, Targeting, Mounts, etc.), then pick the specific action. Makes it way easier to find what you're looking for.

- **Fixed the CI build.** The workflow was missing `submodules: recursive` on checkout, so the build would fail on GitHub runners since imgui, mumble, and nexus live in submodules now.

- **Cleaned up header files.** All those `extern` keywords on function declarations were just noise — in C++ they're implicit at namespace scope. Only `shared.h` still uses them (for global variables, where it actually matters).

### Supported actions now

Movement: Dodge, Jump, About Face.

Skills: Weapon 1–5, Heal, Utility 1–3, Elite, Profession 1–7, Weapon Swap, Special Action.

Targeting: Take/Call/Alert target, nearest/next/previous enemy or ally, lock target, snap ground target, auto-targeting toggles, ally targeting modes.

UI: Trading Post, Contacts, Guild, Hero, Inventory, Pets, Logout, Mail, Options, Party, PvP, PvP Build, Scoreboard, Wizard's Vault, Information, chat commands/focus/reply/toggle, squad broadcast chat.

Camera: Free Camera, Zoom In/Out, Reverse, Action Mode.

Screenshots: Normal, Stereoscopic.

Map: Toggle, Focus Player, Floor Up/Down, Zoom In/Out.

Mounts: Mount/Dismount, Mount Ability 1/2, all nine mounts (Raptor through Siege Turtle).

Mastery: Mastery, Fishing, Skiff, Jade Bot Waypoint, Rift Scan, Skyscale, Homestead Doorway.

Misc: AoE Loot, Interact, Show Enemies/Allies, Stow/Draw Weapons, Toggle Language, Toggle Pet Combat, Toggle Fullscreen, Toggle Decorate Mode.

Toys: Default, Chair, Instrument, Held Item, Toy, Tonic.

Build Templates 1–9, Equipment Templates 1–9.
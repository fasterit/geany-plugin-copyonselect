# Copy on Select - Geany Plugin

A Geany plugin that automatically copies selected text to the X selection (primary clipboard), enabling natural middle-click paste functionality in Geany.
I.e. you can place the cursor with a left-click (or multiple) and then paste with the middle-mouse button. Just like it works in any non-GNOME editor.

## Features

- **Automatic Copying**: Selected text is immediately copied to the X primary selection
- **Middle-Click Paste**: Paste selected text in any X application (and geany) using the middle mouse button even after a left-click to place the cursor
- **Lightweight**: Minimal overhead with efficient signal handling
- **Non-Intrusive**: Works silently in the background without affecting normal clipboard operations

## Requirements

- Geany text editor (version 1.22 or later)
- GDK 3.0 or GDK 2.0 (for clipboard functionality, part of GTK+)
- GLib 2.0
- X11 windowing system
- Development packages: `libgtk-3-dev` (or `libgtk2.0-dev` for older systems)
  - Note: Only GDK and GLib are linked; the full GTK+ widget library is not used

## Installation

### Building from Source

1. **Install dependencies**:

  For Debian/Ubuntu:
   ```bash
   sudo apt-get install geany libgtk-3-dev build-essential pkg-config
   ```

   For Fedora/RHEL:
   ```bash
   sudo dnf install geany gtk3-devel gcc make pkgconfig
   ```

2. **Clone and build**:
   ```bash
   git clone https://git.faster-it.de/geany-plugin-copyonselect
   cd geany-plugin-copyonselect
   make
   ```

3. **Install the plugin**:
   ```bash
   make install
   ```

   This installs the plugin to `~/.config/geany/plugins/`. For system-wide installation:
   ```bash
   # Common locations: /usr/lib/geany, /usr/lib/x86_64-linux-gnu/geany, /usr/local/lib/geany
   # Check your system's Geany plugin directory first
   sudo make install GEANY_PLUGIN_DIR=/usr/lib/geany
   ```

### Enabling the Plugin

1. Open Geany
2. Go to **Tools → Plugin Manager**
3. Find "Copy on Select" in the list
4. Check the box to enable it

## Usage

Once enabled, the plugin works automatically:

1. Select any text in Geany
2. The text is immediately copied to the X primary selection
3. You can now left click in Geany to place the cursor and then middle-click to paste the text
4. You can middle-click in any other X application to paste the selected text as well

No configuration is needed - it just works.

## How It Works

The plugin hooks into Geany's editor notification system and listens for selection change events (`SCN_UPDATEUI` with `SC_UPDATE_SELECTION`). When text is selected, it:

1. Retrieves the selected text range from the Scintilla editor
2. Copies the text to the GTK clipboard with `GDK_SELECTION_PRIMARY`
3. Stores the clipboard data to make it available to other X applications

This follows the X11 convention where the primary selection (middle-click paste) is separate from the clipboard (Ctrl+C/Ctrl+V).

This fixes the long standing problem that Geany adheres to GNOME UI standards (middle-click paste without placing the cursor first) that are not matching the modern Linux desktop expectations (any more).
Cf. https://users.geany.narkive.com/BJh5jBYs/geany-problem-with-mouse-copy-paste .

## Uninstallation

```bash
make uninstall
```

Or manually remove:
```bash
rm ~/.config/geany/plugins/copyonselect.so
```

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

## Credits

This plugin has been developed by Faster IT GmbH, Düren, Germany.

## Outlook

Depending on the general feedback about this plugin, we intend to submit it for inclusion with the general [geany plugins collection](https://github.com/geany/geany-plugins).

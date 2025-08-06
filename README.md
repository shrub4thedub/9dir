# 9dir - Plan 9 Visual Directory Browser for Linux

A minimalistic visual directory browser ported from Plan 9 vdir to Linux with X11, featuring comprehensive configuration support and intelligent file type handling.

## Features

- **Visual file browser** with toolbar and scrollable directory listing
- **Comprehensive configuration system** with `~/.config/vdir/config`
- **Customizable colors and fonts** via configuration file
- **Configurable editor integration** (default: acme)
- **Hidden files support** (configurable)
- **Window size configuration**
- **Improved icons** with Plan 9-inspired design
- **Keyboard and mouse navigation**
- **File and directory operations** (create, delete, rename)

## Dependencies

- X11 development libraries (`libx11-dev` on Debian/Ubuntu)
- Standard C library with POSIX extensions
- GCC or compatible C compiler
- Make

## Installation

```bash
# Install dependencies (Debian/Ubuntu)
sudo apt-get install libx11-dev build-essential

# Build the application
make

# Install system-wide (optional)
sudo make install
```

## Configuration

vdir automatically creates a configuration file at `~/.config/vdir/config` on first run. You can customize all aspects of the application through this file.

### Configuration File Format

```ini
# vdir Linux Configuration File
# Colors are in hex format (#RRGGBB)

# Font settings
font_name = sleekspace
font_size = 12

# Color scheme
bg_color = #FFFFFF
fg_color = #000000
toolbar_bg = #EFEFEF
toolbar_fg = #000000
selected_bg = #EFEFEF
selected_fg = #000000
scrollbar_bg = #999999
scrollbar_fg = #FFFFFF
highlight_color = #00AA00

# Editor settings
editor_command = acme
image_viewer_command = feh

# Window settings
window_width = 800
window_height = 600

# Behavior settings
show_hidden_files = 0
double_click_to_open = 0

# Icon settings
show_icons = 1
icon_theme = default
```

### Configuration Options

#### Font Settings
- **`font_name`**: Font family name (default: sleekspace)
- **`font_size`**: Font size in points (default: 12) - *Note: Currently display-only*

#### Colors
All colors use hex format (#RRGGBB):
- **`bg_color`**: Main background color
- **`fg_color`**: Main text color
- **`toolbar_bg`**: Toolbar background
- **`toolbar_fg`**: Toolbar text and icons
- **`selected_bg`**: Selected item background
- **`selected_fg`**: Selected item text
- **`scrollbar_bg`**: Scrollbar background
- **`scrollbar_fg`**: Scrollbar handle
- **`highlight_color`**: Accent color (used for + icons)

#### Application Settings
- **`editor_command`**: Command to open text files (default: acme)
- **`image_viewer_command`**: Command to open image files (default: feh)
- **`window_width`**: Initial window width (default: 800)
- **`window_height`**: Initial window height (default: 600)
- **`show_hidden_files`**: Show files starting with '.' (0=no, 1=yes)
- **`double_click_to_open`**: Require double-click to open (0=single, 1=double)
- **`show_icons`**: Display file/folder icons (0=no, 1=yes)
- **`icon_theme`**: Icon theme name (default: default)

## Usage

```bash
# Start vdir in current directory
./vdir-linux

# Start vdir in specific directory
./vdir-linux /path/to/directory
```

## Controls

### Mouse Controls:
- **Left click on toolbar icons:**
  - Home icon: Go to home directory
  - Up arrow: Go to parent directory  
  - Right arrow: Enter directory path manually
  - Folder+ icon: Create new directory
  - File+ icon: Create new file

- **Left click on items:** Open directories or files
- **Right click on items:** Delete or rename menu
- **Mouse wheel:** Scroll through directory listing

### Keyboard Shortcuts:
- **q** or **Delete**: Quit application
- **Page Up/Down**: Scroll directory listing
- **Home**: Go to home directory
- **Up Arrow**: Go to parent directory
- **Space**: Print current path (to console)

## Theming Examples

### Dark Theme
```ini
bg_color = #2B2B2B
fg_color = #FFFFFF
toolbar_bg = #3C3C3C
toolbar_fg = #FFFFFF
selected_bg = #4A4A4A
selected_fg = #FFFFFF
scrollbar_bg = #1E1E1E
scrollbar_fg = #5A5A5A
highlight_color = #00AA00
```

### Plan 9 Acme Theme
```ini
bg_color = #FFFFEA
fg_color = #000000
toolbar_bg = #EAFFFF
toolbar_fg = #000000
selected_bg = #EEEE9E
selected_fg = #000000
scrollbar_bg = #99994C
scrollbar_fg = #FFFFEA
highlight_color = #006600
```

### High Contrast Theme
```ini
bg_color = #000000
fg_color = #FFFFFF
toolbar_bg = #333333
toolbar_fg = #FFFFFF
selected_bg = #FFFFFF
selected_fg = #000000
scrollbar_bg = #666666
scrollbar_fg = #FFFFFF
highlight_color = #FFFF00
```

## File Structure

- `vdir_linux.c` - Main application and X11 interface
- `config.c/h` - Configuration file management
- `theme_linux.c/h` - Color management (legacy theme support)
- `icons_linux.c/h` - Icon rendering system
- `dialogs_linux.c/h` - Alert and input dialogs
- `Makefile` - Build system

## Building from Source

```bash
git clone <repository>
cd vdir-linux
make clean
make
```

For debugging:
```bash
make debug
```

For optimized release:
```bash  
make release
```

## File Type Handling

vdir automatically detects file types and opens them with the appropriate application:

### Image Files
Image files (jpg, jpeg, png, gif, bmp, tiff, tif, webp, svg, ico) are automatically opened with the `image_viewer_command`:
- Default: `feh` (lightweight image viewer)
- Common alternatives: `eog`, `gwenview`, `ristretto`, `sxiv`

### Text Files  
All other files are opened with the `editor_command`:
- Default: `acme` (Plan 9 editor)
- Common alternatives: `vim`, `emacs`, `gedit`, `code`

## Editor Integration

vdir can be configured to open files with any editor by setting the `editor_command` option:

```ini
# For acme (default)
editor_command = acme

# For vim in terminal
editor_command = xterm -e vim

# For emacs
editor_command = emacs

# For VS Code
editor_command = code

# For gedit
editor_command = gedit

# Image viewer examples
image_viewer_command = feh        # Default
image_viewer_command = eog        # GNOME Image Viewer  
image_viewer_command = gwenview   # KDE Image Viewer
image_viewer_command = ristretto  # XFCE Image Viewer
image_viewer_command = sxiv       # Simple X Image Viewer
```

## Advanced Configuration

### Multiple Font Fallback
The font loading system automatically falls back through:
1. Configured font name
2. Fixed-width system font
3. Any available font

### Configuration File Location
The configuration is loaded from:
1. `~/.config/vdir/config` (user configuration)
2. Defaults are used if no config file exists
3. Config file is automatically created with defaults on first run

### Hidden Files
When `show_hidden_files = 1`, all files starting with '.' are shown except for the current directory ('.') which is always hidden.

## Differences from Plan 9 Version

### Removed Features:
- Plan 9 plumber integration (replaced with configurable editor)
- Plan 9 specific fonts (replaced with X11 font system)
- Plan 9 threading model (simplified to single-threaded)

### Added Features:
- Comprehensive configuration system
- Customizable colors and editor
- Hidden files toggle
- Window size configuration
- Enhanced error handling

### Technical Changes:
- Replaced Plan 9 `draw` library with X11 Xlib
- Replaced Plan 9 `thread` library with simplified event loop
- Replaced Plan 9 file APIs with POSIX dirent/stat
- Added configuration file parsing and management
- Replaced Plan 9 `plumber` with configurable editor command

## Troubleshooting

### Cannot load font
If your configured font is not available, vdir will fall back to system fonts. Check available fonts with:
```bash
xlsfonts | grep your-font-name
```

### Config file not found
vdir automatically creates `~/.config/vdir/config` on first run. If you want to recreate it:
```bash
rm ~/.config/vdir/config
./vdir-linux  # Will recreate with defaults
```

### Colors not working
Ensure colors are in proper hex format (#RRGGBB). Invalid colors will fall back to black.

## License

Same as original Plan 9 vdir (check original source for license terms).

## Contributing

Contributions welcome for:
- Additional themes and color schemes
- Font size support implementation
- Better icon loading (PNG/XPM support)
- Native X11 modal dialogs
- Improved error handling

## Acknowledgments

- Original vdir for Plan 9 by the Plan 9 development team
- Configuration system inspired by modern Linux applications

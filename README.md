# 9dir

minimal file browser based on plan9's vdir

this software is provided to you by shrub industries under the terms of the GNU GPL version 3 license

much of this is to be tested - there are likely bugs

here is an image.

<img width="1600" height="900" alt="9dir" src="https://github.com/user-attachments/assets/097feb2d-4326-40ff-bae4-e11ca10673a8" />

## dependencies

- X11 development libraries (`libx11-dev` on Debian/Ubuntu)
- Standard C library with POSIX extensions
- GCC or compatible C compiler
- Make

## installation

```bash
# Install dependencies (debian-based)
sudo apt-get install libx11-dev build-essential
git clone https://github.com/shrub4thedub/9dir
cd 9dir
make
sudo make install
```

## configuration

9dir automatically creates a configuration file at `~/.config/9dir/config` on first run. You can customize all aspects of the application through this file.

### configuration File Format

```ini
# 9dir config
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

### configuration options

#### font settings
- **`font_name`**: Font family name (default: sleekspace)
- **`font_size`**: Font size in points (default: 12) - *Note: Currently display-only*

#### colors
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

#### application settings
- **`editor_command`**: Command to open text files (default: acme)
- **`image_viewer_command`**: Command to open image files (default: feh)
- **`window_width`**: Initial window width (default: 800)
- **`window_height`**: Initial window height (default: 600)
- **`show_hidden_files`**: Show files starting with '.' (0=no, 1=yes)
- **`double_click_to_open`**: Require double-click to open (0=single, 1=double)
- **`show_icons`**: Display file/folder icons (0=no, 1=yes)
- **`icon_theme`**: Icon theme name (default: default)

## usage

```bash
# Start vdir in current directory
9dir

# Start vdir in specific directory
9dir /path/to/directory
```

## controls

### mouse Controls:
- **Left click on toolbar icons:**
  - Home icon: Go to home directory
  - Up arrow: Go to parent directory  
  - Right arrow: Enter directory path manually
  - Folder+ icon: Create new directory
  - File+ icon: Create new file

- **Left click on items:** Open directories or files
- **Right click on items:** Delete or rename menu
- **Mouse wheel:** Scroll through directory listing

### keyboard Shortcuts:
- **q** or **Delete**: Quit application
- **Page Up/Down**: Scroll directory listing
- **Home**: Go to home directory
- **Up Arrow**: Go to parent directory
- **Space**: Print current path (to console)

## acknowledgments

- Original vdir for Plan 9 by telephil9 available at https://github.com/telephil9/vdir

# vdir - Linux X11 Port

This is a port of the vdir visual directory browser from Plan 9 to Linux with X11.

## Overview

vdir is a minimalistic visual directory browser originally written for Plan 9. This Linux port maintains the same functionality while using X11 for graphics and standard POSIX APIs for file operations.

## Features

- Visual file browser with toolbar and scrollable directory listing
- Keyboard shortcuts for navigation
- Mouse-driven interface
- File and directory operations (create, delete, rename)
- Improved icon representation for files and folders
- Theme support via configuration files
- Opens files in acme editor
- Uses sleekspace font (with fallback to system fonts)

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

# Create example theme (optional)
make theme-example
```

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

## Configuration

### Theme Files

You can customize colors by creating a theme file:

- User theme: `~/.config/vdir/theme`
- System theme: `/etc/vdir/theme`

Theme format:
```
# vdir theme file
back 0xFFFFFF    # Background color
text 0x000000    # Text color  
border 0xCCCCCC  # Border color
high 0x00AA00    # Highlight color
```

Colors are specified in hexadecimal RGB format.

## Differences from Plan 9 Version

### Removed Features:
- Plan 9 plumber integration (replaced with xdg-open)
- Plan 9 specific fonts (uses X11 fixed font)
- Plan 9 threading model (simplified to single-threaded)

### Added Features:
- X11 integration for modern Linux desktops
- Standard file operations using POSIX APIs
- Console-based dialogs (can be enhanced with X11 dialogs)
- acme editor integration for file opening
- sleekspace font support with automatic fallback

### Technical Changes:
- Replaced Plan 9 `draw` library with X11 Xlib
- Replaced Plan 9 `thread` library with simplified event loop
- Replaced Plan 9 file APIs with POSIX dirent/stat
- Replaced Plan 9 `plumber` with acme editor for file opening

## File Structure

- `vdir_linux.c` - Main application and X11 interface
- `theme_linux.c/h` - Theme loading and color management
- `icons_linux.c/h` - Simple pixmap-based icons
- `dialogs_linux.c/h` - Alert and input dialogs
- `Makefile` - Build system

## Building from Source

```bash
git clone <repository>
cd vdir
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

## Known Limitations

1. **Icons**: Currently uses simple geometric shapes instead of proper icons
2. **Dialogs**: Uses console input instead of proper X11 modal dialogs  
3. **Font**: Limited to X11 server fonts, no custom font loading
4. **Threading**: Single-threaded, may block on file operations
5. **Error Handling**: Basic error reporting compared to Plan 9 version

## Future Improvements

- Load proper PNG/XPM icons
- Implement native X11 modal dialogs
- Add font configuration support
- Improve error handling and user feedback
- Add more keyboard shortcuts
- Implement drag and drop support

## License

Same as original Plan 9 vdir (check original source for license terms).

## Contributing

This is a demonstration port showing how Plan 9 applications can be adapted to Linux. Contributions welcome for:

- Better icon support (PNG/XPM loading)
- Native X11 dialogs
- Font configuration
- Theme improvements
- Bug fixes

## Acknowledgments

Original vdir for Plan 9 by the Plan 9 development team.
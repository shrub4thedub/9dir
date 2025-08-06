# vdir Linux Port - Recent Improvements

## Latest Changes (Image File Support)

### 1. Smart File Type Detection
- **Automatic image detection** based on file extensions
- **Supported formats**: jpg, jpeg, png, gif, bmp, tiff, tif, webp, svg, ico
- **Case-insensitive matching** for all extensions
- **Configurable image viewer** separate from text editor

### 2. Dual Application Support
- **Image files** → opened with `image_viewer_command` (default: feh)
- **Text files** → opened with `editor_command` (default: acme)
- **Background execution** maintains vdir responsiveness
- **User customizable** via config file

### 3. Enhanced Configuration
- **New config option**: `image_viewer_command = feh`
- **Automatic config migration** with new defaults
- **Documentation examples** for popular image viewers
- **Runtime file type detection** without caching

## Previous Changes (Configuration System)

### 1. Comprehensive Configuration System
- **Config file location**: `~/.config/vdir/config`
- **Automatic creation** of default config on first run
- **Runtime configuration loading** with error handling
- **Memory management** for dynamic config values

### 2. Configurable Colors
- **All UI colors customizable** via hex color codes (#RRGGBB)
- **Separate color controls** for toolbar, background, selection, scrollbar
- **Predefined color schemes** in documentation
- **Runtime color parsing** with fallback to safe defaults

### 3. Font Configuration
- **Configurable font family** with automatic fallback
- **Font pattern generation** for X11 font system
- **Graceful degradation** when fonts are unavailable

### 4. Editor Configuration
- **Configurable editor command** (replaces hardcoded acme)
- **Support for any editor** via command line
- **Background execution** to keep vdir responsive

### 5. Window and Behavior Settings
- **Configurable window size** (width/height)
- **Hidden files toggle** with smart filtering
- **Future-ready settings** for double-click, icon themes

### 6. Code Architecture Improvements
- **Modular configuration system** (config.c/config.h)
- **Clean separation** of concerns
- **Memory-safe string handling**
- **Comprehensive error handling**

## Previous Changes Made

### 1. Font Support
- **Updated font loading** to prioritize the sleekspace font
- **Automatic fallback** to system fonts if sleekspace is not available
- **Font loading order**: sleekspace → fixed → any available font

### 2. acme Editor Integration
- **File opening** now launches acme instead of the default system application
- **Command**: `acme "/path/to/file" &` 
- **Background execution** to keep vdir responsive

### 3. Improved Icons
- **Better geometric icons** inspired by Plan 9 design principles
- **Proper rendering** with filled shapes instead of simple outlines
- **Color-coded elements**: Green plus signs for "new" operations
- **Toolbar icons**: Home, up arrow, right arrow, new folder, new file
- **Directory listing icons**: Distinct folder and file representations

### Icon Improvements Details:
- **Home icon**: Detailed house shape with door
- **Folder icon**: Filled folder with tab and interior
- **File icon**: Document with visible text lines
- **Up arrow**: Solid arrow pointing up
- **Right arrow**: Solid arrow pointing right (for "go to directory")  
- **New file**: Document with green plus sign
- **New folder**: Folder with green plus sign

### 4. Visual Polish
- **Light gray backgrounds** for icons instead of pure white
- **Consistent styling** across all icons
- **Better contrast** and readability
- **Plan 9 aesthetic** maintained in Linux environment

## Code Changes

### Main Files Modified:
- `vdir_linux.c`: Font loading and file opening logic
- `icons_linux.c`: Complete icon rendering rewrite
- `README_LINUX.md`: Documentation updates

### Font Loading Logic:
```c
// Try sleekspace first, then fallback
font = XLoadQueryFont(display, "-*-sleekspace-*-*-*-*-*-*-*-*-*-*-*-*");
if (font == NULL) {
    font = XLoadQueryFont(display, "fixed");
    if (font == NULL) {
        font = XLoadQueryFont(display, "*");
    }
}
```

### File Opening Logic:
```c
// Open file in acme
char cmd[PATH_MAX + 20];
snprintf(cmd, sizeof(cmd), "acme \"%s/%s\" &", path, d.name);
system(cmd);
```

## User Experience Improvements

1. **Better font rendering** with sleekspace for improved readability
2. **Seamless acme integration** for Plan 9-style text editing workflow
3. **Visual clarity** with improved icons that are easier to distinguish
4. **Consistent design language** that respects Plan 9 aesthetics
5. **Responsive interface** with background file opening

## Future Enhancement Opportunities

While the current improvements significantly enhance the user experience, future enhancements could include:

- Loading actual Plan 9 RGBA icon data for pixel-perfect reproduction
- Implementing proper X11 modal dialogs instead of console prompts
- Adding font size configuration options
- Supporting additional editors beyond acme
- Implementing drag and drop functionality

These improvements make the Linux port much closer to the original Plan 9 experience while taking advantage of modern Linux capabilities.
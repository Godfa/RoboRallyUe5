# Card Icon Textures

This folder contains SVG icons for Robot Rally card actions.

## Converting SVG to PNG for Unreal Engine

Unreal Engine requires PNG or other raster formats for textures. Convert these SVG files to PNG using one of these methods:

### Method 1: Online Converter (Quick)
1. Go to https://cloudconvert.com/svg-to-png
2. Upload each SVG file
3. Set size to 64x64 or 128x128 pixels
4. Download PNG files

### Method 2: Inkscape (Batch Convert)
```bash
inkscape --export-type=png --export-width=64 --export-height=64 T_Icon_Move1.svg
inkscape --export-type=png --export-width=64 --export-height=64 T_Icon_Move2.svg
# ... repeat for all files
```

### Method 3: ImageMagick (Command Line)
```bash
magick convert -background none -size 64x64 T_Icon_Move1.svg T_Icon_Move1.png
magick convert -background none -size 64x64 T_Icon_Move2.svg T_Icon_Move2.png
# ... repeat for all files
```

### Method 4: GIMP
1. Open SVG in GIMP
2. Set import size to 64x64
3. Export as PNG with transparency

## Icon Descriptions

- **T_Icon_Move1**: Single forward arrow (Move 1 space)
- **T_Icon_Move2**: Double forward arrows (Move 2 spaces)
- **T_Icon_Move3**: Triple forward arrows (Move 3 spaces)
- **T_Icon_MoveBack**: Backward arrow (Move 1 space backward)
- **T_Icon_RotateRight**: Clockwise curved arrow (Rotate 90° right)
- **T_Icon_RotateLeft**: Counter-clockwise curved arrow (Rotate 90° left)
- **T_Icon_UTurn**: U-shaped arrow (Rotate 180°)

## Import to Unreal Engine

After converting to PNG:

1. In Unreal Editor Content Browser, navigate to this folder
2. Drag and drop all PNG files
3. For each texture, set these properties:
   - **Compression Settings**: UserInterface2D
   - **Texture Group**: UI
   - **sRGB**: Enabled
   - **Never Stream**: True

## Replacing Icons

These are placeholder icons. To replace with better graphics:
1. Create new 64x64 (or larger) PNG files with white icons on transparent background
2. Name them exactly as listed above (T_Icon_*.png)
3. Reimport or replace in Unreal Engine
4. The Widget Blueprint will automatically use the new icons

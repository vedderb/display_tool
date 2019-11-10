# Display Tool

Display Tool is a graphical editor written in the Qt framework that can be used to create graphics for displays with 16 brightness levels per pixel. It can also generate bitmap fonts from system fonts (with or without antialiasing) and export them to C files. The graphics editing is mostly useful for the VESC Wand display, but the font generation is likely to be useful for a wide variety of displays. It should be fairly easy to adapt display tool for other types of displays. Some features:

* Draw images pixel-by-pixel
* Import PNG or JPEG images with optional dithering and antialiasing.
* Export images and fonts as C code.
* Generate bitmap fonts from system fons with or without antialiasing.
* Export bitmap fonts as C files.
* Create image on second layer and import it to the first layer with transforms (scaling, rotation, translation) that are available in the VESC Wand oled library.

Display tool can be built and run in the usual qmake way:

```bash
qmake
make
./display_tool
```

Once display tool is running, some usage instructions can be found under Help -> Show instruction sheet.


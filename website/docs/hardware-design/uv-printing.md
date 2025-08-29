---
sidebar_position: 1
---

# UV Printing How To

[Based on this guide, but adjusted for KiCAD 9](https://www.hackster.io/zst123/printing-full-color-pcb-artwork-using-pcbway-dd5298)

1. In KiCAD project File -> New Jobset file and Create a new one.
2. Press the + to create a new jobset
3. Choose PCB: Export SVG
    - Include Layers: F.Silkscreen, B.Silkscreen, F.Mask, B.Mask.
    - Plot on All Layers: Edge.Cuts
    - General Options: Uncheck all boxes except "Fit page to board".
    - Drill marks: None
    - Precistion: 6
    - OK
4. Save Jobset for easy use later
4. Press Generate All Destinations 
5. Now you find a .svg in the folder with the same name as the jobset.

Now go to [https://svgcrop.com/](https://svgcrop.com/) and drag in all svg:s at once in order to crop them.

Now the Front and Bottom Silkscreen can be used as a base to crate a nice looking UV print.
The Front and Bottom Mask shall not be part of the final UV print, but they can be used as a "keepout" reminder. So suggest to add it as a layer in the image and then skip exporting it.

Do note that there will be a screen on the large left area as seen in the real image shown of the current design. So assume the user can't see that area so avoid putting anything relevant there.

There should be no transparency on the image, i.e. background has to be filled.
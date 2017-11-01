# RepaintBrush
RepaintBrush is a tool for managing folders of images, where images are
continually inserted into said folder. More specifically, it is used for
'repainting' video games; that is, replacing almost every texture in a game
with gags for the sake of comedy. Generally with emulators, this means
playing through a game to 'dump' textures as they are encountered in game.
This tool makes organizing these 'dumped' textures much easier, as they can
be sorted as they are found.

# Downloading
You can download the latest build from the
[Releases](https://github.com/Jellonator/repaintbrush/releases) page.

# How to use this tool
There are two methods you can utilize to use this tool. You can run it from the
command line, or you can use the GUI. You can access the GUI by either executing
the program without any arguments, or by double clicking the executable from
your file manager.

## GUI
The GUI is the most straight-forward method of using this program.

### Managing projects
A project is how you manage files. A project has a set of input folders, a set
of filters, and a set of files that it is currently managing. These will all be
discussed later.

To create a new project, go to File->New; or you can use File->Open to open an
existing project. When you are done with this project, you can use File->Close
to close the opened project, or use File->Exit to close the entire application.

### Input folders
An 'input folder' is a folder where images are pulled from. You can add an input
by clicking on the 'Add Input' button. You can remove inputs by selecting an
input from the list and clicking the 'Remove Input' button. Later when you start
importing images, files will be pulled from these folders.

If you are using Dolphin, you will probably be adding an input folder from
Dolphin's 'Dump/Textures/{Game Code}' directory.

### Filters
A 'filter' is a way to only include images that you want, and ignore the
images that you don't care about. For example, a mipmap filter will remove
mipmap images. Filters come in two flavors: input filters, and output
filters. An input filter will prevent images from being imported, and an
output filter will prevent images from being exported.

An output filter may be useful if you have a 'junk' folder containing images
that you will manually sort out so that they aren't exported. In this case, you
will create a 'path' filter of type 'output' with the argument 'junk', which is
relative to the project directory.

### Importing images
Click on the 'Import' button when you are ready to import images. Images will
be imported from all inputs. Only images that have not already been imported
and are not filtered by any input filters will be included. When you import
images, you will be prompted to select a folder to import the images into.

### Exporting images
Click on the 'Export' button when you want to export images. All images in
your project folder that have been previously imported and do not match any
output filters will be exported. Note that this means that if you put an
image into the project folder yourself, it will not be registered and will
therefor not be included in the exporting process.

# Compiling
This project only has two dependencies: wxWidgets (for the GUI), and sqlite3.
Make sure to install these before you compile this project.
To build this project, you need [Scons](http://scons.org/). To compile from
the command line, run the following command in the project directory:
```
scons
```

Generally, you'll probably want to download from the releases page.

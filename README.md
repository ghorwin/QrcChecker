# QrcChecker

Tool to check and clean out project's QRC files and resource directories (during refactoring/code cleanups).

The tool checks for the following situations:

- resource/pixmap is referenced in qrc and correctly used in the source code
- source code contains invalid qrc-path (not detected at compile time, and difficult to find through testing)
- resource/pixmap is referenced in qrc but nowhere used (i.e. unused and could be deleted)
- resource/pixmap is in directory structure but not (yet) referenced in qrc (i.e. unused and could be deleted)

and provides some automatic functionality for cleaning out qrc files and resource directories.

## Functionality

1. select a base directory of your source code, usually the top-level directory that contains both source code files, ui-files, the qrc-file(s) and the images
2. select the qrc-file(s) to scan
3. select image types or other resource file types (you can enter a semi-colon separated list of custom wildcards, like `*.txt;*.md`)
4. click on _Scan QRC and directory structure_

![Screenshot](https://user-images.githubusercontent.com/6892676/152869135-f8db4795-1fdd-41f1-ad46-aaf45c982d18.png)

The table will now be populated with a colored list of scan results:

- magenta - files that appear in the qrc but are no-where used in the code
- blue - files that are in the directory structure, but not referenced in qrc
- red - files that are referenced via QRC-path in the source code/ui-files, but are not anywhere in the qrc; **these are the real bad guys to fix!**


## Other QRC related tools

See also the QrcChecker script https://github.com/miurahr/qrcchecker, that generates a QRC file based on the contents of a directory tree.


# QrcChecker
Tool to check Qt qrc-files, find and remove unused resource files and detect invalid qrc-path.

Purpose: Check and clean out project's QRC files and resource directories (during refactoring/code cleanups).

Basically, the tool checks for the following situations:

- resource/pixmap is referenced in qrc and correctly used in the source code
- source code contains invalid qrc-path (not detected at compile time, and difficult to find through testing)
- resource/pixmap is referenced in qrc but nowhere used (i.e. unused and could be deleted)
- resource/pixmap is in directory structure but not (yet) referenced in qrc (i.e. unused and could be deleted)

## Functionality

... screenshots follow...





## Other QRC related tools

See also the QrcChecker script https://github.com/miurahr/qrcchecker, that generates a QRC file based on the contents of a directory tree.

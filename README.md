# Image resizer

## About the project

A command line tool written in C that resizes 24-bit BitMaps, based on a CS50 project.

## Usage

For this program a C/C++ compiler is needed, e.g with gcc:

```bash
gcc resize.c -o resize
```

To resize an image, the executable needs to run with the following syntax, on Linux:

```bash
./resize factor infile_path outfile_path
```

## Features

This program allows to enlarge and shrink images up to a factor of 100. It shrinks from 0 to 1, taking decimal values, and enlarges them from 1 to 100. The BMP data types are based on Microsoft's own.

## Project reflections

In this project I learned three main things:

- How to make the code more modular, programming it in a functional style. Trying to build functions that are small and respond to a single responsability.
- How to manipulate data using pointers, from reading and writting to disk to dynamically allocating memory.
- On Windows, in contrast to Linux, a program treats text files and binary files differently. Linux reads files (binary or text) from disk as they are, with no conversions, so using the text mode won't make a difference. In contrast, on Windows, files are read in text mode by default; this mode has conversions such as line feeds `\n` converting to `\r\n` sequences on output.

The main difficulty with this project was making it work on a Windows system. I had to read the `fopen` documentation to make it work properly using the binary flag.
Other difficulties that I found were the dynamically allocation of a two dimensional array and the resizing of pixels "algorithm".

As a future addition, it would be nice to allow resizing of other type of BMPs or image types.

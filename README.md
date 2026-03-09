# file_renamer

A command line tool for renaming image and video files in a directory. Files are sorted by date and renamed sequentially to `img_1`, `img_2`, etc. while preserving their original file extension. Subdirectories are processed automatically, with each folder maintaining its own counter.

## Usage

```
file_renamer.exe <directory path>
```

**Example:**
```
file_renamer.exe "E:/Pictures/Unsorted"
```

## Behavior

- Recursively enters subdirectories and processes each one independently
- Only renames recognized image and video files, everything else is left untouched
- Files are sorted by last modified date before numbering
- Uses a two-pass rename (first to temporary names, then to final names) to avoid overwriting files mid-process
- If a temporary filename already exists in the folder, that file is skipped

## Supported formats

**Images:** jpg, jpeg, png, gif, bmp, tiff, tif, webp, heic, heif, raw

**Video:** mp4, mov, avi, mkv, wmv, flv, webm, m4v, mpeg, mpg

## Building

Requires a C++17 compatible compiler. Built with CMake and MinGW on Windows.

```
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build .
```

> **Note for Windows/MinGW builds:** Add the following to `CMakeLists.txt` to avoid missing DLL errors:
> ```cmake
> target_link_options(file_renamer PRIVATE -static)
> ```

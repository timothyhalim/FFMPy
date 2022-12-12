# FFMPEG for Python
FFMPEG compiled for python that can be imported as library

## Building

### Requirement
- Windows x64
- Python
- CMake
- Visual Studio

### Build steps
- Run `make.bat` to build .pyd inside the build/Release folder
- Run `test.py` with same version of python for building to test the compiled pyd

## Usage
After compiling or download the precompiled, you can run it in python by adding it to sys.path

```
import FFMPy                         # Import the library

print(dir(FFMPy))                    # Print avaliable class and method

decoder = FFMPy.Decoder()            # Instantiate the Decoder class
decoder.set_filepath("video.mp4")    # Set file path for Decoder

decoder = FFMPy.Decoder("video.mp4") # Or Directly set the filepath during creation

print(dir(decoder))                  # Print Decoder class method
print(decoder.get_filepath())        # Get current decoder filepath
print(decoder.get_file_info())       # Get file info of current filepath

frame = decoder.extract_frame(2)     # Extract bytes of specified frame
```
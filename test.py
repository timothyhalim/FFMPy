import sys
import os

lib = os.path.join(__file__, "..", "build", "Release")
if os.path.exists(lib):
    lib = os.path.normpath(lib)
    if not lib in sys.path:
        sys.path.append(lib)

import FFMPy

print("FFMPy dir")
print(dir(FFMPy))

print("Decoder Init")
decoder = FFMPy.Decoder()
print(decoder)
print("Decoder set path")
print(decoder.set_filepath("E:/cpp/FFMPy-cmake/video.mp4"))

print("FILL DECODER!")
decoder = FFMPy.Decoder("E:/cpp/FFMPy-cmake/video.mp4")
print(decoder)
print("Decoder dir")
print(dir(decoder))

print("Decoder get path and info")
print(decoder.get_filepath())
print(decoder.get_file_info())

print("EXTRACTING FRAME")
print(decoder.extract_frame(1))

# print(PyExt.example(1, 2))
# print(PyExt.extract_frame(1))

# print(PyExt.extract())
# x = PyExt.Decoder("Test")
# print("Result")
# print(x)
# print(dir(x))
# p = x.set_filepath("Filepath")
# print(p)
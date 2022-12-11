import sys
import os

lib = os.path.join(__file__, "..", "build", "Release")
if os.path.exists(lib):
    lib = os.path.normpath(lib)
    if not lib in sys.path:
        sys.path.append(lib)

import FFMPy

print(dir(FFMPy))
print("EMPTY DECODER!")
decoder = FFMPy.Decoder()
print(decoder)


print("FILL DECODER!")
decoder = FFMPy.Decoder("File")
print(decoder)


print(dir(decoder))

# print(decoder.set_filepath("E:/cpp/FFMPy-cmake/video.mp4"))
# print(decoder.set_filepath(r"F:\TDW\2020-11-13 14-15-16.mp4"))
print(decoder.set_filepath(r"File"))
print(decoder.get_filepath())

import time
start = time.time()
print("Processed in %.02f" %(time.time() - start))
print(decoder.get_file_info())
# print(PyExt.example(1, 2))
# print(PyExt.extract_frame(1))

# print(PyExt.extract())
# x = PyExt.Decoder("Test")
# print("Result")
# print(x)
# print(dir(x))
# p = x.set_filepath("Filepath")
# print(p)
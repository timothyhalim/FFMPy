import sys
import os

lib = os.path.join(__file__, "..", "build", "Release")
if os.path.exists(lib):
    lib = os.path.normpath(lib)
    if not lib in sys.path:
        sys.path.append(lib)

import PyExt

print(dir(PyExt))

print("Testters")

print(PyExt.example(1, 2))
print(PyExt.extract_frame(1))

# print(PyExt.extract())
# x = PyExt.Decoder("Test")
# print("Result")
# print(x)
# print(dir(x))
# p = x.set_filepath("Filepath")
# print(p)
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

print(decoder.set_filepath("File2"))
print("Testters")

print(decoder.get_filepath())
# print(PyExt.example(1, 2))
# print(PyExt.extract_frame(1))

# print(PyExt.extract())
# x = PyExt.Decoder("Test")
# print("Result")
# print(x)
# print(dir(x))
# p = x.set_filepath("Filepath")
# print(p)
import sys
import os

lib = os.path.join(__file__, "..", "..", "build", "Release")
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
print(decoder.set_input_file(os.path.join(__file__, "..", "video.mp4")))

print("FILL DECODER!")
decoder = FFMPy.Decoder(os.path.join(__file__, "..", "video.mp4"))
print(decoder)
print("Decoder dir")
print(dir(decoder))

print("Decoder get path and info")
print(decoder.get_input_file())
print(decoder.get_input_info())

print("EXTRACTING One FRAME")
from time import time
start = time()
frame = decoder.extract_frame(2)
print("Extracted in %.03f sec" %(time()-start))

print("EXTRACTING 25 FRAME")
# This method is better used to extract one frame since it open the file then extract and close the file
from time import time
start = time()
for i in range(25):
    frame = decoder.extract_frame(i)
print("Extracted in %.03f sec" %(time()-start))

print("EXTRACTING 25 FRAME With stream")
# This method is better used to extract multiple frames since it keeps the file open until close_stream is called
from time import time
start = time()
decoder.open_stream()
for i in range(25):
    frame = decoder.extract_frame(i)
decoder.close_stream()
print("Extracted in %.03f sec" %(time()-start))
#!/usr/bin/env python3

RATE = 15625

import struct
import sys
import wave

path_txt = sys.argv[1]
path_wav = path_txt + ".wav"

F = []
with open(path_txt) as t:
    for l in t.readlines():
        if l.strip() == "BEGIN":
            print("begin")
            continue
        if l.strip() == "END":
            print("end")
            break
        for s in l.split():
            F.append(int(s))

with wave.open(path_wav, "wb") as w:
    w.setframerate(RATE)
    w.setnchannels(1)
    w.setsampwidth(2)
    for f in F:
        w.writeframes(struct.pack("<h", f))

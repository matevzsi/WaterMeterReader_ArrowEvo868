# WaterMeterReader_ArrowEvo868
Reception and decoding of water meter reports for Arrow Evo 868 MHz (not universal, cipher to be decoded)

Arrow Evo 868 installed uses a manufacturer-specific 0xA2 data frame instead of standard Wireless M-Bus format and 
the Arduino code is used to decode those.

Each data frame starts with a counter
Data is encoded using a XOR code of an unknown generator
Coding changes two times per year with one of the fields being subtracted each time (starting at approx. 0x16), which may be
related to the battery lifetime (10 years).

Frame [-90 dB] : 19 44 24 34 ?? ?? ?? ?? ?? ?? 82 69 A2 ED 10 00 14 B6 66 17 73 42 78 CA 5A 4D F2 19  T1 ['MAD' ????????????] A2 data: Meter: 348.15 m3 (captured 345.65 m3) date: 16A4
Frame [-89 dB] : 19 44 24 34 ?? ?? ?? ?? ?? ?? 82 69 A2 FD 10 00 14 B6 71 38 3A E9 59 19 83 96 EB 1B  T1 ['MAD' ????????????] A2 data: Meter: 348.15 m3 (captured 345.65 m3) date: 16A4
Frame [-90 dB] : 19 44 24 34 ?? ?? ?? ?? ?? ?? 82 69 A2 8D 10 00 14 B6 14 F4 C6 B8 BC 22 8E 97 A4 16  T1 ['MAD' ????????????] A2 data: Meter: 348.15 m3 (captured 345.65 m3) date: 16A4
Frame [-87 dB] : 19 44 24 34 ?? ?? ?? ?? ?? ?? 82 69 A2 9D 10 00 14 B6 03 DB 8F 13 9D F1 57 4C BD 14  T1 ['MAD' ????????????] A2 data: Meter: 348.15 m3 (captured 345.65 m3) date: 16A4

# WaterMeterReader_ArrowEvo868
Reception and decoding of water meter reports for Arrow Evo 868 MHz (not universal, cipher to be decoded)

Arrow Evo 868 installed uses a manufacturer-specific 0xA2 data frame instead of standard Wireless M-Bus format and 
the Arduino code is used to decode those.

Each data frame starts with a counter, which counts in steps of 0x10 from 0x8D to 0xFD. All packets contain the same payload, but encoded with a different XOR code. XOR cipher/code
is defined by the counter value.

Coding changes two times per year with one of the fields being subtracted each time (starting at approx. 0x16), which may be related to the battery lifetime (10 years).

'''
2022-12-11 13:39:32.525703 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 AD 10 00 14 B6 C6 A5 55 EE FF 85 3D 21 96 13  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:39:43.871923 Frame [-90 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 BD 10 00 14 B6 D1 8A 1C 45 DE 56 E4 FA 8F 11  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:39:55.076816 Frame [-86 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 CD 10 00 14 B6 B4 46 E0 14 3B 6D E9 FB C0 1C  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:40:06.327929 Frame [-90 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 DD 10 00 14 B6 A3 69 A9 BF 1A BE 30 20 D9 1E  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:40:18.019178 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 ED 10 00 14 B6 9A 18 73 42 78 CA 5A 4D F2 19  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:40:41.616442 Frame [-86 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 8D 10 00 14 B6 E8 FB C6 B8 BC 22 8E 97 A4 16  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:40:53.526142 Frame [-86 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 9D 10 00 14 B6 FF D4 8F 13 9D F1 57 4C BD 14  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:41:04.851851 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 AD 10 00 14 B6 C6 A5 55 EE FF 85 3D 21 96 13  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:41:16.195576 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 BD 10 00 14 B6 D1 8A 1C 45 DE 56 E4 FA 8F 11  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:41:27.402847 Frame [-85 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 CD 10 00 14 B6 B4 46 E0 14 3B 6D E9 FB C0 1C  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:41:38.648488 Frame [-89 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 DD 10 00 14 B6 A3 69 A9 BF 1A BE 30 20 D9 1E  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:41:50.340285 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 ED 10 00 14 B6 9A 18 73 42 78 CA 5A 4D F2 19  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:42:02.092356 Frame [-89 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 FD 10 00 14 B6 8D 37 3A E9 59 19 83 96 EB 1B  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:42:13.928297 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 8D 10 00 14 B6 E8 FB C6 B8 BC 22 8E 97 A4 16  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
2022-12-11 13:42:25.844400 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 9D 10 00 14 B6 FF D4 8F 13 9D F1 57 4C BD 14  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
'''

We can take a look at the payload section of the frame and observe the XOR between two consecutive values - some sort of a pattern appears.
'''
2022-12-11 13:40:41.616442 Frame [-86 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 8D 10 00 14 B6 E8 FB C6 B8 BC 22 8E 97 A4 16  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  17 2F 49 AB 21 D3 D9 DB 19 02   (XOR of data)
2022-12-11 13:40:53.526142 Frame [-86 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 9D 10 00 14 B6 FF D4 8F 13 9D F1 57 4C BD 14  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  39 71 DA FD 62 74 6A 6D 2B 07   (XOR of data)
2022-12-11 13:41:04.851851 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 AD 10 00 14 B6 C6 A5 55 EE FF 85 3D 21 96 13  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  17 2F 49 AB 21 D3 D9 DB 19 02   (XOR of data)
2022-12-11 13:41:16.195576 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 BD 10 00 14 B6 D1 8A 1C 45 DE 56 E4 FA 8F 11  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  65 CC FC 51 E5 3B 0D 01 4F 0D   (XOR of data)
2022-12-11 13:41:27.402847 Frame [-85 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 CD 10 00 14 B6 B4 46 E0 14 3B 6D E9 FB C0 1C  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  17 2F 49 AB 21 D3 D9 DB 19 02   (XOR of data)
2022-12-11 13:41:38.648488 Frame [-89 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 DD 10 00 14 B6 A3 69 A9 BF 1A BE 30 20 D9 1E  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  39 71 DA FD 62 74 6A 6D 2B 07   (XOR of data)
2022-12-11 13:41:50.340285 Frame [-88 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 ED 10 00 14 B6 9A 18 73 42 78 CA 5A 4D F2 19  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  17 2F 49 AB 21 D3 D9 DB 19 02   (XOR of data)
2022-12-11 13:42:02.092356 Frame [-89 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 FD 10 00 14 B6 8D 37 3A E9 59 19 83 96 EB 1B  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                  65 CC FC 51 E5 3B 0D 01 4F 0D   (XOR of data)
2022-12-11 13:42:13.928297 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 8D 10 00 14 B6 E8 FB C6 B8 BC 22 8E 97 A4 16  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
                                                                                                **17 2F 49 AB 21 D3 D9 DB 19 02**   (XOR of data)
2022-12-11 13:42:25.844400 Frame [-87 dB] : 19 44 24 34 ?? ?? 13 05 84 17 82 69 A2 9D 10 00 14 B6 FF D4 8F 13 9D F1 57 4C BD 14  T1 ['MAD' ????13058417] A2 data: Meter: 348.19 m3 (captured 345.65 m3) date: 16A4
'''
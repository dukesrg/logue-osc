/*
 * File: waveforms16.h
 *
 * DX7/DX21/DX11-series 16-bit PCM waveforms
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#define FORMAT_PCM16
#define SAMPLE_COUNT 256
#define WAVE_COUNT 8
#define WAVE_COUNT_X 8
#define WAVE_COUNT_Y 1
#define WAVEBANK \
0x0000, 0x0324, 0x0648, 0x096B, 0x0C8C, 0x0FAB, 0x12C8, 0x15E2,\
0x18F9, 0x1C0C, 0x1F1A, 0x2224, 0x2528, 0x2827, 0x2B1F, 0x2E11,\
0x30FC, 0x33DF, 0x36BA, 0x398D, 0x3C57, 0x3F17, 0x41CE, 0x447B,\
0x471D, 0x49B4, 0x4C40, 0x4EC0, 0x5134, 0x539B, 0x55F6, 0x5843,\
0x5A82, 0x5CB4, 0x5ED7, 0x60EC, 0x62F2, 0x64E9, 0x66D0, 0x68A7,\
0x6A6E, 0x6C24, 0x6DCA, 0x6F5F, 0x70E3, 0x7255, 0x73B6, 0x7505,\
0x7642, 0x776C, 0x7885, 0x798A, 0x7A7D, 0x7B5D, 0x7C2A, 0x7CE4,\
0x7D8A, 0x7E1E, 0x7E9D, 0x7F0A, 0x7F62, 0x7FA7, 0x7FD9, 0x7FF6,\
0x7FFF, 0x7FF6, 0x7FD9, 0x7FA7, 0x7F62, 0x7F0A, 0x7E9D, 0x7E1E,\
0x7D8A, 0x7CE4, 0x7C2A, 0x7B5D, 0x7A7D, 0x798A, 0x7885, 0x776C,\
0x7642, 0x7505, 0x73B6, 0x7255, 0x70E3, 0x6F5F, 0x6DCA, 0x6C24,\
0x6A6E, 0x68A7, 0x66D0, 0x64E9, 0x62F2, 0x60EC, 0x5ED7, 0x5CB4,\
0x5A82, 0x5843, 0x55F6, 0x539B, 0x5134, 0x4EC0, 0x4C40, 0x49B4,\
0x471D, 0x447B, 0x41CE, 0x3F17, 0x3C57, 0x398D, 0x36BA, 0x33DF,\
0x30FC, 0x2E11, 0x2B1F, 0x2827, 0x2528, 0x2224, 0x1F1A, 0x1C0C,\
0x18F9, 0x15E2, 0x12C8, 0x0FAB, 0x0C8C, 0x096B, 0x0648, 0x0324,\
0x0000, 0xFCDC, 0xF9B8, 0xF695, 0xF374, 0xF055, 0xED38, 0xEA1E,\
0xE707, 0xE3F4, 0xE0E6, 0xDDDC, 0xDAD8, 0xD7D9, 0xD4E1, 0xD1EF,\
0xCF04, 0xCC21, 0xC946, 0xC673, 0xC3A9, 0xC0E9, 0xBE32, 0xBB85,\
0xB8E3, 0xB64C, 0xB3C0, 0xB140, 0xAECC, 0xAC65, 0xAA0A, 0xA7BD,\
0xA57E, 0xA34C, 0xA129, 0x9F14, 0x9D0E, 0x9B17, 0x9930, 0x9759,\
0x9592, 0x93DC, 0x9236, 0x90A1, 0x8F1D, 0x8DAB, 0x8C4A, 0x8AFB,\
0x89BE, 0x8894, 0x877B, 0x8676, 0x8583, 0x84A3, 0x83D6, 0x831C,\
0x8276, 0x81E2, 0x8163, 0x80F6, 0x809E, 0x8059, 0x8027, 0x800A,\
0x8000, 0x800A, 0x8027, 0x8059, 0x809E, 0x80F6, 0x8163, 0x81E2,\
0x8276, 0x831C, 0x83D6, 0x84A3, 0x8583, 0x8676, 0x877B, 0x8894,\
0x89BE, 0x8AFB, 0x8C4A, 0x8DAB, 0x8F1D, 0x90A1, 0x9236, 0x93DC,\
0x9592, 0x9759, 0x9930, 0x9B17, 0x9D0E, 0x9F14, 0xA129, 0xA34C,\
0xA57E, 0xA7BD, 0xAA0A, 0xAC65, 0xAECC, 0xB140, 0xB3C0, 0xB64C,\
0xB8E3, 0xBB85, 0xBE32, 0xC0E9, 0xC3A9, 0xC673, 0xC946, 0xCC21,\
0xCF04, 0xD1EF, 0xD4E1, 0xD7D9, 0xDAD8, 0xDDDC, 0xE0E6, 0xE3F4,\
0xE707, 0xEA1E, 0xED38, 0xF055, 0xF374, 0xF695, 0xF9B8, 0xFCDC,\
0x0000, 0x0014, 0x004F, 0x00B1, 0x013B, 0x01EB, 0x02C1, 0x03BE,\
0x04DF, 0x0625, 0x078F, 0x091B, 0x0AC9, 0x0C98, 0x0E87, 0x1094,\
0x12BF, 0x1505, 0x1766, 0x19E0, 0x1C72, 0x1F19, 0x21D5, 0x24A3,\
0x2782, 0x2A70, 0x2D6C, 0x3073, 0x3384, 0x369C, 0x39BA, 0x3CDC,\
0x4000, 0x4324, 0x4646, 0x4964, 0x4C7C, 0x4F8D, 0x5294, 0x5590,\
0x587E, 0x5B5D, 0x5E2B, 0x60E7, 0x638E, 0x6620, 0x689A, 0x6AFB,\
0x6D41, 0x6F6C, 0x7179, 0x7368, 0x7537, 0x76E5, 0x7871, 0x79DB,\
0x7B21, 0x7C42, 0x7D3F, 0x7E15, 0x7EC5, 0x7F4F, 0x7FB1, 0x7FEC,\
0x7FFF, 0x7FEC, 0x7FB1, 0x7F4F, 0x7EC5, 0x7E15, 0x7D3F, 0x7C42,\
0x7B21, 0x79DB, 0x7871, 0x76E5, 0x7537, 0x7368, 0x7179, 0x6F6C,\
0x6D41, 0x6AFB, 0x689A, 0x6620, 0x638E, 0x60E7, 0x5E2B, 0x5B5D,\
0x587E, 0x5590, 0x5294, 0x4F8D, 0x4C7C, 0x4964, 0x4646, 0x4324,\
0x4000, 0x3CDC, 0x39BA, 0x369C, 0x3384, 0x3073, 0x2D6C, 0x2A70,\
0x2782, 0x24A3, 0x21D5, 0x1F19, 0x1C72, 0x19E0, 0x1766, 0x1505,\
0x12BF, 0x1094, 0x0E87, 0x0C98, 0x0AC9, 0x091B, 0x078F, 0x0625,\
0x04DF, 0x03BE, 0x02C1, 0x01EB, 0x013B, 0x00B1, 0x004F, 0x0014,\
0x0000, 0xFFEC, 0xFFB1, 0xFF4F, 0xFEC5, 0xFE15, 0xFD3F, 0xFC42,\
0xFB21, 0xF9DB, 0xF871, 0xF6E5, 0xF537, 0xF368, 0xF179, 0xEF6C,\
0xED41, 0xEAFB, 0xE89A, 0xE620, 0xE38E, 0xE0E7, 0xDE2B, 0xDB5D,\
0xD87E, 0xD590, 0xD294, 0xCF8D, 0xCC7C, 0xC964, 0xC646, 0xC324,\
0xC000, 0xBCDC, 0xB9BA, 0xB69C, 0xB384, 0xB073, 0xAD6C, 0xAA70,\
0xA782, 0xA4A3, 0xA1D5, 0x9F19, 0x9C72, 0x99E0, 0x9766, 0x9505,\
0x92BF, 0x9094, 0x8E87, 0x8C98, 0x8AC9, 0x891B, 0x878F, 0x8625,\
0x84DF, 0x83BE, 0x82C1, 0x81EB, 0x813B, 0x80B1, 0x804F, 0x8014,\
0x8000, 0x8014, 0x804F, 0x80B1, 0x813B, 0x81EB, 0x82C1, 0x83BE,\
0x84DF, 0x8625, 0x878F, 0x891B, 0x8AC9, 0x8C98, 0x8E87, 0x9094,\
0x92BF, 0x9505, 0x9766, 0x99E0, 0x9C72, 0x9F19, 0xA1D5, 0xA4A3,\
0xA782, 0xAA70, 0xAD6C, 0xB073, 0xB384, 0xB69C, 0xB9BA, 0xBCDC,\
0xC000, 0xC324, 0xC646, 0xC964, 0xCC7C, 0xCF8D, 0xD294, 0xD590,\
0xD87E, 0xDB5D, 0xDE2B, 0xE0E7, 0xE38E, 0xE620, 0xE89A, 0xEAFB,\
0xED41, 0xEF6C, 0xF179, 0xF368, 0xF537, 0xF6E5, 0xF871, 0xF9DB,\
0xFB21, 0xFC42, 0xFD3F, 0xFE15, 0xFEC5, 0xFF4F, 0xFFB1, 0xFFEC,\
0x0000, 0x0324, 0x0648, 0x096B, 0x0C8C, 0x0FAB, 0x12C8, 0x15E2,\
0x18F9, 0x1C0C, 0x1F1A, 0x2224, 0x2528, 0x2827, 0x2B1F, 0x2E11,\
0x30FC, 0x33DF, 0x36BA, 0x398D, 0x3C57, 0x3F17, 0x41CE, 0x447B,\
0x471D, 0x49B4, 0x4C40, 0x4EC0, 0x5134, 0x539B, 0x55F6, 0x5843,\
0x5A82, 0x5CB4, 0x5ED7, 0x60EC, 0x62F2, 0x64E9, 0x66D0, 0x68A7,\
0x6A6E, 0x6C24, 0x6DCA, 0x6F5F, 0x70E3, 0x7255, 0x73B6, 0x7505,\
0x7642, 0x776C, 0x7885, 0x798A, 0x7A7D, 0x7B5D, 0x7C2A, 0x7CE4,\
0x7D8A, 0x7E1E, 0x7E9D, 0x7F0A, 0x7F62, 0x7FA7, 0x7FD9, 0x7FF6,\
0x7FFF, 0x7FF6, 0x7FD9, 0x7FA7, 0x7F62, 0x7F0A, 0x7E9D, 0x7E1E,\
0x7D8A, 0x7CE4, 0x7C2A, 0x7B5D, 0x7A7D, 0x798A, 0x7885, 0x776C,\
0x7642, 0x7505, 0x73B6, 0x7255, 0x70E3, 0x6F5F, 0x6DCA, 0x6C24,\
0x6A6E, 0x68A7, 0x66D0, 0x64E9, 0x62F2, 0x60EC, 0x5ED7, 0x5CB4,\
0x5A82, 0x5843, 0x55F6, 0x539B, 0x5134, 0x4EC0, 0x4C40, 0x49B4,\
0x471D, 0x447B, 0x41CE, 0x3F17, 0x3C57, 0x398D, 0x36BA, 0x33DF,\
0x30FC, 0x2E11, 0x2B1F, 0x2827, 0x2528, 0x2224, 0x1F1A, 0x1C0C,\
0x18F9, 0x15E2, 0x12C8, 0x0FAB, 0x0C8C, 0x096B, 0x0648, 0x0324,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0014, 0x004F, 0x00B1, 0x013B, 0x01EB, 0x02C1, 0x03BE,\
0x04DF, 0x0625, 0x078F, 0x091B, 0x0AC9, 0x0C98, 0x0E87, 0x1094,\
0x12BF, 0x1505, 0x1766, 0x19E0, 0x1C72, 0x1F19, 0x21D5, 0x24A3,\
0x2782, 0x2A70, 0x2D6C, 0x3073, 0x3384, 0x369C, 0x39BA, 0x3CDC,\
0x4000, 0x4324, 0x4646, 0x4964, 0x4C7C, 0x4F8D, 0x5294, 0x5590,\
0x587E, 0x5B5D, 0x5E2B, 0x60E7, 0x638E, 0x6620, 0x689A, 0x6AFB,\
0x6D41, 0x6F6C, 0x7179, 0x7368, 0x7537, 0x76E5, 0x7871, 0x79DB,\
0x7B21, 0x7C42, 0x7D3F, 0x7E15, 0x7EC5, 0x7F4F, 0x7FB1, 0x7FEC,\
0x7FFF, 0x7FEC, 0x7FB1, 0x7F4F, 0x7EC5, 0x7E15, 0x7D3F, 0x7C42,\
0x7B21, 0x79DB, 0x7871, 0x76E5, 0x7537, 0x7368, 0x7179, 0x6F6C,\
0x6D41, 0x6AFB, 0x689A, 0x6620, 0x638E, 0x60E7, 0x5E2B, 0x5B5D,\
0x587E, 0x5590, 0x5294, 0x4F8D, 0x4C7C, 0x4964, 0x4646, 0x4324,\
0x4000, 0x3CDC, 0x39BA, 0x369C, 0x3384, 0x3073, 0x2D6C, 0x2A70,\
0x2782, 0x24A3, 0x21D5, 0x1F19, 0x1C72, 0x19E0, 0x1766, 0x1505,\
0x12BF, 0x1094, 0x0E87, 0x0C98, 0x0AC9, 0x091B, 0x078F, 0x0625,\
0x04DF, 0x03BE, 0x02C1, 0x01EB, 0x013B, 0x00B1, 0x004F, 0x0014,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0648, 0x0C8C, 0x12C8, 0x18F9, 0x1F1A, 0x2528, 0x2B1F,\
0x30FC, 0x36BA, 0x3C57, 0x41CE, 0x471D, 0x4C40, 0x5134, 0x55F6,\
0x5A82, 0x5ED7, 0x62F2, 0x66D0, 0x6A6E, 0x6DCA, 0x70E3, 0x73B6,\
0x7642, 0x7885, 0x7A7D, 0x7C2A, 0x7D8A, 0x7E9D, 0x7F62, 0x7FD9,\
0x7FFF, 0x7FD9, 0x7F62, 0x7E9D, 0x7D8A, 0x7C2A, 0x7A7D, 0x7885,\
0x7642, 0x73B6, 0x70E3, 0x6DCA, 0x6A6E, 0x66D0, 0x62F2, 0x5ED7,\
0x5A82, 0x55F6, 0x5134, 0x4C40, 0x471D, 0x41CE, 0x3C57, 0x36BA,\
0x30FC, 0x2B1F, 0x2528, 0x1F1A, 0x18F9, 0x12C8, 0x0C8C, 0x0648,\
0x0000, 0xF9B8, 0xF374, 0xED38, 0xE707, 0xE0E6, 0xDAD8, 0xD4E1,\
0xCF04, 0xC946, 0xC3A9, 0xBE32, 0xB8E3, 0xB3C0, 0xAECC, 0xAA0A,\
0xA57E, 0xA129, 0x9D0E, 0x9930, 0x9592, 0x9236, 0x8F1D, 0x8C4A,\
0x89BE, 0x877B, 0x8583, 0x83D6, 0x8276, 0x8163, 0x809E, 0x8027,\
0x8000, 0x8027, 0x809E, 0x8163, 0x8276, 0x83D6, 0x8583, 0x877B,\
0x89BE, 0x8C4A, 0x8F1D, 0x9236, 0x9592, 0x9930, 0x9D0E, 0xA129,\
0xA57E, 0xAA0A, 0xAECC, 0xB3C0, 0xB8E3, 0xBE32, 0xC3A9, 0xC946,\
0xCF04, 0xD4E1, 0xDAD8, 0xE0E6, 0xE707, 0xED38, 0xF374, 0xF9B8,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x004F, 0x013B, 0x02C1, 0x04DF, 0x078F, 0x0AC9, 0x0E87,\
0x12BF, 0x1766, 0x1C72, 0x21D5, 0x2782, 0x2D6C, 0x3384, 0x39BA,\
0x4000, 0x4646, 0x4C7C, 0x5294, 0x587E, 0x5E2B, 0x638E, 0x689A,\
0x6D41, 0x7179, 0x7537, 0x7871, 0x7B21, 0x7D3F, 0x7EC5, 0x7FB1,\
0x7FFF, 0x7FB1, 0x7EC5, 0x7D3F, 0x7B21, 0x7871, 0x7537, 0x7179,\
0x6D41, 0x689A, 0x638E, 0x5E2B, 0x587E, 0x5294, 0x4C7C, 0x4646,\
0x4000, 0x39BA, 0x3384, 0x2D6C, 0x2782, 0x21D5, 0x1C72, 0x1766,\
0x12BF, 0x0E87, 0x0AC9, 0x078F, 0x04DF, 0x02C1, 0x013B, 0x004F,\
0x0000, 0xFFB1, 0xFEC5, 0xFD3F, 0xFB21, 0xF871, 0xF537, 0xF179,\
0xED41, 0xE89A, 0xE38E, 0xDE2B, 0xD87E, 0xD294, 0xCC7C, 0xC646,\
0xC000, 0xB9BA, 0xB384, 0xAD6C, 0xA782, 0xA1D5, 0x9C72, 0x9766,\
0x92BF, 0x8E87, 0x8AC9, 0x878F, 0x84DF, 0x82C1, 0x813B, 0x804F,\
0x8000, 0x804F, 0x813B, 0x82C1, 0x84DF, 0x878F, 0x8AC9, 0x8E87,\
0x92BF, 0x9766, 0x9C72, 0xA1D5, 0xA782, 0xAD6C, 0xB384, 0xB9BA,\
0xC000, 0xC646, 0xCC7C, 0xD294, 0xD87E, 0xDE2B, 0xE38E, 0xE89A,\
0xED41, 0xF179, 0xF537, 0xF871, 0xFB21, 0xFD3F, 0xFEC5, 0xFFB1,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0648, 0x0C8C, 0x12C8, 0x18F9, 0x1F1A, 0x2528, 0x2B1F,\
0x30FC, 0x36BA, 0x3C57, 0x41CE, 0x471D, 0x4C40, 0x5134, 0x55F6,\
0x5A82, 0x5ED7, 0x62F2, 0x66D0, 0x6A6E, 0x6DCA, 0x70E3, 0x73B6,\
0x7642, 0x7885, 0x7A7D, 0x7C2A, 0x7D8A, 0x7E9D, 0x7F62, 0x7FD9,\
0x7FFF, 0x7FD9, 0x7F62, 0x7E9D, 0x7D8A, 0x7C2A, 0x7A7D, 0x7885,\
0x7642, 0x73B6, 0x70E3, 0x6DCA, 0x6A6E, 0x66D0, 0x62F2, 0x5ED7,\
0x5A82, 0x55F6, 0x5134, 0x4C40, 0x471D, 0x41CE, 0x3C57, 0x36BA,\
0x30FC, 0x2B1F, 0x2528, 0x1F1A, 0x18F9, 0x12C8, 0x0C8C, 0x0648,\
0x0000, 0x0648, 0x0C8C, 0x12C8, 0x18F9, 0x1F1A, 0x2528, 0x2B1F,\
0x30FC, 0x36BA, 0x3C57, 0x41CE, 0x471D, 0x4C40, 0x5134, 0x55F6,\
0x5A82, 0x5ED7, 0x62F2, 0x66D0, 0x6A6E, 0x6DCA, 0x70E3, 0x73B6,\
0x7642, 0x7885, 0x7A7D, 0x7C2A, 0x7D8A, 0x7E9D, 0x7F62, 0x7FD9,\
0x7FFF, 0x7FD9, 0x7F62, 0x7E9D, 0x7D8A, 0x7C2A, 0x7A7D, 0x7885,\
0x7642, 0x73B6, 0x70E3, 0x6DCA, 0x6A6E, 0x66D0, 0x62F2, 0x5ED7,\
0x5A82, 0x55F6, 0x5134, 0x4C40, 0x471D, 0x41CE, 0x3C57, 0x36BA,\
0x30FC, 0x2B1F, 0x2528, 0x1F1A, 0x18F9, 0x12C8, 0x0C8C, 0x0648,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x004F, 0x013B, 0x02C1, 0x04DF, 0x078F, 0x0AC9, 0x0E87,\
0x12BF, 0x1766, 0x1C72, 0x21D5, 0x2782, 0x2D6C, 0x3384, 0x39BA,\
0x4000, 0x4646, 0x4C7C, 0x5294, 0x587E, 0x5E2B, 0x638E, 0x689A,\
0x6D41, 0x7179, 0x7537, 0x7871, 0x7B21, 0x7D3F, 0x7EC5, 0x7FB1,\
0x7FFF, 0x7FB1, 0x7EC5, 0x7D3F, 0x7B21, 0x7871, 0x7537, 0x7179,\
0x6D41, 0x689A, 0x638E, 0x5E2B, 0x587E, 0x5294, 0x4C7C, 0x4646,\
0x4000, 0x39BA, 0x3384, 0x2D6C, 0x2782, 0x21D5, 0x1C72, 0x1766,\
0x12BF, 0x0E87, 0x0AC9, 0x078F, 0x04DF, 0x02C1, 0x013B, 0x004F,\
0x0000, 0x004F, 0x013B, 0x02C1, 0x04DF, 0x078F, 0x0AC9, 0x0E87,\
0x12BF, 0x1766, 0x1C72, 0x21D5, 0x2782, 0x2D6C, 0x3384, 0x39BA,\
0x4000, 0x4646, 0x4C7C, 0x5294, 0x587E, 0x5E2B, 0x638E, 0x689A,\
0x6D41, 0x7179, 0x7537, 0x7871, 0x7B21, 0x7D3F, 0x7EC5, 0x7FB1,\
0x7FFF, 0x7FB1, 0x7EC5, 0x7D3F, 0x7B21, 0x7871, 0x7537, 0x7179,\
0x6D41, 0x689A, 0x638E, 0x5E2B, 0x587E, 0x5294, 0x4C7C, 0x4646,\
0x4000, 0x39BA, 0x3384, 0x2D6C, 0x2782, 0x21D5, 0x1C72, 0x1766,\
0x12BF, 0x0E87, 0x0AC9, 0x078F, 0x04DF, 0x02C1, 0x013B, 0x004F,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

#include "wavebank.h"

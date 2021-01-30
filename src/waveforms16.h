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
0x0000, 0x000A, 0x0027, 0x0059, 0x009E, 0x00F6, 0x0163, 0x01E2,\
0x0276, 0x031C, 0x03D6, 0x04A3, 0x0583, 0x0676, 0x077B, 0x0894,\
0x09BE, 0x0AFB, 0x0C4A, 0x0DAB, 0x0F1D, 0x10A1, 0x1236, 0x13DC,\
0x1592, 0x1759, 0x1930, 0x1B17, 0x1D0E, 0x1F14, 0x2129, 0x234C,\
0x257E, 0x27BD, 0x2A0A, 0x2C65, 0x2ECC, 0x3140, 0x33C0, 0x364C,\
0x38E3, 0x3B85, 0x3E32, 0x40E9, 0x43A9, 0x4673, 0x4946, 0x4C21,\
0x4F04, 0x51EF, 0x54E1, 0x57D9, 0x5AD8, 0x5DDC, 0x60E6, 0x63F4,\
0x6707, 0x6A1E, 0x6D38, 0x7055, 0x7374, 0x7695, 0x79B8, 0x7CDC,\
0x7FFF, 0x7CDC, 0x79B8, 0x7695, 0x7374, 0x7055, 0x6D38, 0x6A1E,\
0x6707, 0x63F4, 0x60E6, 0x5DDC, 0x5AD8, 0x57D9, 0x54E1, 0x51EF,\
0x4F04, 0x4C21, 0x4946, 0x4673, 0x43A9, 0x40E9, 0x3E32, 0x3B85,\
0x38E3, 0x364C, 0x33C0, 0x3140, 0x2ECC, 0x2C65, 0x2A0A, 0x27BD,\
0x257E, 0x234C, 0x2129, 0x1F14, 0x1D0E, 0x1B17, 0x1930, 0x1759,\
0x1592, 0x13DC, 0x1236, 0x10A1, 0x0F1D, 0x0DAB, 0x0C4A, 0x0AFB,\
0x09BE, 0x0894, 0x077B, 0x0676, 0x0583, 0x04A3, 0x03D6, 0x031C,\
0x0276, 0x01E2, 0x0163, 0x00F6, 0x009E, 0x0059, 0x0027, 0x000A,\
0x0000, 0xFFF6, 0xFFD9, 0xFFA7, 0xFF62, 0xFF0A, 0xFE9D, 0xFE1E,\
0xFD8A, 0xFCE4, 0xFC2A, 0xFB5D, 0xFA7D, 0xF98A, 0xF885, 0xF76C,\
0xF642, 0xF505, 0xF3B6, 0xF255, 0xF0E3, 0xEF5F, 0xEDCA, 0xEC24,\
0xEA6E, 0xE8A7, 0xE6D0, 0xE4E9, 0xE2F2, 0xE0EC, 0xDED7, 0xDCB4,\
0xDA82, 0xD843, 0xD5F6, 0xD39B, 0xD134, 0xCEC0, 0xCC40, 0xC9B4,\
0xC71D, 0xC47B, 0xC1CE, 0xBF17, 0xBC57, 0xB98D, 0xB6BA, 0xB3DF,\
0xB0FC, 0xAE11, 0xAB1F, 0xA827, 0xA528, 0xA224, 0x9F1A, 0x9C0C,\
0x98F9, 0x95E2, 0x92C8, 0x8FAB, 0x8C8C, 0x896B, 0x8648, 0x8324,\
0x8000, 0x8324, 0x8648, 0x896B, 0x8C8C, 0x8FAB, 0x92C8, 0x95E2,\
0x98F9, 0x9C0C, 0x9F1A, 0xA224, 0xA528, 0xA827, 0xAB1F, 0xAE11,\
0xB0FC, 0xB3DF, 0xB6BA, 0xB98D, 0xBC57, 0xBF17, 0xC1CE, 0xC47B,\
0xC71D, 0xC9B4, 0xCC40, 0xCEC0, 0xD134, 0xD39B, 0xD5F6, 0xD843,\
0xDA82, 0xDCB4, 0xDED7, 0xE0EC, 0xE2F2, 0xE4E9, 0xE6D0, 0xE8A7,\
0xEA6E, 0xEC24, 0xEDCA, 0xEF5F, 0xF0E3, 0xF255, 0xF3B6, 0xF505,\
0xF642, 0xF76C, 0xF885, 0xF98A, 0xFA7D, 0xFB5D, 0xFC2A, 0xFCE4,\
0xFD8A, 0xFE1E, 0xFE9D, 0xFF0A, 0xFF62, 0xFFA7, 0xFFD9, 0xFFF6,\
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
0x0000, 0x000A, 0x0027, 0x0059, 0x009E, 0x00F6, 0x0163, 0x01E2,\
0x0276, 0x031C, 0x03D6, 0x04A3, 0x0583, 0x0676, 0x077B, 0x0894,\
0x09BE, 0x0AFB, 0x0C4A, 0x0DAB, 0x0F1D, 0x10A1, 0x1236, 0x13DC,\
0x1592, 0x1759, 0x1930, 0x1B17, 0x1D0E, 0x1F14, 0x2129, 0x234C,\
0x257E, 0x27BD, 0x2A0A, 0x2C65, 0x2ECC, 0x3140, 0x33C0, 0x364C,\
0x38E3, 0x3B85, 0x3E32, 0x40E9, 0x43A9, 0x4673, 0x4946, 0x4C21,\
0x4F04, 0x51EF, 0x54E1, 0x57D9, 0x5AD8, 0x5DDC, 0x60E6, 0x63F4,\
0x6707, 0x6A1E, 0x6D38, 0x7055, 0x7374, 0x7695, 0x79B8, 0x7CDC,\
0x7FFF, 0x7CDC, 0x79B8, 0x7695, 0x7374, 0x7055, 0x6D38, 0x6A1E,\
0x6707, 0x63F4, 0x60E6, 0x5DDC, 0x5AD8, 0x57D9, 0x54E1, 0x51EF,\
0x4F04, 0x4C21, 0x4946, 0x4673, 0x43A9, 0x40E9, 0x3E32, 0x3B85,\
0x38E3, 0x364C, 0x33C0, 0x3140, 0x2ECC, 0x2C65, 0x2A0A, 0x27BD,\
0x257E, 0x234C, 0x2129, 0x1F14, 0x1D0E, 0x1B17, 0x1930, 0x1759,\
0x1592, 0x13DC, 0x1236, 0x10A1, 0x0F1D, 0x0DAB, 0x0C4A, 0x0AFB,\
0x09BE, 0x0894, 0x077B, 0x0676, 0x0583, 0x04A3, 0x03D6, 0x031C,\
0x0276, 0x01E2, 0x0163, 0x00F6, 0x009E, 0x0059, 0x0027, 0x000A,\
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
0x0000, 0x0027, 0x009E, 0x0163, 0x0276, 0x03D6, 0x0583, 0x077B,\
0x09BE, 0x0C4A, 0x0F1D, 0x1236, 0x1592, 0x1930, 0x1D0E, 0x2129,\
0x257E, 0x2A0A, 0x2ECC, 0x33C0, 0x38E3, 0x3E32, 0x43A9, 0x4946,\
0x4F04, 0x54E1, 0x5AD8, 0x60E6, 0x6707, 0x6D38, 0x7374, 0x79B8,\
0x7FFF, 0x79B8, 0x7374, 0x6D38, 0x6707, 0x60E6, 0x5AD8, 0x54E1,\
0x4F04, 0x4946, 0x43A9, 0x3E32, 0x38E3, 0x33C0, 0x2ECC, 0x2A0A,\
0x257E, 0x2129, 0x1D0E, 0x1930, 0x1592, 0x1236, 0x0F1D, 0x0C4A,\
0x09BE, 0x077B, 0x0583, 0x03D6, 0x0276, 0x0163, 0x009E, 0x0027,\
0x0000, 0xFFD9, 0xFF62, 0xFE9D, 0xFD8A, 0xFC2A, 0xFA7D, 0xF885,\
0xF642, 0xF3B6, 0xF0E3, 0xEDCA, 0xEA6E, 0xE6D0, 0xE2F2, 0xDED7,\
0xDA82, 0xD5F6, 0xD134, 0xCC40, 0xC71D, 0xC1CE, 0xBC57, 0xB6BA,\
0xB0FC, 0xAB1F, 0xA528, 0x9F1A, 0x98F9, 0x92C8, 0x8C8C, 0x8648,\
0x8000, 0x8648, 0x8C8C, 0x92C8, 0x98F9, 0x9F1A, 0xA528, 0xAB1F,\
0xB0FC, 0xB6BA, 0xBC57, 0xC1CE, 0xC71D, 0xCC40, 0xD134, 0xD5F6,\
0xDA82, 0xDED7, 0xE2F2, 0xE6D0, 0xEA6E, 0xEDCA, 0xF0E3, 0xF3B6,\
0xF642, 0xF885, 0xFA7D, 0xFC2A, 0xFD8A, 0xFE9D, 0xFF62, 0xFFD9,\
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
0x0000, 0x0027, 0x009E, 0x0163, 0x0276, 0x03D6, 0x0583, 0x077B,\
0x09BE, 0x0C4A, 0x0F1D, 0x1236, 0x1592, 0x1930, 0x1D0E, 0x2129,\
0x257E, 0x2A0A, 0x2ECC, 0x33C0, 0x38E3, 0x3E32, 0x43A9, 0x4946,\
0x4F04, 0x54E1, 0x5AD8, 0x60E6, 0x6707, 0x6D38, 0x7374, 0x79B8,\
0x7FFF, 0x79B8, 0x7374, 0x6D38, 0x6707, 0x60E6, 0x5AD8, 0x54E1,\
0x4F04, 0x4946, 0x43A9, 0x3E32, 0x38E3, 0x33C0, 0x2ECC, 0x2A0A,\
0x257E, 0x2129, 0x1D0E, 0x1930, 0x1592, 0x1236, 0x0F1D, 0x0C4A,\
0x09BE, 0x077B, 0x0583, 0x03D6, 0x0276, 0x0163, 0x009E, 0x0027,\
0x0000, 0x0027, 0x009E, 0x0163, 0x0276, 0x03D6, 0x0583, 0x077B,\
0x09BE, 0x0C4A, 0x0F1D, 0x1236, 0x1592, 0x1930, 0x1D0E, 0x2129,\
0x257E, 0x2A0A, 0x2ECC, 0x33C0, 0x38E3, 0x3E32, 0x43A9, 0x4946,\
0x4F04, 0x54E1, 0x5AD8, 0x60E6, 0x6707, 0x6D38, 0x7374, 0x79B8,\
0x7FFF, 0x79B8, 0x7374, 0x6D38, 0x6707, 0x60E6, 0x5AD8, 0x54E1,\
0x4F04, 0x4946, 0x43A9, 0x3E32, 0x38E3, 0x33C0, 0x2ECC, 0x2A0A,\
0x257E, 0x2129, 0x1D0E, 0x1930, 0x1592, 0x1236, 0x0F1D, 0x0C4A,\
0x09BE, 0x077B, 0x0583, 0x03D6, 0x0276, 0x0163, 0x009E, 0x0027,\
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
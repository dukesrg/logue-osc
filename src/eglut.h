/*
 * File: eglut.cpp
 *
 * DX7/DX21/DX11-series EG LUT
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

static q31_t eg_lut[1024] =
{
0x000084F3, 0x00008664, 0x000087DA, 0x00008953, 0x00008AD0, 0x00008C52, 0x00008DD8, 0x00008F62,
0x000090F0, 0x00009283, 0x0000941A, 0x000095B5, 0x00009755, 0x000098FA, 0x00009AA3, 0x00009C50,
0x00009E03, 0x00009FBA, 0x0000A175, 0x0000A336, 0x0000A4FB, 0x0000A6C6, 0x0000A895, 0x0000AA69,
0x0000AC43, 0x0000AE21, 0x0000B005, 0x0000B1EE, 0x0000B3DC, 0x0000B5D0, 0x0000B7C9, 0x0000B9C8,
0x0000BBCC, 0x0000BDD5, 0x0000BFE5, 0x0000C1FA, 0x0000C415, 0x0000C635, 0x0000C85C, 0x0000CA89,
0x0000CCBB, 0x0000CEF4, 0x0000D133, 0x0000D378, 0x0000D5C4, 0x0000D816, 0x0000DA6E, 0x0000DCCD,
0x0000DF32, 0x0000E19E, 0x0000E411, 0x0000E68B, 0x0000E90B, 0x0000EB92, 0x0000EE21, 0x0000F0B6,
0x0000F353, 0x0000F5F7, 0x0000F8A2, 0x0000FB55, 0x0000FE0F, 0x000100D1, 0x0001039B, 0x0001066C,
0x00010945, 0x00010C26, 0x00010F0F, 0x00011200, 0x000114F9, 0x000117FA, 0x00011B04, 0x00011E17,
0x00012131, 0x00012455, 0x00012781, 0x00012AB6, 0x00012DF4, 0x0001313A, 0x0001348A, 0x000137E4,
0x00013B46, 0x00013EB2, 0x00014227, 0x000145A6, 0x0001492F, 0x00014CC1, 0x0001505E, 0x00015404,
0x000157B5, 0x00015B70, 0x00015F35, 0x00016305, 0x000166DF, 0x00016AC4, 0x00016EB4, 0x000172AE,
0x000176B4, 0x00017AC5, 0x00017EE1, 0x00018309, 0x0001873C, 0x00018B7B, 0x00018FC6, 0x0001941C,
0x0001987F, 0x00019CEE, 0x0001A169, 0x0001A5F1, 0x0001AA85, 0x0001AF26, 0x0001B3D3, 0x0001B88E,
0x0001BD56, 0x0001C22B, 0x0001C70E, 0x0001CBFE, 0x0001D0FC, 0x0001D608, 0x0001DB21, 0x0001E049,
0x0001E580, 0x0001EAC4, 0x0001F018, 0x0001F57A, 0x0001FAEB, 0x0002006B, 0x000205FB, 0x00020B9A,
0x00021148, 0x00021707, 0x00021CD5, 0x000222B4, 0x000228A3, 0x00022EA2, 0x000234B2, 0x00023AD2,
0x00024104, 0x00024747, 0x00024D9C, 0x00025402, 0x00025A79, 0x00026103, 0x0002679F, 0x00026E4D,
0x0002750E, 0x00027BE2, 0x000282C8, 0x000289C2, 0x000290CF, 0x000297F0, 0x00029F24, 0x0002A66D,
0x0002ADC9, 0x0002B53A, 0x0002BCC0, 0x0002C45B, 0x0002CC0B, 0x0002D3D0, 0x0002DBAB, 0x0002E39C,
0x0002EBA2, 0x0002F3BF, 0x0002FBF3, 0x0003043D, 0x00030C9E, 0x00031517, 0x00031DA7, 0x0003264F,
0x00032F0F, 0x000337E7, 0x000340D8, 0x000349E2, 0x00035305, 0x00035C41, 0x00036597, 0x00036F07,
0x00037890, 0x00038235, 0x00038BF4, 0x000395CF, 0x00039FC4, 0x0003A9D6, 0x0003B403, 0x0003BE4D,
0x0003C8B3, 0x0003D336, 0x0003DDD6, 0x0003E894, 0x0003F370, 0x0003FE6A, 0x00040982, 0x000414B9,
0x00042010, 0x00042B85, 0x0004371B, 0x000442D1, 0x00044EA7, 0x00045A9F, 0x000466B7, 0x000472F1,
0x00047F4D, 0x00048BCC, 0x0004986D, 0x0004A531, 0x0004B219, 0x0004BF24, 0x0004CC54, 0x0004D9A8,
0x0004E722, 0x0004F4C1, 0x00050285, 0x00051070, 0x00051E82, 0x00052CBA, 0x00053B1B, 0x000549A3,
0x00055853, 0x0005672C, 0x0005762F, 0x0005855B, 0x000594B2, 0x0005A433, 0x0005B3DF, 0x0005C3B6,
0x0005D3BA, 0x0005E3EA, 0x0005F447, 0x000604D2, 0x0006158A, 0x00062671, 0x00063787, 0x000648CD,
0x00065A42, 0x00066BE8, 0x00067DBF, 0x00068FC7, 0x0006A202, 0x0006B46F, 0x0006C70F, 0x0006D9E3,
0x0006ECEC, 0x00070029, 0x0007139C, 0x00072745, 0x00073B24, 0x00074F3B, 0x00076389, 0x00077810,
0x00078CD0, 0x0007A1C9, 0x0007B6FD, 0x0007CC6B, 0x0007E216, 0x0007F7FC, 0x00080E1F, 0x00082480,
0x00083B1F, 0x000851FD, 0x0008691A, 0x00088077, 0x00089816, 0x0008AFF6, 0x0008C818, 0x0008E07D,
0x0008F927, 0x00091214, 0x00092B47, 0x000944C0, 0x00095E80, 0x00097887, 0x000992D7, 0x0009AD6F,
0x0009C851, 0x0009E37F, 0x0009FEF7, 0x000A1ABC, 0x000A36CE, 0x000A532E, 0x000A6FDE, 0x000A8CDC,
0x000AAA2B, 0x000AC7CC, 0x000AE5BF, 0x000B0405, 0x000B229F, 0x000B418F, 0x000B60D3, 0x000B806F,
0x000BA063, 0x000BC0B0, 0x000BE157, 0x000C0257, 0x000C23B4, 0x000C456E, 0x000C6785, 0x000C89FB,
0x000CACD0, 0x000CD006, 0x000CF39E, 0x000D179A, 0x000D3BF9, 0x000D60BC, 0x000D85E7, 0x000DAB78,
0x000DD172, 0x000DF7D5, 0x000E1EA3, 0x000E45DD, 0x000E6D83, 0x000E9598, 0x000EBE1D, 0x000EE711,
0x000F1078, 0x000F3A51, 0x000F649F, 0x000F8F62, 0x000FBA9C, 0x000FE64E, 0x0010127A, 0x00103F21,
0x00106C43, 0x001099E2, 0x0010C801, 0x0010F6A0, 0x001125C0, 0x00115563, 0x0011858A, 0x0011B637,
0x0011E76C, 0x00121929, 0x00124B70, 0x00127E43, 0x0012B1A3, 0x0012E592, 0x00131A11, 0x00134F22,
0x001384C6, 0x0013BAFF, 0x0013F1D0, 0x00142938, 0x0014613A, 0x001499D8, 0x0014D313, 0x00150CED,
0x00154768, 0x00158285, 0x0015BE46, 0x0015FAAE, 0x001637BD, 0x00167576, 0x0016B3DB, 0x0016F2EC,
0x001732AD, 0x0017731F, 0x0017B445, 0x0017F61F, 0x001838AF, 0x00187BF9, 0x0018BFFE, 0x001904C0,
0x00194A41, 0x00199083, 0x0019D788, 0x001A1F52, 0x001A67E4, 0x001AB140, 0x001AFB67, 0x001B465C,
0x001B9222, 0x001BDEBA, 0x001C2C27, 0x001C7A6A, 0x001CC987, 0x001D1981, 0x001D6A58, 0x001DBC10,
0x001E0EAA, 0x001E622B, 0x001EB693, 0x001F0BE6, 0x001F6226, 0x001FB955, 0x00201176, 0x00206A8D,
0x0020C49B, 0x00211FA4, 0x00217BA8, 0x0021D8AD, 0x002236B4, 0x002295C0, 0x0022F5D5, 0x002356F3,
0x0023B920, 0x00241C5F, 0x002480B0, 0x0024E619, 0x00254C9A, 0x0025B438, 0x00261CF7, 0x002686D9,
0x0026F1E1, 0x00275E11, 0x0027CB6F, 0x002839FC, 0x0028A9BD, 0x00291AB4, 0x00298CE4, 0x002A0052,
0x002A7501, 0x002AEAF4, 0x002B622F, 0x002BDAB4, 0x002C5489, 0x002CCFB0, 0x002D4C2D, 0x002DCA05,
0x002E4938, 0x002EC9CE, 0x002F4BCA, 0x002FCF2E, 0x00305400, 0x0030DA41, 0x003161F8, 0x0031EB29,
0x003275D7, 0x00330206, 0x00338FB8, 0x00341EF6, 0x0034AFC2, 0x00354221, 0x0035D616, 0x00366BA4,
0x003702D3, 0x00379BA6, 0x00383622, 0x0038D24C, 0x00397025, 0x003A0FB6, 0x003AB103, 0x003B5410,
0x003BF8E2, 0x003C9F7B, 0x003D47E6, 0x003DF224, 0x003E9E3C, 0x003F4C31, 0x003FFC08, 0x0040ADC9,
0x00416178, 0x0042171B, 0x0042CEB6, 0x0043884C, 0x004443E9, 0x0045018F, 0x0045C143, 0x0046830C,
0x004746ED, 0x00480CF1, 0x0048D51B, 0x00499F71, 0x004A6BF9, 0x004B3AB6, 0x004C0BB5, 0x004CDEF8,
0x004DB486, 0x004E8C65, 0x004F669A, 0x0050432F, 0x00512229, 0x0052038E, 0x0052E766, 0x0053CDB4,
0x0054B684, 0x0055A1DB, 0x00568FC0, 0x0057803A, 0x0058734D, 0x00596906, 0x005A616A, 0x005B5C80,
0x005C5A50, 0x005D5ADD, 0x005E5E36, 0x005F6460, 0x00606D62, 0x00617945, 0x0062880C, 0x006399C6,
0x0064AE7A, 0x0065C62E, 0x0066E0EB, 0x0067FEB5, 0x00691F9E, 0x006A43A9, 0x006B6ADF, 0x006C954A,
0x006DC2ED, 0x006EF3DB, 0x00702818, 0x00715FAD, 0x00729AA3, 0x0073D900, 0x00751AD6, 0x0076602B,
0x0077A906, 0x0078F574, 0x007A4579, 0x007B9927, 0x007CF085, 0x007E4B9D, 0x007FAA7A, 0x00810D20,
0x008273A5, 0x0083DE0D, 0x00854C64, 0x0086BEB5, 0x00883505, 0x0089AF6B, 0x008B2DEC, 0x008CB093,
0x008E376C, 0x008FC27F, 0x009151DF, 0x0092E596, 0x00947DAE, 0x00961A33, 0x0097BB2E, 0x009960B4,
0x009B0ACD, 0x009CB986, 0x009E6CEB, 0x00A02504, 0x00A1E1EA, 0x00A3A3A4, 0x00A56A3F, 0x00A735C9,
0x00A90649, 0x00AADBDA, 0x00ACB684, 0x00AE9654, 0x00B07B59, 0x00B2659B, 0x00B45535, 0x00B64A31,
0x00B8449C, 0x00BA4486, 0x00BC49F7, 0x00BE550D, 0x00C065D0, 0x00C27C50, 0x00C4989D, 0x00C6BABF,
0x00C8E2D6, 0x00CB10EB, 0x00CD450E, 0x00CF7F50, 0x00D1BFBB, 0x00D4066F, 0x00D65376, 0x00D8A6E1,
0x00DB00C2, 0x00DD6123, 0x00DFC827, 0x00E235D7, 0x00E4AA46, 0x00E72587, 0x00E9A7A4, 0x00EC30C2,
0x00EEC0EA, 0x00F15832, 0x00F3F6AD, 0x00F69C65, 0x00F94981, 0x00FBFE0C, 0x00FEBA1B, 0x01017DC2,
0x01044910, 0x01071C28, 0x0109F71A, 0x010CD9FA, 0x010FC4E0, 0x0112B7D6, 0x0115B308, 0x0118B682,
0x011BC25A, 0x011ED6AA, 0x0121F37C, 0x01251900, 0x01284740, 0x012B7E58, 0x012EBE5E, 0x01320760,
0x0135598C, 0x0138B4F4, 0x013C19AE, 0x013F87D6, 0x0142FF7A, 0x014680CC, 0x014A0BDC, 0x014DA0C2,
0x01513F96, 0x0154E87E, 0x01589B92, 0x015C58E6, 0x016020A4, 0x0163F2DC, 0x0167CFB8, 0x016BB74E,
0x016FA9B8, 0x0173A720, 0x0177AF96, 0x017BC346, 0x017FE24A, 0x01840CBC, 0x018842C6, 0x018C847C,
0x0190D20C, 0x01952B90, 0x01999122, 0x019E02F4, 0x01A28116, 0x01A70BBC, 0x01ABA2FE, 0x01B046FA,
0x01B4F7E2, 0x01B9B5CC, 0x01BE80E8, 0x01C35956, 0x01C83F32, 0x01CD32B2, 0x01D233EA, 0x01D74312,
0x01DC6048, 0x01E18BAA, 0x01E6C572, 0x01EC0DB6, 0x01F164AE, 0x01F6CA7C, 0x01FC3F40, 0x0201C334,
0x02075674, 0x020CF938, 0x0212ABA8, 0x02186DE0, 0x021E4020, 0x02242280, 0x022A1548, 0x02301894,
0x02362C88, 0x023C516C, 0x02428754, 0x0248CE8C, 0x024F2730, 0x0255916C, 0x025C0D88, 0x02629B9C,
0x02693BEC, 0x026FEEAC, 0x0276B3F8, 0x027D8C20, 0x0284773C, 0x028B75A0, 0x02928770, 0x0299ACDC,
0x02A0E62C, 0x02A83380, 0x02AF9528, 0x02B70B54, 0x02BE9630, 0x02C63608, 0x02CDEB04, 0x02D5B578,
0x02DD958C, 0x02E58B78, 0x02ED978C, 0x02F5B9F0, 0x02FDF2F8, 0x030642D8, 0x030EA9C4, 0x03172814,
0x031FBDF0, 0x03286BB0, 0x03313190, 0x033A0FC0, 0x034306A0, 0x034C1658, 0x03553F4C, 0x035E81B4,
0x0367DDC4, 0x037153E8, 0x037AE440, 0x03848F3C, 0x038E5514, 0x03983600, 0x03A23270, 0x03AC4A8C,
0x03B67EC4, 0x03C0CF54, 0x03CB3C7C, 0x03D5C6AC, 0x03E06E10, 0x03EB3324, 0x03F61620, 0x04011748,
0x040C3710, 0x041775B8, 0x0422D3A8, 0x042E5130, 0x0439EE88, 0x0445AC40, 0x04518A80, 0x045D89D0,
0x0469AA70, 0x0475ECA8, 0x04825108, 0x048ED7C0, 0x049B8160, 0x04A84E28, 0x04B53E68, 0x04C252B0,
0x04CF8B38, 0x04DCE890, 0x04EA6B08, 0x04F812F0, 0x0505E0E8, 0x0513D518, 0x0521F028, 0x05303268,
0x053E9C28, 0x054D2E10, 0x055BE858, 0x056ACBA0, 0x0579D848, 0x05890EA8, 0x05986F60, 0x05A7FAB8,
0x05B7B158, 0x05C793A0, 0x05D7A1E8, 0x05E7DCF0, 0x05F844E8, 0x0608DA98, 0x06199E58, 0x062A9090,
0x063BB1F8, 0x064D02D8, 0x065E83F0, 0x067035B0, 0x06821870, 0x06942D08, 0x06A673B0, 0x06B8ED48,
0x06CB9A30, 0x06DE7AD8, 0x06F19010, 0x0704DA28, 0x07185A00, 0x072C1000, 0x073FFCA0, 0x075420C0,
0x07687CA8, 0x077D1148, 0x0791DF18, 0x07A6E688, 0x07BC2890, 0x07D1A578, 0x07E75E40, 0x07FD5358,
0x08138550, 0x0829F520, 0x0840A310, 0x08579030, 0x086EBCF0, 0x08862A00, 0x089DD840, 0x08B5C820,
0x08CDFAB0, 0x08E67070, 0x08FF2A00, 0x09182870, 0x09316C20, 0x094AF620, 0x0964C720, 0x097EDFA0,
0x099940E0, 0x09B3EB30, 0x09CEDFC0, 0x09EA1F30, 0x0A05AA20, 0x0A2181D0, 0x0A3DA6A0, 0x0A5A19D0,
0x0A76DC10, 0x0A93EE00, 0x0AB150E0, 0x0ACF0540, 0x0AED0C40, 0x0B0B66C0, 0x0B2A1550, 0x0B491960,
0x0B687360, 0x0B8824A0, 0x0BA82E00, 0x0BC89020, 0x0BE94C70, 0x0C0A6370, 0x0C2BD6A0, 0x0C4DA6B0,
0x0C6FD480, 0x0C926180, 0x0CB54E40, 0x0CD89C40, 0x0CFC4C30, 0x0D205F70, 0x0D44D6C0, 0x0D69B360,
0x0D8EF660, 0x0DB4A0F0, 0x0DDAB440, 0x0E013130, 0x0E281910, 0x0E4F6D00, 0x0E772E40, 0x0E9F5E10,
0x0EC7FD50, 0x0EF10D80, 0x0F1A8FB0, 0x0F448540, 0x0F6EEF80, 0x0F99CF70, 0x0FC52680, 0x0FF0F5F0,
0x101D3F20, 0x104A0360, 0x10774400, 0x10A50240, 0x10D33FA0, 0x1101FD60, 0x11313D20, 0x11610020,
0x119147A0, 0x11C21560, 0x11F36AA0, 0x12254920, 0x1257B220, 0x128AA700, 0x12BE2980, 0x12F23B20,
0x1326DD80, 0x135C11E0, 0x1391DA40, 0x13C837E0, 0x13FF2CA0, 0x1436BA20, 0x146EE1E0, 0x14A7A580,
0x14E106E0, 0x151B07C0, 0x1555A9E0, 0x1590EEC0, 0x15CCD840, 0x16096820, 0x1646A040, 0x168482C0,
0x16C310E0, 0x17024CE0, 0x17423880, 0x1782D5A0, 0x17C42680, 0x18062CA0, 0x1848EA40, 0x188C6120,
0x18D093A0, 0x191583A0, 0x195B3300, 0x19A1A3E0, 0x19E8D8A0, 0x1A30D300, 0x1A7995A0, 0x1AC32240,
0x1B0D7B00, 0x1B58A280, 0x1BA49AA0, 0x1BF16620, 0x1C3F06C0, 0x1C8D7EE0, 0x1CDCD120, 0x1D2CFFC0,
0x1D7E0D60, 0x1DCFFBE0, 0x1E22CE00, 0x1E768620, 0x1ECB26E0, 0x1F20B300, 0x1F772CA0, 0x1FCE9660,
0x2026F300, 0x20804500, 0x20DA8F80, 0x2135D480, 0x21921740, 0x21EF5A00, 0x224DA000, 0x22ACEC40,
0x230D4100, 0x236EA140, 0x23D11000, 0x24349000, 0x24992500, 0x24FED100, 0x25659780, 0x25CD7B80,
0x26368040, 0x26A0A900, 0x270BF840, 0x277871C0, 0x27E61880, 0x2854EFC0, 0x28C4FB40, 0x29363E00,
0x29A8BB40, 0x2A1C76C0, 0x2A917380, 0x2B07B540, 0x2B7F3FC0, 0x2BF81640, 0x2C723C80, 0x2CEDB5C0,
0x2D6A8640, 0x2DE8B1C0, 0x2E683B80, 0x2EE92780, 0x2F6B79C0, 0x2FEF3600, 0x30746040, 0x30FAFC80,
0x31830EC0, 0x320C9AC0, 0x3297A500, 0x332431C0, 0x33B24480, 0x3441E280, 0x34D30F00, 0x3565CEC0,
0x35FA26C0, 0x36901A40, 0x3727AEC0, 0x37C0E840, 0x385BCB40, 0x38F85CC0, 0x3996A100, 0x3A369D00,
0x3AD85580, 0x3B7BCF00, 0x3C210F00, 0x3CC81A00, 0x3D70F540, 0x3E1BA540, 0x3EC82F40, 0x3F769900,
0x4026E700, 0x40D91F00, 0x418D4600, 0x42436180, 0x42FB7700, 0x43B58C00, 0x4471A600, 0x452FCA00,
0x45EFFE80, 0x46B24900, 0x4776AF00, 0x483D3700, 0x4905E680, 0x49D0C380, 0x4A9DD400, 0x4B6D1E00,
0x4C3EA800, 0x4D127880, 0x4DE89500, 0x4EC10480, 0x4F9BCD00, 0x5078F580, 0x51588480, 0x523A8000,
0x531EF000, 0x5405DA80, 0x54EF4600, 0x55DB3A80, 0x56C9BE00, 0x57BAD880, 0x58AE9100, 0x59A4EE00,
0x5A9DF780, 0x5B99B500, 0x5C982E00, 0x5D996A00, 0x5E9D7000, 0x5FA44900, 0x60ADFC00, 0x61BA9100,
0x62CA1080, 0x63DC8200, 0x64F1EE00, 0x660A5C80, 0x6725D600, 0x68446380, 0x69660C80, 0x6A8ADA80,
0x6BB2D600, 0x6CDE0780, 0x6E0C7880, 0x6F3E3180, 0x70733C00, 0x71ABA080, 0x72E76980, 0x74269F80,
0x75694C00, 0x76AF7980, 0x77F93100, 0x79467C00, 0x7A976580, 0x7BEBF680, 0x7D443A00, 0x7EA03980
};
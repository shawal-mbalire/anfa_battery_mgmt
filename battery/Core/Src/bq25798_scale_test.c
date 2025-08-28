/*
 * bq25798_scale_test.c
 *
 * Lightweight self-test harness for the BQ25798 scaling helpers.
 * Pure arithmetic: no hardware interaction, safe to call early in bring-up or from host simulation.
 */
#include <stdio.h>
#include "bq25798.h"

/* Table-driven samples (physical -> expected raw) excluding ambiguous entries */
typedef struct { const char *label; uint32_t physical; uint32_t raw; uint32_t (*enc)(uint32_t); uint32_t (*dec)(uint32_t); } scale_case32;

typedef struct { const char *label; uint16_t physical; uint16_t raw; uint16_t (*enc)(uint16_t); uint16_t (*dec)(uint16_t); } scale_case16;

typedef struct { const char *label; uint16_t physical; uint8_t raw; uint8_t (*enc)(uint16_t); uint16_t (*dec)(uint8_t); } scale_case8;

typedef struct { const char *label; uint16_t physical; uint8_t raw; uint8_t (*enc)(uint16_t); uint16_t (*dec)(uint8_t); } scale_case8_prechg;

void BQ25798_runScalingSelfTest(void){
    unsigned failures = 0;
    printf("[BQ][SCALE] Starting scaling self-test\n");

    /* 16-bit voltage/current (10 mV / 10 mA per LSB) */
    scale_case16 cases16[] = {
        {"VREG 14600mV", 14600, 1460, BQ25798_encodeChargeVoltage_mV, BQ25798_decodeChargeVoltage_raw},
        {"ICHG 4000mA",   4000,  400, BQ25798_encodeChargeCurrent_mA, BQ25798_decodeChargeCurrent_raw},
        {"ICHG 1000mA",   1000,  100, BQ25798_encodeChargeCurrent_mA, BQ25798_decodeChargeCurrent_raw},
        {"ICHG 5000mA",   5000,  500, BQ25798_encodeChargeCurrent_mA, BQ25798_decodeChargeCurrent_raw},
        {"IIN  3300mA",   3300,  330, BQ25798_encodeInputCurrent_mA,  BQ25798_decodeInputCurrent_raw}
    };

    for (unsigned i=0;i<sizeof(cases16)/sizeof(cases16[0]);++i){
        uint16_t enc = cases16[i].enc(cases16[i].physical);
        uint16_t dec = cases16[i].dec(cases16[i].raw);
        int okEnc = (enc == cases16[i].raw);
        int okDec = (dec == cases16[i].physical);
        if (!okEnc || !okDec){ failures++; }
        printf("[BQ][SCALE] %-15s enc=%u(expect %u) dec=%u(expect %u) %s%s\n",
               cases16[i].label, enc, cases16[i].raw, dec, cases16[i].physical,
               (!okEnc||!okDec)?"FAIL":"OK",
               (cases16[i].physical==5000&&!okEnc)?" (check datasheet: sample 0x03F4 vs expected 0x01F4)":"");
    }

    /* 8-bit VINDPM (100 mV per LSB) */
    scale_case8 vindpm_cases[] = {
        {"VINDPM 3600mV", 3600, 36, BQ25798_encodeInputVoltageLimit_mV, BQ25798_decodeInputVoltageLimit_raw},
    };
    for (unsigned i=0;i<sizeof(vindpm_cases)/sizeof(vindpm_cases[0]);++i){
        uint8_t enc = vindpm_cases[i].enc(vindpm_cases[i].physical);
        uint16_t dec = vindpm_cases[i].dec(vindpm_cases[i].raw);
        int okEnc = (enc == vindpm_cases[i].raw);
        int okDec = (dec == vindpm_cases[i].physical);
        if (!okEnc || !okDec) failures++;
        printf("[BQ][SCALE] %-15s enc=%u(expect %u) dec=%u(expect %u) %s\n",
               vindpm_cases[i].label, enc, vindpm_cases[i].raw, dec, vindpm_cases[i].physical,
               (!okEnc||!okDec)?"FAIL":"OK");
    }

    /* Precharge current (40 mA per code) */
    scale_case8_prechg prechg_cases[] = {
        {"PCHG 120mA", 120, 3, BQ25798_encodePrechargeCurrent_mA, BQ25798_decodePrechargeCurrent_raw},
        {"PCHG 160mA", 160, 4, BQ25798_encodePrechargeCurrent_mA, BQ25798_decodePrechargeCurrent_raw},
        {"PCHG 200mA", 200, 5, BQ25798_encodePrechargeCurrent_mA, BQ25798_decodePrechargeCurrent_raw},
    };
    for (unsigned i=0;i<sizeof(prechg_cases)/sizeof(prechg_cases[0]);++i){
        uint8_t enc = prechg_cases[i].enc(prechg_cases[i].physical);
        uint16_t dec = prechg_cases[i].dec(prechg_cases[i].raw);
        int okEnc = (enc == prechg_cases[i].raw);
        int okDec = (dec == prechg_cases[i].physical);
        if (!okEnc || !okDec) failures++;
        printf("[BQ][SCALE] %-15s enc=%u(expect %u) dec=%u(expect %u) %s\n",
               prechg_cases[i].label, enc, prechg_cases[i].raw, dec, prechg_cases[i].physical,
               (!okEnc||!okDec)?"FAIL":"OK");
    }

    printf("[BQ][SCALE] Self-test complete: %u failure(s)\n", failures);
}

/* Rich report: re-runs scaling tests with PASS/FAIL tags and (optionally) performs a live PART_INFO read */
void BQ25798_runAssertionReport(BQ25798 *dev){
    unsigned failures = 0;
    printf("[BQ][ASSERT] Assertion report start\n");

    /* --- Scaling Assertions (mirrors static asserts) --- */
    struct { const char *name; int ok; } tests[] = {
        {"VREG 14600mV encode", BQ25798_encodeChargeVoltage_mV(14600) == 1460},
        {"VREG 1460 raw decode", BQ25798_decodeChargeVoltage_raw(1460) == 14600},
        {"ICHG 4000mA encode",  BQ25798_encodeChargeCurrent_mA(4000) == 400},
        {"ICHG 400 raw decode",  BQ25798_decodeChargeCurrent_raw(400) == 4000},
        {"ICHG 1000mA encode",  BQ25798_encodeChargeCurrent_mA(1000) == 100},
        {"ICHG 5000mA encode",  BQ25798_encodeChargeCurrent_mA(5000) == 500},
        {"VINDPM 3600mV encode", BQ25798_encodeInputVoltageLimit_mV(3600) == 36},
        {"IIN 3300mA encode",   BQ25798_encodeInputCurrent_mA(3300) == 330},
        {"PCHG 120mA encode",   BQ25798_encodePrechargeCurrent_mA(120) == 3},
        {"PCHG code3 decode",   BQ25798_decodePrechargeCurrent_raw(3) == 120},
    };
    for (unsigned i=0;i<sizeof(tests)/sizeof(tests[0]);++i){
        if (!tests[i].ok) failures++;
        printf("[BQ][ASSERT] %-22s : %s\n", tests[i].name, tests[i].ok?"PASS":"FAIL");
    }
    printf("[BQ][ASSERT] Scaling summary: %u failure(s)\n", failures);

    /* --- Part Number / Revision Comparison (if device & HAL present) --- */
#ifndef BQ25798_NO_HAL
    if (dev){
        BQ25798_PartInfo info; BQ25798_Result r = BQ25798_confirmPart(dev, &info);
        int legacy_ok = (info.part3bit == BQ25798_PART_NUM_VAL) && (info.rev == BQ25798_DEV_REV_VAL);
        int full_ok   = (info.part5bit == BQ25798_EXPECTED_PARTNUM_5BIT) && (info.rev == BQ25798_EXPECTED_REVISION);
        printf("[BQ][ASSERT] PART_INFO raw=0x%02X part3=%u part5=0x%02X rev=%u : %s (legacy=%s full=%s)\n",
               info.raw, info.part3bit, info.part5bit, info.rev,
               (r==BQ25798_OK)?"PASS":"FAIL",
               legacy_ok?"ok":"no", full_ok?"ok":"no");
        if (r!=BQ25798_OK) failures++;
    } else {
        printf("[BQ][ASSERT] Device pointer NULL: skipping PART_INFO comparison\n");
    }
#else
    (void)dev; /* unused */
    printf("[BQ][ASSERT] BQ25798_NO_HAL defined: skipping PART_INFO comparison\n");
#endif
    printf("[BQ][ASSERT] Assertion report complete: total %u failure(s)\n", failures);
}

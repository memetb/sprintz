//
//  test_bitpack.cpp
//  Compress
//
//  Created by DB on 9/21/17.
//  Copyright © 2017 D Blalock. All rights reserved.
//

#include <stdio.h>

#include "catch.hpp"
#include "eigen/Eigen"

#include "array_utils.hpp"
#include "bitpack.h"
#include "timing_utils.hpp"
#include "test_utils.hpp"


TEST_CASE("building blocks", "[bitpack]") {
    uint16_t sz = 16;
    Vec_u8 raw(sz);
    raw.setRandom(); // random vals in [0, 255]

    Vec_i8 compressed(sz);
    Vec_u8 decompressed(sz);

    SECTION("bitpack") {
        Vec_u8 compressed(sz);
//        for (uint8_t nbits = 1; nbits <= 1; nbits++) {
        for (uint8_t nbits = 1; nbits <= 8; nbits++) {
            CAPTURE(nbits);

            raw.setRandom();
            raw /= (1 << (8 - nbits));

            auto len = compress8b_bitpack(raw.data(), sz, compressed.data(), nbits);
            REQUIRE(len == (sz / 8) * nbits);

//            std::cout << "raw: " << raw.cast<uint16_t>();
//            std::cout << "comp: " << compressed.cast<uint16_t>();

            auto len2 = decompress8b_bitpack(compressed.data(), len, decompressed.data(), nbits);
            REQUIRE(len2 == sz);
//
//            std::cout << "raw: " << raw.cast<uint16_t>();
//            std::cout << "decomp: " << decompressed.cast<uint16_t>();

            REQUIRE(ar::all_eq(raw, decompressed));
        }
    }
}

TEST_CASE("max_nbits_i16", "[bitpack]") {
    const uint16_t SIZE = 8;
    int16_t data[SIZE];

    SECTION("8bit values") {
        SECTION("nbits_simple") {
        for (int val = -128; val <= 127; val++) {
            // for (int val = -1; val <= -1; val++) {
                CAPTURE(val);
                _set_random_bits(data, SIZE, val);
                for (int i = 0; i < SIZE; i += 8) {
                    uint8_t nbits_simple = needed_nbits_i16x8_simple(data + i);
                    CAPTURE((int)nbits_simple);
                    REQUIRE(nbits_simple == NBITS_COST_I8[val]);
                }
            }
        }
        SECTION("nbits simd") {
            for (int val = -128; val <= 127; val++) {
            // for (int val = -1; val <= -1; val++) {
                CAPTURE(val);
//                 for (int i = 0; i < SIZE; i++) { data[i] = val; }
                _set_random_bits(data, SIZE, val);
                for (int i = 0; i < SIZE; i += 8) {
                    uint8_t nbits = needed_nbits_i16x8(data + i);
                    CAPTURE((int)nbits);
                    REQUIRE(nbits == NBITS_COST_I8[val]);
                }
            }
        }
    }
    SECTION("all 16bit values") {
        // for (int val = -1; val <= -1; val++) {
        for (int val = -32768; val <= 32767; val++) {
//            for (int i = 0; i < SIZE; i++) { data[i] = val; }
            _set_random_bits(data, SIZE, val);
            for (int i = 0; i < SIZE; i += 8) {
                CAPTURE(val);
                uint8_t nbits = needed_nbits_i16x8(data + i);
                uint8_t nbits_simple = needed_nbits_i16x8_simple(data + i);
                CAPTURE((int)nbits);
                CAPTURE((int)nbits_simple);
                if (nbits_simple != nbits) {
                    printf("val=%d) nbits, nbits_simple: %d, %d\n", val, nbits, nbits_simple);
                }
                REQUIRE(nbits_simple == nbits);
            }
        }
    }
 }

TEST_CASE("max_nbits_i8", "[bitpack]") {
   const uint16_t SIZE = 8 * 8;
   int8_t data[SIZE];

    SECTION("nbits_simple") {
        srand(123);
        for (int val = -128; val <= 127; val++) {
            CAPTURE(val);
            _set_random_bits(data, SIZE, val);
            for (int i = 0; i < SIZE / 8; i += 8) {
                uint8_t nbits_simple = needed_nbits_i8x8_simple(data + i);
                REQUIRE(nbits_simple == NBITS_COST_I8[val]);
            }
        }
    }
    SECTION("nbits_simd") {
       for (int val = -128; val <= 127; val++) {
           CAPTURE(val);
           _set_random_bits(data, SIZE, val);
           for (int i = 0; i < SIZE / 8; i += 8) {
               uint8_t nbits = needed_nbits_i8x8(data + i);
               CAPTURE((int)nbits);
               CAPTURE((int)NBITS_COST_I8[val]);
               REQUIRE(nbits == NBITS_COST_I8[val]);
           }
       }
   }
}

TEST_CASE("profile_bitpack_u8", "[profile][bitpack]") {
    uint64_t sz = 256 * 1024 * 1024;
    Vec_u8 raw_orig(sz);
    Vec_u8 raw(sz);
    raw.setRandom(); // random vals in [0, 255]

    Vec_u8 compressed(sz);
    Vec_u8 decompressed(sz);
    for (uint8_t nbits = 1; nbits <= 8; nbits++) {
        std::cout << "---- nbits: " << (uint16_t)nbits << "\n";
//        raw.setRandom();
        raw = raw_orig / (1 << (8 - nbits));

        uint64_t len = 0, len2 = 0;
        {
//            cputime_t _tstart(timeNow());
            volatile PrintTimer t("compress");
            len = compress8b_bitpack(raw.data(), sz, compressed.data(), nbits);
//            auto elapsed = durationMs(_tstart, timeNow());
//            std::cout << "compress time:\t" << elapsed << "\tms\n";
        }
        if (len != (sz / 8) * nbits) { std::cout << "compression error!\n"; }
       REQUIRE(len == (sz / 8) * nbits);

        {
            volatile PrintTimer t("decompress");
            len2 = decompress8b_bitpack(compressed.data(), len, decompressed.data(), nbits);
        }
        if (len2 != sz) { std::cout << "decompresion error!\n"; }
       REQUIRE(len2 == sz);
    }
}
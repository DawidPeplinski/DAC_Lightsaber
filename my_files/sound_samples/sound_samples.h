#ifndef __SOUND_SAMPLES
#define __SOUND_SAMPLES

#include "idle_samples.h"
#include "power_up_samples.h"
#include "power_down_samples.h"
#include "hit1_samples.h"
#include "hit2_samples.h"
#include "hit3_samples.h"
#include "hit4_samples.h"
#include "hit5_samples.h"
#include "hit6_samples.h"
#include "hit7_samples.h"
#include "hit8_samples.h"
#include "swing1_samples.h"
#include "swing2_samples.h"
#include "swing3_samples.h"
#include "swing4_samples.h"
#include "swing5_samples.h"
#include "swing6_samples.h"
#include "swing7_samples.h"
#include "swing8_samples.h"
#include "swing9_samples.h"

// #define USE_EMPTY_SAMPLES_ARRAYS

#define POWER_UP_PATTERN      0
#define IDLE_PATTERN          1
#define HIT1_PATTERN          2
#define HIT2_PATTERN          3
#define HIT3_PATTERN          4
#define HIT4_PATTERN          5
#define HIT5_PATTERN          6
#define HIT6_PATTERN          7
#define HIT7_PATTERN          8
#define HIT8_PATTERN          9
#define LAST_HIT_PATTERN      HIT8_PATTERN
#define SWING1_PATTERN        10
#define SWING2_PATTERN        11
#define SWING3_PATTERN        12
#define SWING4_PATTERN        13
#define SWING5_PATTERN        14
#define SWING6_PATTERN        15
#define SWING7_PATTERN        16
#define SWING8_PATTERN        17
#define SWING9_PATTERN        18
#define LAST_SWING_PATTERN    SWING9_PATTERN
#define POWER_DOWN_PATTERN    19


typedef struct samples_data_struct {
  const uint8_t        *array_ptr;
  uint32_t              array_size;  
} samples_data_struct;

samples_data_struct power_up_struct = {
    .array_ptr = power_up_samples,
    .array_size = sizeof(power_up_samples)
};

samples_data_struct idle_struct = {
    .array_ptr = idle_samples,
    .array_size = sizeof(idle_samples)
};

samples_data_struct hit1_struct = {
    .array_ptr = hit1_samples,
    .array_size = sizeof(hit1_samples)
};

samples_data_struct hit2_struct = {
    .array_ptr = hit2_samples,
    .array_size = sizeof(hit2_samples)
};

samples_data_struct hit3_struct = {
    .array_ptr = hit3_samples,
    .array_size = sizeof(hit3_samples)
};

samples_data_struct hit4_struct = {
    .array_ptr = hit4_samples,
    .array_size = sizeof(hit4_samples)
};

samples_data_struct hit5_struct = {
    .array_ptr = hit5_samples,
    .array_size = sizeof(hit5_samples)
};

samples_data_struct hit6_struct = {
    .array_ptr = hit6_samples,
    .array_size = sizeof(hit6_samples)
};

samples_data_struct hit7_struct = {
    .array_ptr = hit7_samples,
    .array_size = sizeof(hit7_samples)
};

samples_data_struct hit8_struct = {
    .array_ptr = hit8_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing1_struct = {
    .array_ptr = swing1_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing2_struct = {
    .array_ptr = swing2_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing3_struct = {
    .array_ptr = swing3_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing4_struct = {
    .array_ptr = swing4_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing5_struct = {
    .array_ptr = swing5_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing6_struct = {
    .array_ptr = swing6_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing7_struct = {
    .array_ptr = swing7_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing8_struct = {
    .array_ptr = swing8_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct swing9_struct = {
    .array_ptr = swing9_samples,
    .array_size = sizeof(hit8_samples)
};

samples_data_struct power_down_struct = {
    .array_ptr = power_down_samples,
    .array_size = sizeof(power_down_samples)
};


#ifdef USE_EMPTY_SAMPLES_ARRAYS
const uint8_t empty_samples_array[] = {
    0,
    0,
    0
};

samples_data_struct empty_samples_struct = {
    .array_ptr = empty_samples_array,
    .array_size = sizeof(empty_samples_array)
};

samples_data_struct *sound_structs_array[] = {
    &empty_samples_struct, // &power_up_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct,
    &empty_samples_struct //&power_down_struct
};
 #else
samples_data_struct *sound_structs_array[] = {
    &power_up_struct,
    &idle_struct,
    &hit1_struct,
    &hit2_struct,
    &hit3_struct,
    &hit4_struct,
    &hit5_struct,
    &hit6_struct,
    &hit7_struct,
    &hit8_struct,
    &swing1_struct,
    &swing2_struct,
    &swing3_struct,
    &swing4_struct,
    &swing5_struct,
    &swing6_struct,
    &swing7_struct,
    &swing8_struct,
    &swing9_struct,
    &power_down_struct
};
#endif

#endif
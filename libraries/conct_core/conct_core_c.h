#pragma once

#include "conct_platform.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum conct_value_type
{
	conct_value_type_void,
	conct_value_type_boolean,
	conct_value_type_integer,
	conct_value_type_unsigned,
	conct_value_type_percent_value,
	conct_value_type_device_id,
	conct_value_type_instance_id,
	conct_value_type_type_crc,
	conct_value_type_string,
	conct_value_type_struct,
	conct_value_type_array
};

typedef uint8_t conct_device_id;
typedef uint16_t conct_type_crc;
typedef uint16_t conct_instance_id;
typedef uint16_t conct_percent_value;
typedef uint32_t conct_guid;

struct conct_instance
{
	conct_instance_id	id;
	conct_type_crc		type;
};

#ifdef __cplusplus
}
#endif

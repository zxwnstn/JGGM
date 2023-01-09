#pragma once

#if _WIN32

using int8 = char;
using uint8 = unsigned char;
using int16 = short;
using uint16 = unsigned short;
using int32 = int;
using uint32 = unsigned int;
using int64 = long long;
using uint64 = unsigned long long;

enum : int32 {
	INVALID_INDEX = -1,
	INVALID_ID_32 = -1,
};
enum : int64 {
	INVALID_ID_64 = -1,
};

#endif
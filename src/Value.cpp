#include "bp/Value.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cmath>

/**
 * Initialize a BpValue structure.
 * This sets the value to a known empty state.
 */
void bpValueInit(BpValue *value) {
    if (!value)
        return;

    memset(&value->value, 0, sizeof(BpVal));
    value->size = 0;
    value->tag = BP_VAL_TYPE_NONE;
}

/**
 * Clear a BpValue and free associated memory.
 */
void bpValueClear(BpValue *value) {
    if (!value)
        return;

    // Free any dynamically allocated memory
    if (bpValueIsString(value) && value->value.str) {
        free(value->value.str);
        value->value.str = nullptr;
    } else if (bpValueIsBuffer(value) && value->value.buf) {
        free(value->value.buf);
        value->value.buf = nullptr;
    }

    // Reset value to empty state
    bpValueSetType(value, BP_VAL_TYPE_NONE, BP_VAL_SUBTYPE_NONE);
    value->size = 0;
    memset(&value->value, 0, sizeof(BpVal));
}

/**
 * Copy source BpValue to destination.
 */
void bpValueCopy(BpValue *dest, const BpValue *src) {
    if (!dest || !src)
        return;

    if (dest == src)
        return;

    // Clear destination first to free any existing resources
    bpValueClear(dest);

    switch (bpValueGetType(src)) {
    case BP_VAL_TYPE_BOOL:
        dest->value.b = src->value.b;
        dest->size = src->size;
        dest->tag = src->tag;
        break;
    case BP_VAL_TYPE_CHAR:
        dest->value.c = src->value.c;
        dest->size = src->size;
        dest->tag = src->tag;
        break;
    case BP_VAL_TYPE_NUM:
        memcpy(&dest->value, &src->value, sizeof(BpVal));
        dest->size = src->size;
        dest->tag = src->tag;
        break;
    case BP_VAL_TYPE_STR:
        if (src->value.str) {
            bpValueSetString(dest, src->value.str);
        }
        break;
    case BP_VAL_TYPE_BUF:
        if (src->value.buf && src->size > 0) {
            bpValueSetBuffer(dest, src->value.buf, src->size);
        }
        break;
    case BP_VAL_TYPE_PTR:
        dest->value.ptr = src->value.ptr;
        dest->size = src->size;
        dest->tag = src->tag;
        break;
    default:
        // Just initialize to empty for unknown types
        bpValueInit(dest);
        break;
    }
}

/*
 * Safely copy a BpValue with error handling
 */
bool bpValueCopySafe(BpValue *dest, const BpValue *src) {
    if (!dest || !src)
        return false;

    if (dest == src)
        return true;

    // Handle string and buffer types specially
    if (bpValueIsString(src)) {
        const char *srcStr = bpValueGetString(src);
        if (!srcStr)
            return false;

        char *newStr = strdup(srcStr);
        if (!newStr)
            return false;

        bpValueClear(dest);
        dest->value.str = newStr;
        dest->size = src->size;
        dest->tag = src->tag;
        return true;
    } else if (bpValueIsBuffer(src)) {
        const void *srcBuf = bpValueGetBuffer(src);
        size_t srcSize = bpValueGetSize(src);
        if (!srcBuf || srcSize == 0)
            return false;

        void *newBuf = malloc(srcSize);
        if (!newBuf)
            return false;

        memcpy(newBuf, srcBuf, srcSize);
        bpValueClear(dest);
        dest->value.buf = newBuf;
        dest->size = srcSize;
        dest->tag = src->tag;
        return true;
    } else {
        // For other types, a simple copy is sufficient
        bpValueClear(dest);
        memcpy(&dest->value, &src->value, sizeof(BpVal));
        dest->size = src->size;
        dest->tag = src->tag;
        return true;
    }
}

/*
 * Thread-safe version of bpValueClear with double-free protection
 */
void bpValueClearSafe(BpValue *value) {
    if (!value)
        return;

    // For string and buffer types, only free if pointer is not null
    if (bpValueIsString(value) && value->value.str) {
        void *ptr = value->value.str;
        value->value.str = NULL; // Mark as cleared before freeing
        free(ptr);
    } else if (bpValueIsBuffer(value) && value->value.buf) {
        void *ptr = value->value.buf;
        value->value.buf = NULL; // Mark as cleared before freeing
        free(ptr);
    }

    // Reset value to empty state
    value->size = 0;
    value->tag = BP_VAL_TYPE_NONE;
    memset(&value->value, 0, sizeof(BpVal));
}

/**
 * Move source BpValue to destination (source becomes empty).
 */
void bpValueMove(BpValue *dest, BpValue *src) {
    if (!dest || !src)
        return;

    if (dest == src)
        return;

    // Clear destination first to free any existing resources
    bpValueClear(dest);

    // Copy all values from source to destination
    memcpy(&dest->value, &src->value, sizeof(BpVal));
    dest->size = src->size;
    dest->tag = src->tag;

    // Reset source to empty state without freeing memory
    memset(&src->value, 0, sizeof(BpVal));
    src->size = 0;
    src->tag = BP_VAL_TYPE_NONE;
}

/**
 * Swap two BpValue objects.
 */
void bpValueSwap(BpValue *lhs, BpValue *rhs) {
    if (!lhs || !rhs)
        return;

    if (lhs == rhs)
        return;

    // Use primitive variables for the temporary storage
    BpVal tmpValue;
    size_t tmpSize;
    uint8_t tmpTag;

    // Save lhs to temporaries
    memcpy(&tmpValue, &lhs->value, sizeof(BpVal));
    tmpSize = lhs->size;
    tmpTag = lhs->tag;

    // Copy rhs to lhs
    memcpy(&lhs->value, &rhs->value, sizeof(BpVal));
    lhs->size = rhs->size;
    lhs->tag = rhs->tag;

    // Copy temporaries to rhs
    memcpy(&rhs->value, &tmpValue, sizeof(BpVal));
    rhs->size = tmpSize;
    rhs->tag = tmpTag;
}

/**
 * Get the size of the value's data.
 */
size_t bpValueGetSize(const BpValue *value) {
    return value ? value->size : 0;
}

/**
 * Get the tag of the value.
 */
uint8_t bpValueGetTag(const BpValue *value) {
    return value ? static_cast<uint8_t>(value->tag & BP_VAL_TAG_MASK) : 0;
}

/**
 * Set the tag of the value.
 */
void bpValueSetTag(BpValue *value, uint8_t tag) {
    if (value) {
        value->tag = static_cast<uint8_t>(tag & BP_VAL_TAG_MASK);
    }
}

/**
 * Get the type of the value.
 */
BpValueType bpValueGetType(const BpValue *value) {
    return value ? static_cast<BpValueType>(value->tag & BP_VAL_TYPE_MASK) : BP_VAL_TYPE_NONE;
}

/**
 * Set the type and subtype of the value.
 */
void bpValueSetType(BpValue *value, BpValueType type, BpValueSubtype subtype) {
    if (value) {
        value->tag = static_cast<uint8_t>((type & BP_VAL_TYPE_MASK) | (subtype & BP_VAL_SUBTYPE_MASK));
    }
}

/**
 * Get the subtype of the value.
 */
BpValueSubtype bpValueGetSubtype(const BpValue *value) {
    return value ? static_cast<BpValueSubtype>(value->tag & BP_VAL_SUBTYPE_MASK) : BP_VAL_SUBTYPE_NONE;
}

/**
 * Set the subtype of the value.
 */
void bpValueSetSubtype(BpValue *value, BpValueSubtype subtype) {
    if (value) {
        value->tag = static_cast<uint8_t>((value->tag & ~BP_VAL_SUBTYPE_MASK) | (subtype & BP_VAL_SUBTYPE_MASK));
    }
}

/**
 * Type check functions
 */
bool bpValueIsNone(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_NONE;
}

bool bpValueIsBool(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_BOOL;
}

bool bpValueIsChar(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_CHAR;
}

bool bpValueIsNum(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_NUM;
}

bool bpValueIsUint8(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_UINT8;
}

bool bpValueIsInt8(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_INT8;
}

bool bpValueIsUint16(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_UINT16;
}

bool bpValueIsInt16(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_INT16;
}

bool bpValueIsUint32(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_UINT32;
}

bool bpValueIsInt32(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_INT32;
}

bool bpValueIsUint64(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_UINT64;
}

bool bpValueIsInt64(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_INT64;
}

bool bpValueIsFloat32(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_FLOAT32;
}

bool bpValueIsFloat64(const BpValue *value) {
    return bpValueIsNum(value) && bpValueGetSubtype(value) == BP_VAL_SUBTYPE_FLOAT64;
}

bool bpValueIsString(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_STR;
}

bool bpValueIsBuffer(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_BUF;
}

bool bpValueIsPointer(const BpValue *value) {
    return bpValueGetType(value) == BP_VAL_TYPE_PTR;
}

/**
 * Compare two BpValue objects for equality.
 */
bool bpValueEquals(const BpValue *lhs, const BpValue *rhs) {
    if (!lhs || !rhs)
        return false;

    if (lhs == rhs)
        return true;

    // Different types can't be equal
    if (bpValueGetType(lhs) != bpValueGetType(rhs))
        return false;

    // For numeric values, also check subtype
    if (bpValueIsNum(lhs) && bpValueGetSubtype(lhs) != bpValueGetSubtype(rhs))
        return false;

    // Compare based on type
    switch (bpValueGetType(lhs)) {
    case BP_VAL_TYPE_BOOL:
        return bpValueGetBool(lhs) == bpValueGetBool(rhs);

    case BP_VAL_TYPE_CHAR:
        return bpValueGetChar(lhs) == bpValueGetChar(rhs);

    case BP_VAL_TYPE_NUM:
        switch (bpValueGetSubtype(lhs)) {
        case BP_VAL_SUBTYPE_UINT8:
            return bpValueGetUint8(lhs) == bpValueGetUint8(rhs);
        case BP_VAL_SUBTYPE_INT8:
            return bpValueGetInt8(lhs) == bpValueGetInt8(rhs);
        case BP_VAL_SUBTYPE_UINT16:
            return bpValueGetUint16(lhs) == bpValueGetUint16(rhs);
        case BP_VAL_SUBTYPE_INT16:
            return bpValueGetInt16(lhs) == bpValueGetInt16(rhs);
        case BP_VAL_SUBTYPE_UINT32:
            return bpValueGetUint32(lhs) == bpValueGetUint32(rhs);
        case BP_VAL_SUBTYPE_INT32:
            return bpValueGetInt32(lhs) == bpValueGetInt32(rhs);
        case BP_VAL_SUBTYPE_UINT64:
            return bpValueGetUint64(lhs) == bpValueGetUint64(rhs);
        case BP_VAL_SUBTYPE_INT64:
            return bpValueGetInt64(lhs) == bpValueGetInt64(rhs);
        case BP_VAL_SUBTYPE_FLOAT32: {
            float a = bpValueGetFloat32(lhs);
            float b = bpValueGetFloat32(rhs);
            // Handle NaN values properly
            if (std::isnan(a) && std::isnan(b))
                return true;
            return a == b;
        }
        case BP_VAL_SUBTYPE_FLOAT64: {
            double a = bpValueGetFloat64(lhs);
            double b = bpValueGetFloat64(rhs);
            // Handle NaN values properly
            if (std::isnan(a) && std::isnan(b))
                return true;
            return a == b;
        }
        default:
            return false;
        }

    case BP_VAL_TYPE_STR: {
        const char *str1 = bpValueGetString(lhs);
        const char *str2 = bpValueGetString(rhs);

        // Handle null strings
        if (!str1 || !str2)
            return str1 == str2;

        return strcmp(str1, str2) == 0;
    }

    case BP_VAL_TYPE_BUF: {
        const void *buf1 = bpValueGetBuffer(lhs);
        const void *buf2 = bpValueGetBuffer(rhs);
        size_t size1 = bpValueGetSize(lhs);
        size_t size2 = bpValueGetSize(rhs);

        // Different sizes or null buffers
        if (size1 != size2 || !buf1 || !buf2)
            return buf1 == buf2 && size1 == size2;

        return memcmp(buf1, buf2, size1) == 0;
    }

    case BP_VAL_TYPE_PTR:
        // For pointers, we only compare the actual pointer values
        return bpValueGetPointer(lhs) == bpValueGetPointer(rhs);

    default:
        // For unknown or NONE types, they're equal if both have the same type
        return true;
    }
}

/**
 * Value comparison functions
 */
bool bpValueEqualsBool(const BpValue *value, bool val) {
    return bpValueIsBool(value) && bpValueGetBool(value) == val;
}

bool bpValueEqualsChar(const BpValue *value, char val) {
    return bpValueIsChar(value) && bpValueGetChar(value) == val;
}

bool bpValueEqualsUint8(const BpValue *value, uint8_t val) {
    return bpValueIsUint8(value) && bpValueGetUint8(value) == val;
}

bool bpValueEqualsInt8(const BpValue *value, int8_t val) {
    return bpValueIsInt8(value) && bpValueGetInt8(value) == val;
}

bool bpValueEqualsUint16(const BpValue *value, uint16_t val) {
    return bpValueIsUint16(value) && bpValueGetUint16(value) == val;
}

bool bpValueEqualsInt16(const BpValue *value, int16_t val) {
    return bpValueIsInt16(value) && bpValueGetInt16(value) == val;
}

bool bpValueEqualsUint32(const BpValue *value, uint32_t val) {
    return bpValueIsUint32(value) && bpValueGetUint32(value) == val;
}

bool bpValueEqualsInt32(const BpValue *value, int32_t val) {
    return bpValueIsInt32(value) && bpValueGetInt32(value) == val;
}

bool bpValueEqualsUint64(const BpValue *value, uint64_t val) {
    return bpValueIsUint64(value) && bpValueGetUint64(value) == val;
}

bool bpValueEqualsInt64(const BpValue *value, int64_t val) {
    return bpValueIsInt64(value) && bpValueGetInt64(value) == val;
}

bool bpValueEqualsFloat32(const BpValue *value, float val) {
    if (!bpValueIsFloat32(value))
        return false;

    float storedVal = bpValueGetFloat32(value);
    // Handle NaN values properly
    if (std::isnan(val) && std::isnan(storedVal))
        return true;
    return storedVal == val;
}

bool bpValueEqualsFloat64(const BpValue *value, double val) {
    if (!bpValueIsFloat64(value))
        return false;

    double storedVal = bpValueGetFloat64(value);
    // Handle NaN values properly
    if (std::isnan(val) && std::isnan(storedVal))
        return true;
    return storedVal == val;
}

bool bpValueEqualsString(const BpValue *value, const char *str) {
    if (!bpValueIsString(value) || !str)
        return false;

    const char *storedStr = bpValueGetString(value);
    if (!storedStr)
        return false;

    return strcmp(storedStr, str) == 0;
}

bool bpValueEqualsBuffer(const BpValue *value, const void *buf, size_t size) {
    if (!bpValueIsBuffer(value) || !buf || size == 0)
        return false;

    const void *storedBuf = bpValueGetBuffer(value);
    size_t storedSize = bpValueGetSize(value);

    if (!storedBuf || storedSize != size)
        return false;

    return memcmp(storedBuf, buf, size) == 0;
}

bool bpValueEqualsPointer(const BpValue *value, void *ptr, size_t size) {
    return bpValueIsPointer(value) && bpValueGetPointer(value) == ptr && bpValueGetSize(value) == size;
}

/**
 * Getter functions
 */
bool bpValueGetBool(const BpValue *value) {
    if (!value || !bpValueIsBool(value))
        return false;
    return value->value.b;
}

char bpValueGetChar(const BpValue *value) {
    if (!value || !bpValueIsChar(value))
        return '\0';
    return value->value.c;
}

uint8_t bpValueGetUint8(const BpValue *value) {
    if (!value || !bpValueIsUint8(value))
        return 0;
    return value->value.u8;
}

int8_t bpValueGetInt8(const BpValue *value) {
    if (!value || !bpValueIsInt8(value))
        return 0;
    return value->value.i8;
}

uint16_t bpValueGetUint16(const BpValue *value) {
    if (!value || !bpValueIsUint16(value))
        return 0;
    return value->value.u16;
}

int16_t bpValueGetInt16(const BpValue *value) {
    if (!value || !bpValueIsInt16(value))
        return 0;
    return value->value.i16;
}

uint32_t bpValueGetUint32(const BpValue *value) {
    if (!value || !bpValueIsUint32(value))
        return 0;
    return value->value.u32;
}

int32_t bpValueGetInt32(const BpValue *value) {
    if (!value || !bpValueIsInt32(value))
        return 0;
    return value->value.i32;
}

uint64_t bpValueGetUint64(const BpValue *value) {
    if (!value || !bpValueIsUint64(value))
        return 0;
    return value->value.u64;
}

int64_t bpValueGetInt64(const BpValue *value) {
    if (!value || !bpValueIsInt64(value))
        return 0;
    return value->value.i64;
}

float bpValueGetFloat32(const BpValue *value) {
    if (!value)
        return 0.0f;

    if (bpValueIsFloat32(value))
        return value->value.f32;
    else if (bpValueIsFloat64(value))
        return static_cast<float>(value->value.f64);

    return 0.0f;
}

double bpValueGetFloat64(const BpValue *value) {
    if (!value)
        return 0.0;

    if (bpValueIsFloat64(value))
        return value->value.f64;
    else if (bpValueIsFloat32(value))
        return static_cast<double>(value->value.f32);

    return 0.0;
}

const char *bpValueGetString(const BpValue *value) {
    if (!value || !bpValueIsString(value))
        return nullptr;
    return value->value.str;
}

const void *bpValueGetBuffer(const BpValue *value) {
    if (!value || !bpValueIsBuffer(value))
        return nullptr;
    return value->value.buf;
}

void *bpValueGetPointer(const BpValue *value) {
    if (!value || !bpValueIsPointer(value))
        return nullptr;
    return value->value.ptr;
}

/**
 * Setter functions
 */
void bpValueSetBool(BpValue *value, bool val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.b = val;
    value->size = sizeof(bool);
    bpValueSetType(value, BP_VAL_TYPE_BOOL, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetChar(BpValue *value, char val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.c = val;
    value->size = sizeof(char);
    bpValueSetType(value, BP_VAL_TYPE_CHAR, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetUint8(BpValue *value, uint8_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.u8 = val;
    value->size = sizeof(uint8_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_UINT8);
}

void bpValueSetInt8(BpValue *value, int8_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.i8 = val;
    value->size = sizeof(int8_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_INT8);
}

void bpValueSetUint16(BpValue *value, uint16_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.u16 = val;
    value->size = sizeof(uint16_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_UINT16);
}

void bpValueSetInt16(BpValue *value, int16_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.i16 = val;
    value->size = sizeof(int16_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_INT16);
}

void bpValueSetUint32(BpValue *value, uint32_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.u32 = val;
    value->size = sizeof(uint32_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_UINT32);
}

void bpValueSetInt32(BpValue *value, int32_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.i32 = val;
    value->size = sizeof(int32_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_INT32);
}

void bpValueSetUint64(BpValue *value, uint64_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.u64 = val;
    value->size = sizeof(uint64_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_UINT64);
}

void bpValueSetInt64(BpValue *value, int64_t val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.i64 = val;
    value->size = sizeof(int64_t);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_INT64);
}

void bpValueSetFloat32(BpValue *value, float val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.f32 = val;
    value->size = sizeof(float);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_FLOAT32);
}

void bpValueSetFloat64(BpValue *value, double val) {
    if (!value)
        return;

    bpValueClear(value);
    value->value.f64 = val;
    value->size = sizeof(double);
    bpValueSetType(value, BP_VAL_TYPE_NUM, BP_VAL_SUBTYPE_FLOAT64);
}

void bpValueSetString(BpValue *value, const char *str) {
    if (!value || !str)
        return;

    // Allocate memory first, before clearing previous value
    char *newStr = strdup(str);
    if (!newStr) {
        // If allocation fails, leave the value unchanged
        return;
    }

    // Only clear after successful allocation
    bpValueClear(value);

    value->value.str = newStr;
    value->size = strlen(newStr);
    bpValueSetType(value, BP_VAL_TYPE_STR, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetStringN(BpValue *value, const char *str, size_t size) {
    if (!value || !str || size == 0)
        return;

    // Allocate memory first, before clearing previous value
    char *newStr = static_cast<char *>(malloc(size + 1));
    if (!newStr) {
        // If allocation fails, leave the value unchanged
        return;
    }

    memcpy(newStr, str, size);
    newStr[size] = '\0'; // Ensure null termination

    // Only clear after successful allocation
    bpValueClear(value);

    value->value.str = newStr;
    value->size = size;
    bpValueSetType(value, BP_VAL_TYPE_STR, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetStringV(BpValue *value, const char *fmt, va_list args) {
    if (!value || !fmt)
        return;

    // Calculate required buffer size
    va_list args_copy;
    va_copy(args_copy, args);
    const int len = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (len < 0) {
        // Handle formatting error
        return;
    }

    // Allocate memory first, before clearing previous value
    char *newStr = static_cast<char *>(malloc(len + 1));
    if (!newStr) {
        // Handle allocation failure
        return;
    }

    // Format the string into the buffer
    vsnprintf(newStr, len + 1, fmt, args);

    // Only clear after successful allocation
    bpValueClear(value);

    value->value.str = newStr;
    value->size = len;
    bpValueSetType(value, BP_VAL_TYPE_STR, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetStringF(BpValue *value, const char *fmt, ...) {
    if (!value || !fmt)
        return;

    va_list args;
    va_start(args, fmt);
    bpValueSetStringV(value, fmt, args);
    va_end(args);
}

void bpValueSetBuffer(BpValue *value, const void *buf, size_t size) {
    if (!value || !buf || size == 0)
        return;

    // Allocate memory first, before clearing previous value
    void *newBuf = malloc(size);
    if (!newBuf) {
        // If allocation fails, leave the value unchanged
        return;
    }

    memcpy(newBuf, buf, size);

    // Only clear after successful allocation
    bpValueClear(value);

    value->value.buf = newBuf;
    value->size = size;
    bpValueSetType(value, BP_VAL_TYPE_BUF, BP_VAL_SUBTYPE_NONE);
}

void bpValueSetPointer(BpValue *value, void *ptr, size_t size) {
    if (!value || !ptr)
        return;

    // Always clear previous value first
    bpValueClear(value);

    value->value.ptr = ptr;
    value->size = size;
    bpValueSetType(value, BP_VAL_TYPE_PTR, BP_VAL_SUBTYPE_NONE);
}

/**
 * Numeric conversion functions
 */
int bpValueToInt(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<int>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return static_cast<int>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<int>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return static_cast<int>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<int>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return value->value.i32;
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<int>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return static_cast<int>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return static_cast<int>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return static_cast<int>(value->value.f64);
    default:
        return 0;
    }
}

long bpValueToLong(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0L;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<long>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return static_cast<long>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<long>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return static_cast<long>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<long>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return static_cast<long>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<long>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return static_cast<long>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return static_cast<long>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return static_cast<long>(value->value.f64);
    default:
        return 0L;
    }
}

long long bpValueToLongLong(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0LL;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<long long>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return static_cast<long long>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<long long>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return static_cast<long long>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<long long>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return static_cast<long long>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<long long>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return value->value.i64;
    case BP_VAL_SUBTYPE_FLOAT32:
        return static_cast<long long>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return static_cast<long long>(value->value.f64);
    default:
        return 0LL;
    }
}

unsigned int bpValueToUint(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0U;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<unsigned int>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return value->value.i8 < 0 ? 0U : static_cast<unsigned int>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<unsigned int>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return value->value.i16 < 0 ? 0U : static_cast<unsigned int>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return value->value.u32;
    case BP_VAL_SUBTYPE_INT32:
        return value->value.i32 < 0 ? 0U : static_cast<unsigned int>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<unsigned int>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return value->value.i64 < 0 ? 0U : static_cast<unsigned int>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return value->value.f32 < 0 ? 0U : static_cast<unsigned int>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return value->value.f64 < 0 ? 0U : static_cast<unsigned int>(value->value.f64);
    default:
        return 0U;
    }
}

unsigned long bpValueToUlong(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0UL;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<unsigned long>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return value->value.i8 < 0 ? 0UL : static_cast<unsigned long>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<unsigned long>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return value->value.i16 < 0 ? 0UL : static_cast<unsigned long>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<unsigned long>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return value->value.i32 < 0 ? 0UL : static_cast<unsigned long>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<unsigned long>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return value->value.i64 < 0 ? 0UL : static_cast<unsigned long>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return value->value.f32 < 0 ? 0UL : static_cast<unsigned long>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return value->value.f64 < 0 ? 0UL : static_cast<unsigned long>(value->value.f64);
    default:
        return 0UL;
    }
}

unsigned long long bpValueToUlongLong(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0ULL;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<unsigned long long>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return value->value.i8 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<unsigned long long>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return value->value.i16 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<unsigned long long>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return value->value.i32 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return value->value.u64;
    case BP_VAL_SUBTYPE_INT64:
        return value->value.i64 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return value->value.f32 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return value->value.f64 < 0 ? 0ULL : static_cast<unsigned long long>(value->value.f64);
    default:
        return 0ULL;
    }
}

float bpValueToFloat(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0.0f;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<float>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return static_cast<float>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<float>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return static_cast<float>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<float>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return static_cast<float>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<float>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return static_cast<float>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return value->value.f32;
    case BP_VAL_SUBTYPE_FLOAT64:
        return static_cast<float>(value->value.f64);
    default:
        return 0.0f;
    }
}

double bpValueToDouble(const BpValue *value) {
    if (!value || !bpValueIsNum(value))
        return 0.0;

    switch (bpValueGetSubtype(value)) {
    case BP_VAL_SUBTYPE_UINT8:
        return static_cast<double>(value->value.u8);
    case BP_VAL_SUBTYPE_INT8:
        return static_cast<double>(value->value.i8);
    case BP_VAL_SUBTYPE_UINT16:
        return static_cast<double>(value->value.u16);
    case BP_VAL_SUBTYPE_INT16:
        return static_cast<double>(value->value.i16);
    case BP_VAL_SUBTYPE_UINT32:
        return static_cast<double>(value->value.u32);
    case BP_VAL_SUBTYPE_INT32:
        return static_cast<double>(value->value.i32);
    case BP_VAL_SUBTYPE_UINT64:
        return static_cast<double>(value->value.u64);
    case BP_VAL_SUBTYPE_INT64:
        return static_cast<double>(value->value.i64);
    case BP_VAL_SUBTYPE_FLOAT32:
        return static_cast<double>(value->value.f32);
    case BP_VAL_SUBTYPE_FLOAT64:
        return value->value.f64;
    default:
        return 0.0;
    }
}

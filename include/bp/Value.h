#ifndef BP_VALUE_H
#define BP_VALUE_H

#include "Defines.h"

BP_BEGIN_CDECLS

/** Type of value (3 bit). */
typedef uint8_t BpValueType;

#define BP_VAL_TYPE_NONE        ((uint8_t)0)        /* _____000 */
#define BP_VAL_TYPE_BOOL        ((uint8_t)1)        /* _____001 */
#define BP_VAL_TYPE_CHAR        ((uint8_t)2)        /* _____010 */
#define BP_VAL_TYPE_NUM         ((uint8_t)3)        /* _____011 */
#define BP_VAL_TYPE_STR         ((uint8_t)4)        /* _____100 */
#define BP_VAL_TYPE_BUF         ((uint8_t)5)        /* _____101 */
#define BP_VAL_TYPE_PTR         ((uint8_t)6)        /* _____110 */

/** Subtype of value (4 bit). */
typedef uint8_t BpValueSubtype;

#define BP_VAL_SUBTYPE_NONE     ((uint8_t)(0 << 3)) /* _0000___ */
#define BP_VAL_SUBTYPE_UINT8    ((uint8_t)(0 << 3)) /* _0000___ */
#define BP_VAL_SUBTYPE_INT8     ((uint8_t)(1 << 3)) /* _0001___ */
#define BP_VAL_SUBTYPE_UINT16   ((uint8_t)(2 << 3)) /* _0010___ */
#define BP_VAL_SUBTYPE_INT16    ((uint8_t)(3 << 3)) /* _0011___ */
#define BP_VAL_SUBTYPE_UINT32   ((uint8_t)(4 << 3)) /* _0100___ */
#define BP_VAL_SUBTYPE_INT32    ((uint8_t)(5 << 3)) /* _0101___ */
#define BP_VAL_SUBTYPE_UINT64   ((uint8_t)(6 << 3)) /* _0110___ */
#define BP_VAL_SUBTYPE_INT64    ((uint8_t)(7 << 3)) /* _0111___ */
#define BP_VAL_SUBTYPE_FLOAT32  ((uint8_t)(8 << 3)) /* _1000___ */
#define BP_VAL_SUBTYPE_FLOAT64  ((uint8_t)(9 << 3)) /* _1001___ */

/** Mask and bits of Variant value tag. */
#define BP_VAL_TYPE_MASK        ((uint8_t)0x07)     /* _____111 */
#define BP_VAL_TYPE_BIT         ((uint8_t)3)
#define BP_VAL_SUBTYPE_MASK     ((uint8_t)0x78)     /* _1111___ */
#define BP_VAL_SUBTYPE_BIT      ((uint8_t)4)
#define BP_VAL_RESERVED_MASK    ((uint8_t)0x80)     /* 1_______ */
#define BP_VAL_RESERVED_BIT     ((uint8_t)1)
#define BP_VAL_TAG_MASK         ((uint8_t)0xFF)     /* 11111111 */
#define BP_VAL_TAG_BIT          ((uint8_t)8)

typedef union BpVal {
    bool        b;
    char        c;
    uint8_t     u8;
    int8_t      i8;
    uint16_t    u16;
    int16_t     i16;
    uint32_t    u32;
    int32_t     i32;
    uint64_t    u64;
    int64_t     i64;
    float       f32;
    double      f64;
    char       *str;
    void       *buf;
    void       *ptr;
    uint8_t     raw[8];
} BpVal;

typedef struct BpValue BpValue;

/**
 * @brief Initialize the value.
 * @param value The value.
 */
BP_EXPORT void bpValueInit(BpValue *value);

/**
 * @brief Clear the value.
 * @param value The value.
 */
BP_EXPORT void bpValueClear(BpValue *value);

/**
 * @brief Copy the value.
 * @param dest The destination value.
 * @param src The source value.
 */
BP_EXPORT void bpValueCopy(BpValue *dest, const BpValue *src);

/**
 * @brief Move the value.
 * @param dest The destination value.
 * @param src The source value.
 */
BP_EXPORT void bpValueMove(BpValue *dest, BpValue *src);

/**
 * @brief Swap the values.
 * @param lhs The left-hand side value.
 * @param rhs The right-hand side value.
 */
BP_EXPORT void bpValueSwap(BpValue *lhs, BpValue *rhs);

/**
 * @brief Get the size of the value.
 * @param value The value.
 * @return The size of the value.
 */
BP_EXPORT size_t bpValueGetSize(const BpValue *value);

/**
 * @brief Get the tag of the value.
 * @param value The value.
 * @return The tag of the value.
 */
BP_EXPORT uint8_t bpValueGetTag(const BpValue *value);

/**
 * @brief Set the tag of the value.
 * @param value The value.
 * @param tag The tag of the value.
 */
BP_EXPORT void bpValueSetTag(BpValue *value, uint8_t tag);

/**
 * @brief Get the type of the value.
 * @param value The value.
 * @return The type of the value.
 */
BP_EXPORT BpValueType bpValueGetType(const BpValue *value);

/**
 * @brief Set the type of the value.
 * @param value The value.
 * @param type The type of the value.
 * @param subtype The subtype of the value.
 */
BP_EXPORT void bpValueSetType(BpValue *value, BpValueType type, BpValueSubtype subtype);

/**
 * @brief Get the subtype of the value.
 * @param value The value.
 * @return The subtype of the value.
 */
BP_EXPORT BpValueSubtype bpValueGetSubtype(const BpValue *value);

/**
 * @brief Set the subtype of the value.
 * @param value The value.
 * @param subtype The subtype of the value.
 */
BP_EXPORT void bpValueSetSubtype(BpValue *value, BpValueSubtype subtype);

/**
 * @brief Check if the value is none.
 * @param value The value.
 * @return True if the value is none, false otherwise.
 */
BP_EXPORT bool bpValueIsNone(const BpValue *value);

/**
 * @brief Check if the value is a boolean.
 * @param value The value.
 * @return True if the value is a boolean, false otherwise.
 */
BP_EXPORT bool bpValueIsBool(const BpValue *value);

/**
 * @brief Check if the value is a character.
 * @param value The value.
 * @return True if the value is a character, false otherwise.
 */
BP_EXPORT bool bpValueIsChar(const BpValue *value);

/**
 * @brief Check if the value is a number.
 * @param value The value.
 * @return True if the value is a number, false otherwise.
 */
BP_EXPORT bool bpValueIsNum(const BpValue *value);

/**
 * @brief Check if the value is a string.
 * @param value The value.
 * @return True if the value is a string, false otherwise.
 */
BP_EXPORT bool bpValueIsUint8(const BpValue *value);

/**
 * @brief Check if the value is a signed 8-bit integer.
 * @param value The value.
 * @return True if the value is a signed 8-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsInt8(const BpValue *value);

/**
 * @brief Check if the value is an unsigned 16-bit integer.
 * @param value The value.
 * @return True if the value is an unsigned 16-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsUint16(const BpValue *value);

/**
 * @brief Check if the value is a signed 16-bit integer.
 * @param value The value.
 * @return True if the value is a signed 16-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsInt16(const BpValue *value);

/**
 * @brief Check if the value is an unsigned 32-bit integer.
 * @param value The value.
 * @return True if the value is an unsigned 32-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsUint32(const BpValue *value);

/**
 * @brief Check if the value is a signed 32-bit integer.
 * @param value The value.
 * @return True if the value is a signed 32-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsInt32(const BpValue *value);

/**
 * @brief Check if the value is an unsigned 64-bit integer.
 * @param value The value.
 * @return True if the value is an unsigned 64-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsUint64(const BpValue *value);

/**
 * @brief Check if the value is a signed 64-bit integer.
 * @param value The value.
 * @return True if the value is a signed 64-bit integer, false otherwise.
 */
BP_EXPORT bool bpValueIsInt64(const BpValue *value);

/**
 * @brief Check if the value is a 32-bit floating-point number.
 * @param value The value.
 * @return True if the value is a 32-bit floating-point number, false otherwise.
 */
BP_EXPORT bool bpValueIsFloat32(const BpValue *value);

/**
 * @brief Check if the value is a 64-bit floating-point number.
 * @param value The value.
 * @return True if the value is a 64-bit floating-point number, false otherwise.
 */
BP_EXPORT bool bpValueIsFloat64(const BpValue *value);

/**
 * @brief Check if the value is a string.
 * @param value The value.
 * @return True if the value is a string, false otherwise.
 */
BP_EXPORT bool bpValueIsString(const BpValue *value);

/**
 * @brief Check if the value is a buffer.
 * @param value The value.
 * @return True if the value is a buffer, false otherwise.
 */
BP_EXPORT bool bpValueIsBuffer(const BpValue *value);

/**
 * @brief Check if the value is a pointer.
 * @param value The value.
 * @return True if the value is a pointer, false otherwise.
 */
BP_EXPORT bool bpValueIsPointer(const BpValue *value);

/**
 * @brief Check if the value is equal to another value.
 * @param lhs The left-hand side value.
 * @param rhs The right-hand side value.
 * @return True if the values are equal, false otherwise.
 */
BP_EXPORT bool bpValueEquals(const BpValue *lhs, const BpValue *rhs);

/**
 * @brief Check if the value is equal to a boolean value.
 * @param value The value.
 * @param val The boolean value.
 * @return True if the value is equal to the boolean value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsBool(const BpValue *value, bool val);

/**
 * @brief Check if the value is equal to a character value.
 * @param value The value.
 * @param val The character value.
 * @return True if the value is equal to the character value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsChar(const BpValue *value, char val);

/**
 * @brief Check if the value is equal to an unsigned 8-bit integer value.
 * @param value The value.
 * @param val The unsigned 8-bit integer value.
 * @return True if the value is equal to the unsigned 8-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsUint8(const BpValue *value, uint8_t val);

/**
 * @brief Check if the value is equal to a signed 8-bit integer value.
 * @param value The value.
 * @param val The signed 8-bit integer value.
 * @return True if the value is equal to the signed 8-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsInt8(const BpValue *value, int8_t val);

/**
 * @brief Check if the value is equal to an unsigned 16-bit integer value.
 * @param value The value.
 * @param val The unsigned 16-bit integer value.
 * @return True if the value is equal to the unsigned 16-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsUint16(const BpValue *value, uint16_t val);

/**
 * @brief Check if the value is equal to a signed 16-bit integer value.
 * @param value The value.
 * @param val The signed 16-bit integer value.
 * @return True if the value is equal to the signed 16-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsInt16(const BpValue *value, int16_t val);

/**
 * @brief Check if the value is equal to an unsigned 32-bit integer value.
 * @param value The value.
 * @param val The unsigned 32-bit integer value.
 * @return True if the value is equal to the unsigned 32-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsUint32(const BpValue *value, uint32_t val);

/**
 * @brief Check if the value is equal to a signed 32-bit integer value.
 * @param value The value.
 * @param val The signed 32-bit integer value.
 * @return True if the value is equal to the signed 32-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsInt32(const BpValue *value, int32_t val);

/**
 * @brief Check if the value is equal to an unsigned 64-bit integer value.
 * @param value The value.
 * @param val The unsigned 64-bit integer value.
 * @return True if the value is equal to the unsigned 64-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsUint64(const BpValue *value, uint64_t val);

/**
 * @brief Check if the value is equal to a signed 64-bit integer value.
 * @param value The value.
 * @param val The signed 64-bit integer value.
 * @return True if the value is equal to the signed 64-bit integer value, false otherwise.
 */
BP_EXPORT bool bpValueEqualsInt64(const BpValue *value, int64_t val);

/**
 * @brief Check if the value is equal to a 32-bit floating-point number.
 * @param value The value.
 * @param val The 32-bit floating-point number.
 * @return True if the value is equal to the 32-bit floating-point number, false otherwise.
 */
BP_EXPORT bool bpValueEqualsFloat32(const BpValue *value, float val);

/**
 * @brief Check if the value is equal to a 64-bit floating-point number.
 * @param value The value.
 * @param val The 64-bit floating-point number.
 * @return True if the value is equal to the 64-bit floating-point number, false otherwise.
 */
BP_EXPORT bool bpValueEqualsFloat64(const BpValue *value, double val);

/**
 * @brief Check if the value is equal to a string.
 * @param value The value.
 * @param val The string.
 * @return True if the value is equal to the string, false otherwise.
 */
BP_EXPORT bool bpValueEqualsString(const BpValue *value, const char *val);

/**
 * @brief Check if the value is equal to a buffer.
 * @param value The value.
 * @param buf The buffer.
 * @param size The size of the buffer.
 * @return True if the value is equal to the buffer, false otherwise.
 */
BP_EXPORT bool bpValueEqualsBuffer(const BpValue *value, const void *buf, size_t size);

/**
 * @brief Check if the value is equal to a pointer.
 * @param value The value.
 * @param ptr The pointer.
 * @param size The size of the pointer.
 * @return True if the value is equal to the pointer, false otherwise.
 */
BP_EXPORT bool bpValueEqualsPointer(const BpValue *value, void *ptr, size_t size);

/**
 * @brief Get the boolean value.
 * @param value The value.
 * @return The boolean value.
 */
BP_EXPORT bool bpValueGetBool(const BpValue *value);

/**
 * @brief Get the character value.
 * @param value The value.
 * @return The character value.
 */
BP_EXPORT char bpValueGetChar(const BpValue *value);

/**
 * @brief Get the unsigned 8-bit integer value.
 * @param value The value.
 * @return The unsigned 8-bit integer value.
 */
BP_EXPORT uint8_t bpValueGetUint8(const BpValue *value);

/**
 * @brief Get the signed 8-bit integer value.
 * @param value The value.
 * @return The signed 8-bit integer value.
 */
BP_EXPORT int8_t bpValueGetInt8(const BpValue *value);

/**
 * @brief Get the unsigned 16-bit integer value.
 * @param value The value.
 * @return The unsigned 16-bit integer value.
 */
BP_EXPORT uint16_t bpValueGetUint16(const BpValue *value);

/**
 * @brief Get the signed 16-bit integer value.
 * @param value The value.
 * @return The signed 16-bit integer value.
 */
BP_EXPORT int16_t bpValueGetInt16(const BpValue *value);

/**
 * @brief Get the unsigned 32-bit integer value.
 * @param value The value.
 * @return The unsigned 32-bit integer value.
 */
BP_EXPORT uint32_t bpValueGetUint32(const BpValue *value);

/**
 * @brief Get the signed 32-bit integer value.
 * @param value The value.
 * @return The signed 32-bit integer value.
 */
BP_EXPORT int32_t bpValueGetInt32(const BpValue *value);

/**
 * @brief Get the unsigned 64-bit integer value.
 * @param value The value.
 * @return The unsigned 64-bit integer value.
 */
BP_EXPORT uint64_t bpValueGetUint64(const BpValue *value);

/**
 * @brief Get the signed 64-bit integer value.
 * @param value The value.
 * @return The signed 64-bit integer value.
 */
BP_EXPORT int64_t bpValueGetInt64(const BpValue *value);

/**
 * @brief Get the 32-bit floating-point number value.
 * @param value The value.
 * @return The 32-bit floating-point number value.
 */
BP_EXPORT float bpValueGetFloat32(const BpValue *value);

/**
 * @brief Get the 64-bit floating-point number value.
 * @param value The value.
 * @return The 64-bit floating-point number value.
 */
BP_EXPORT double bpValueGetFloat64(const BpValue *value);

/**
 * @brief Get the string value.
 * @param value The value.
 * @return The string value.
 */
BP_EXPORT const char *bpValueGetString(const BpValue *value);

/**
 * @brief Get the buffer value.
 * @param value The value.
 * @return The buffer value.
 */
BP_EXPORT const void *bpValueGetBuffer(const BpValue *value);

/**
 * @brief Get the pointer value.
 * @param value The value.
 * @return The pointer value.
 */
BP_EXPORT void *bpValueGetPointer(const BpValue *value);

/**
 * @brief Set the boolean value.
 * @param value The value.
 * @param val The boolean value.
 */
BP_EXPORT void bpValueSetBool(BpValue *value, bool val);

/**
 * @brief Set the character value.
 * @param value The value.
 * @param val The character value.
 */
BP_EXPORT void bpValueSetChar(BpValue *value, char val);

/**
 * @brief Set the unsigned 8-bit integer value.
 * @param value The value.
 * @param val The unsigned 8-bit integer value.
 */
BP_EXPORT void bpValueSetUint8(BpValue *value, uint8_t val);

/**
 * @brief Set the signed 8-bit integer value.
 * @param value The value.
 * @param val The signed 8-bit integer value.
 */
BP_EXPORT void bpValueSetInt8(BpValue *value, int8_t val);

/**
 * @brief Set the unsigned 16-bit integer value.
 * @param value The value.
 * @param val The unsigned 16-bit integer value.
 */
BP_EXPORT void bpValueSetUint16(BpValue *value, uint16_t val);

/**
 * @brief Set the signed 16-bit integer value.
 * @param value The value.
 * @param val The signed 16-bit integer value.
 */
BP_EXPORT void bpValueSetInt16(BpValue *value, int16_t val);

/**
 * @brief Set the unsigned 32-bit integer value.
 * @param value The value.
 * @param val The unsigned 32-bit integer value.
 */
BP_EXPORT void bpValueSetUint32(BpValue *value, uint32_t val);

/**
 * @brief Set the signed 32-bit integer value.
 * @param value The value.
 * @param val The signed 32-bit integer value.
 */
BP_EXPORT void bpValueSetInt32(BpValue *value, int32_t val);

/**
 * @brief Set the unsigned 64-bit integer value.
 * @param value The value.
 * @param val The unsigned 64-bit integer value.
 */
BP_EXPORT void bpValueSetUint64(BpValue *value, uint64_t val);

/**
 * @brief Set the signed 64-bit integer value.
 * @param value The value.
 * @param val The signed 64-bit integer value.
 */
BP_EXPORT void bpValueSetInt64(BpValue *value, int64_t val);

/**
 * @brief Set the 32-bit floating-point number value.
 * @param value The value.
 * @param val The 32-bit floating-point number value.
 */
BP_EXPORT void bpValueSetFloat32(BpValue *value, float val);

/**
 * @brief Set the 64-bit floating-point number value.
 * @param value The value.
 * @param val The 64-bit floating-point number value.
 */
BP_EXPORT void bpValueSetFloat64(BpValue *value, double val);

/**
 * @brief Set the string value.
 * @param value The value.
 * @param str The string value.
 */
BP_EXPORT void bpValueSetString(BpValue *value, const char *str);

/**
 * @brief Set the string value.
 * @param value The value.
 * @param buf The buffer value.
 * @param size The size of the buffer.
 */
BP_EXPORT void bpValueSetStringN(BpValue *value, const char *str, size_t size);

/**
 * @brief Set the format string value.
 * @param value The value.
 * @param fmt The format string.
 * @param args The variable arguments.
 */
BP_EXPORT void bpValueSetStringV(BpValue *value, const char *fmt, va_list args);

/**
 * @brief Set the format string value.
 * @param value The value.
 * @param fmt The format string.
 * @param ... The variable arguments.
 */
BP_EXPORT void bpValueSetStringF(BpValue *value, const char *fmt, ...);

/**
 * @brief Set the buffer value.
 * @param value The value.
 * @param buf The buffer value.
 * @param size The size of the buffer.
 */
BP_EXPORT void bpValueSetBuffer(BpValue *value, const void *buf, size_t size);

/**
 * @brief Set the pointer value.
 * @param value The value.
 * @param ptr The pointer value.
 * @param size The size of the pointer.
 */
BP_EXPORT void bpValueSetPointer(BpValue *value, void *ptr, size_t size);

/**
 * @brief Get the boolean value as an integer.
 * @param value The value.
 * @return The boolean value as an integer.
 */
BP_EXPORT int bpValueToInt(const BpValue *value);

/**
 * @brief Get the character value as an integer.
 * @param value The value.
 * @return The character value as an integer.
 */
BP_EXPORT long bpValueToLong(const BpValue *value);

/**
 * @brief Get the character value as an integer.
 * @param value The value.
 * @return The character value as an integer.
 */
BP_EXPORT long long bpValueToLongLong(const BpValue *value);

/**
 * @brief Get the unsigned 8-bit integer value as an integer.
 * @param value The value.
 * @return The unsigned 8-bit integer value as an integer.
 */
BP_EXPORT unsigned int bpValueToUint(const BpValue *value);

/**
 * @brief Get the signed 8-bit integer value as an integer.
 * @param value The value.
 * @return The signed 8-bit integer value as an integer.
 */
BP_EXPORT unsigned long bpValueToUlong(const BpValue *value);

/**
 * @brief Get the signed 8-bit integer value as an integer.
 * @param value The value.
 * @return The signed 8-bit integer value as an integer.
 */
BP_EXPORT unsigned long long bpValueToUlongLong(const BpValue *value);

/**
 * @brief Get the unsigned 16-bit integer value as an integer.
 * @param value The value.
 * @return The unsigned 16-bit integer value as an integer.
 */
BP_EXPORT float bpValueToFloat(const BpValue *value);

/**
 * @brief Get the signed 16-bit integer value as an integer.
 * @param value The value.
 * @return The signed 16-bit integer value as an integer.
 */
BP_EXPORT double bpValueToDouble(const BpValue *value);

BP_END_CDECLS

struct BpValue {
#ifndef __cplusplus
    BpVal value;
    size_t size;
    uint8_t tag;
#else
    BpVal value = {};
    size_t size = 0;
    uint8_t tag = BP_VAL_TYPE_NONE;

    // Constructors
    BpValue() = default;
    ~BpValue() { bpValueClear(this); }

    // Constructors for various data types
    BpValue(bool val) { bpValueSetBool(this, val); }
    BpValue(char val) { bpValueSetChar(this, val); }
    BpValue(uint8_t val) { bpValueSetUint8(this, val); }
    BpValue(int8_t val) { bpValueSetInt8(this, val); }
    BpValue(uint16_t val) { bpValueSetUint16(this, val); }
    BpValue(int16_t val) { bpValueSetInt16(this, val); }
    BpValue(uint32_t val) { bpValueSetUint32(this, val); }
    BpValue(int32_t val) { bpValueSetInt32(this, val); }
    BpValue(uint64_t val) { bpValueSetUint64(this, val); }
    BpValue(int64_t val) { bpValueSetInt64(this, val); }
    BpValue(float val) { bpValueSetFloat32(this, val); }
    BpValue(double val) { bpValueSetFloat64(this, val); }
    BpValue(const char *val) { bpValueSetString(this, val); }

    // Create a buffer value
    BpValue(const void *buf, size_t sz) {
        bpValueSetBuffer(this, buf, sz);
    }

    // Create a pointer value
    BpValue(void *ptr, size_t sz = sizeof(void *)) {
        bpValueSetPointer(this, ptr, sz);
    }

    // Copy constructor
    BpValue(const BpValue &other) {
        bpValueCopy(this, &other);
    }

    // Move constructor
    BpValue(BpValue &&other) noexcept {
        value = {};
        size = 0;
        tag = BP_VAL_TYPE_NONE;
        bpValueMove(this, &other);
    }

    // Copy assignment operator
    BpValue &operator=(const BpValue &other) {
        if (this != &other) bpValueCopy(this, &other);
        return *this;
    }

    // Move assignment operator
    BpValue &operator=(BpValue &&other) noexcept {
        if (this != &other) bpValueMove(this, &other);
        return *this;
    }

    // Assignment operators for basic types
    BpValue &operator=(bool val) {
        bpValueSetBool(this, val);
        return *this;
    }

    BpValue &operator=(char val) {
        bpValueSetChar(this, val);
        return *this;
    }

    BpValue &operator=(uint8_t val) {
        bpValueSetUint8(this, val);
        return *this;
    }

    BpValue &operator=(int8_t val) {
        bpValueSetInt8(this, val);
        return *this;
    }

    BpValue &operator=(uint16_t val) {
        bpValueSetUint16(this, val);
        return *this;
    }

    BpValue &operator=(int16_t val) {
        bpValueSetInt16(this, val);
        return *this;
    }

    BpValue &operator=(uint32_t val) {
        bpValueSetUint32(this, val);
        return *this;
    }

    BpValue &operator=(int32_t val) {
        bpValueSetInt32(this, val);
        return *this;
    }

    BpValue &operator=(uint64_t val) {
        bpValueSetUint64(this, val);
        return *this;
    }

    BpValue &operator=(int64_t val) {
        bpValueSetInt64(this, val);
        return *this;
    }

    BpValue &operator=(long val) {
        bpValueSetInt32(this, val);
        return *this;
    }

    BpValue &operator=(unsigned long val) {
        bpValueSetUint32(this, val);
        return *this;
    }

    BpValue &operator=(float val) {
        bpValueSetFloat32(this, val);
        return *this;
    }

    BpValue &operator=(double val) {
        bpValueSetFloat64(this, val);
        return *this;
    }

    BpValue &operator=(const char *val) {
        bpValueSetString(this, val);
        return *this;
    }

    BpValue &operator=(void *val) {
        bpValueSetPointer(this, val, sizeof(void *));
        return *this;
    }

    // Equality operators
    bool operator==(const BpValue &other) const {
        return bpValueEquals(this, &other);
    }

    bool operator!=(const BpValue &other) const {
        return !bpValueEquals(this, &other);
    }

    bool operator==(bool val) const {
        return bpValueEqualsBool(this, val);
    }

    bool operator!=(bool val) const {
        return !bpValueEqualsBool(this, val);
    }

    bool operator==(char val) const {
        return bpValueEqualsChar(this, val);
    }

    bool operator!=(char val) const {
        return !bpValueEqualsChar(this, val);
    }

    bool operator==(uint8_t val) const {
        return bpValueEqualsUint8(this, val);
    }

    bool operator!=(uint8_t val) const {
        return !bpValueEqualsUint8(this, val);
    }

    bool operator==(int8_t val) const {
        return bpValueEqualsInt8(this, val);
    }

    bool operator!=(int8_t val) const {
        return !bpValueEqualsInt8(this, val);
    }

    bool operator==(uint16_t val) const {
        return bpValueEqualsUint16(this, val);
    }

    bool operator!=(uint16_t val) const {
        return !bpValueEqualsUint16(this, val);
    }

    bool operator==(int16_t val) const {
        return bpValueEqualsInt16(this, val);
    }

    bool operator!=(int16_t val) const {
        return !bpValueEqualsInt16(this, val);
    }

    bool operator==(uint32_t val) const {
        return bpValueEqualsUint32(this, val);
    }

    bool operator!=(uint32_t val) const {
        return !bpValueEqualsUint32(this, val);
    }

    bool operator==(int32_t val) const {
        return bpValueEqualsInt32(this, val);
    }

    bool operator!=(int32_t val) const {
        return !bpValueEqualsInt32(this, val);
    }

    bool operator==(uint64_t val) const {
        return bpValueEqualsUint64(this, val);
    }

    bool operator!=(uint64_t val) const {
        return !bpValueEqualsUint64(this, val);
    }

    bool operator==(int64_t val) const {
        return bpValueEqualsInt64(this, val);
    }

    bool operator!=(int64_t val) const {
        return !bpValueEqualsInt64(this, val);
    }

    bool operator==(float val) const {
        return bpValueEqualsFloat32(this, val);
    }

    bool operator!=(float val) const {
        return !bpValueEqualsFloat32(this, val);
    }

    bool operator==(double val) const {
        return bpValueEqualsFloat64(this, val);
    }

    bool operator!=(double val) const {
        return !bpValueEqualsFloat64(this, val);
    }

    bool operator==(const char *val) const {
        return bpValueEqualsString(this, val);
    }

    bool operator!=(const char *val) const {
        return !bpValueEqualsString(this, val);
    }

    bool operator==(void *val) const {
        return bpValueEqualsPointer(this, val, sizeof(void *));
    }

    bool operator!=(void *val) const {
        return !bpValueEqualsPointer(this, val, sizeof(void *));
    }

    // Type checking
    bool IsNone() const { return bpValueIsNone(this); }
    bool IsBool() const { return bpValueIsBool(this); }
    bool IsChar() const { return bpValueIsChar(this); }
    bool IsNum() const { return bpValueIsNum(this); }
    bool IsString() const { return bpValueIsString(this); }
    bool IsBuffer() const { return bpValueIsBuffer(this); }
    bool IsPointer() const { return bpValueIsPointer(this); }

    // Numeric type checking
    bool IsUint8() const { return bpValueIsUint8(this); }
    bool IsInt8() const { return bpValueIsInt8(this); }
    bool IsUint16() const { return bpValueIsUint16(this); }
    bool IsInt16() const { return bpValueIsInt16(this); }
    bool IsUint32() const { return bpValueIsUint32(this); }
    bool IsInt32() const { return bpValueIsInt32(this); }
    bool IsUint64() const { return bpValueIsUint64(this); }
    bool IsInt64() const { return bpValueIsInt64(this); }
    bool IsFloat32() const { return bpValueIsFloat32(this); }
    bool IsFloat64() const { return bpValueIsFloat64(this); }

    // Value accessors with explicit type checking
    bool GetBool() const {
        return bpValueGetBool(this);
    }

    char GetChar() const {
        return bpValueGetChar(this);
    }

    uint8_t GetUint8() const {
        return bpValueGetUint8(this);
    }

    int8_t GetInt8() const {
        return bpValueGetInt8(this);
    }

    uint16_t GetUint16() const {
        return bpValueGetUint16(this);
    }

    int16_t GetInt16() const {
        return bpValueGetInt16(this);
    }

    uint32_t GetUint32() const {
        return bpValueGetUint32(this);
    }

    int32_t GetInt32() const {
        return bpValueGetInt32(this);
    }

    uint64_t GetUint64() const {
        return bpValueGetUint64(this);
    }

    int64_t GetInt64() const {
        return bpValueGetInt64(this);
    }

    float GetFloat32() const {
        return bpValueGetFloat32(this);
    }

    double GetFloat64() const {
        return bpValueGetFloat64(this);
    }

    const char *GetString() const {
        return bpValueGetString(this);
    }

    const void *GetBuffer() const {
        return bpValueGetBuffer(this);
    }

    void *GetPointer() const {
        return bpValueGetPointer(this);
    }

    // Value accessors with flexible type conversion and default values
    bool AsBool(bool defaultVal = false) const {
        return IsBool() ? GetBool() : defaultVal;
    }

    char AsChar(char defaultVal = '\0') const {
        return IsChar() ? GetChar() : defaultVal;
    }

    uint8_t AsUint8(uint8_t defaultVal = 0) const {
        return IsUint8() ? GetUint8() : defaultVal;
    }

    int8_t AsInt8(int8_t defaultVal = 0) const {
        return IsInt8() ? GetInt8() : defaultVal;
    }

    uint16_t AsUint16(uint16_t defaultVal = 0) const {
        return IsUint16() ? GetUint16() : defaultVal;
    }

    int16_t AsInt16(int16_t defaultVal = 0) const {
        return IsInt16() ? GetInt16() : defaultVal;
    }

    uint32_t AsUint32(uint32_t defaultVal = 0) const {
        return IsUint32() ? GetUint32() : defaultVal;
    }

    int32_t AsInt32(int32_t defaultVal = 0) const {
        return IsInt32() ? GetInt32() : defaultVal;
    }

    uint64_t AsUint64(uint64_t defaultVal = 0) const {
        return IsUint64() ? GetUint64() : defaultVal;
    }

    int64_t AsInt64(int64_t defaultVal = 0) const {
        return IsInt64() ? GetInt64() : defaultVal;
    }

    int AsInt(int defaultVal = 0) const {
        return IsNum() ? bpValueToInt(this) : defaultVal;
    }

    float AsFloat(float defaultVal = 0.0f) const {
        return IsNum() ? bpValueToFloat(this) : defaultVal;
    }

    double AsDouble(double defaultVal = 0.0) const {
        return IsNum() ? bpValueToDouble(this) : defaultVal;
    }

    const char *AsString(const char *defaultVal = "") const {
        const char *str = GetString();
        return (IsString() && str) ? str : defaultVal;
    }

    const void *AsBuffer(const void *defaultVal = nullptr) const {
        const void *buf = GetBuffer();
        return (IsBuffer() && buf) ? buf : defaultVal;
    }

    void *AsPointer(void *defaultVal = nullptr) const {
        void *ptr = GetPointer();
        return (IsPointer() && ptr) ? ptr : defaultVal;
    }

    template <typename T>
    void SetValue(BpValue *value, T val, BpValueType type, BpValueSubtype subtype) {
        if (!value)
            return;

        bpValueClear(value);
        memcpy(&value->value, &val, sizeof(T));
        value->size = sizeof(T);
        bpValueSetType(value, type, subtype);
    }

    // Specializations for common types with correct type tags
    template <>
    void SetValue<bool>(BpValue *value, bool val, BpValueType, BpValueSubtype) {
        bpValueSetBool(value, val);
    }

    template <>
    void SetValue<uint8_t>(BpValue *value, uint8_t val, BpValueType, BpValueSubtype) {
        bpValueSetUint8(value, val);
    }

    // Buffer management
    void SetBuffer(const void *buf, size_t sz) {
        bpValueSetBuffer(this, buf, sz);
    }

    // Type information management
    uint8_t GetTag() const { return bpValueGetTag(this); }
    BpValueType GetType() const { return bpValueGetType(this); }
    BpValueSubtype GetSubtype() const { return bpValueGetSubtype(this); }

    void SetTag(uint8_t newTag) { bpValueSetTag(this, newTag); }
    void SetType(BpValueType type, BpValueSubtype subtype) { bpValueSetType(this, type, subtype); }

    // Size information
    size_t GetSize() const { return bpValueGetSize(this); }

    // Clear the value
    void Clear() { bpValueClear(this); }
#endif
};

#endif // BP_VALUE_H

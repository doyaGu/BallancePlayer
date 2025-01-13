#ifndef BP_VARIANT_H
#define BP_VARIANT_H

#include <cstdint>
#include <cmath>
#include <limits>

/** Type of Variant value (3 bit). */
typedef uint8_t VariantType;

#define VAR_TYPE_NONE        ((uint8_t)0)        /* _____000 */
#define VAR_TYPE_BOOL        ((uint8_t)1)        /* _____001 */
#define VAR_TYPE_CHAR        ((uint8_t)2)        /* _____010 */
#define VAR_TYPE_NUM         ((uint8_t)3)        /* _____011 */
#define VAR_TYPE_STR         ((uint8_t)4)        /* _____100 */
#define VAR_TYPE_BUF         ((uint8_t)5)        /* _____101 */
#define VAR_TYPE_PTR         ((uint8_t)6)        /* _____110 */

/** Subtype of Variant value (4 bit). */
typedef uint8_t VariantSubtype;

#define VAR_SUBTYPE_NONE     ((uint8_t)(0 << 3)) /* _0000___ */
#define VAR_SUBTYPE_UINT8    ((uint8_t)(0 << 3)) /* _0000___ */
#define VAR_SUBTYPE_INT8     ((uint8_t)(1 << 3)) /* _0001___ */
#define VAR_SUBTYPE_UINT16   ((uint8_t)(2 << 3)) /* _0010___ */
#define VAR_SUBTYPE_INT16    ((uint8_t)(3 << 3)) /* _0011___ */
#define VAR_SUBTYPE_UINT32   ((uint8_t)(4 << 3)) /* _0100___ */
#define VAR_SUBTYPE_INT32    ((uint8_t)(5 << 3)) /* _0101___ */
#define VAR_SUBTYPE_UINT64   ((uint8_t)(6 << 3)) /* _0110___ */
#define VAR_SUBTYPE_INT64    ((uint8_t)(7 << 3)) /* _0111___ */
#define VAR_SUBTYPE_FLOAT32  ((uint8_t)(8 << 3)) /* _1000___ */
#define VAR_SUBTYPE_FLOAT64  ((uint8_t)(9 << 3)) /* _1001___ */

/** Mask and bits of Variant value tag. */
#define VAR_TYPE_MASK        ((uint8_t)0x07)     /* _____111 */
#define VAR_TYPE_BIT         ((uint8_t)3)
#define VAR_SUBTYPE_MASK     ((uint8_t)0x78)     /* _1111___ */
#define VAR_SUBTYPE_BIT      ((uint8_t)4)
#define VAR_RESERVED_MASK    ((uint8_t)0x80)     /* 1_______ */
#define VAR_RESERVED_BIT     ((uint8_t)1)
#define VAR_TAG_MASK         ((uint8_t)0xFF)     /* 11111111 */
#define VAR_TAG_BIT          ((uint8_t)8)

constexpr size_t VARIANT_VALUE_SIZE = 4;

typedef union VariantValue {
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
    uint8_t     raw[VARIANT_VALUE_SIZE];
} VariantValue;

class Variant final {
public:
    Variant();
    Variant(bool value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(char value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(uint8_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(int8_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(uint16_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(int16_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(uint32_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(int32_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(uint64_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(int64_t value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(float value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(double value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(const char *value) { *this = value; } // NOLINT(google-explicit-constructor)
    Variant(const void *buf, size_t size);
    Variant(void *ptr, size_t size) : m_Tag(VAR_TYPE_PTR), m_Size(size) {
        m_Value.ptr = ptr;
    }

    Variant(const Variant &rhs);
    Variant(Variant &&rhs) noexcept;

    ~Variant();

    void Clear();

    Variant& operator=(const Variant& rhs);

    Variant &operator=(bool value) {
        if (!IsBool()) {
            Clear();
            SetType(VAR_TYPE_BOOL, VAR_SUBTYPE_NONE);
            m_Size = sizeof(value);
        }
        m_Value.b = value;
        return *this;
    }

    Variant &operator=(char value) {
        if (!IsChar()) {
            Clear();
            SetType(VAR_TYPE_CHAR, VAR_SUBTYPE_NONE);
            m_Size = sizeof(value);
        }
        m_Value.c = value;
        return *this;
    }

    Variant &operator=(uint8_t value) {
        if (!IsUint8()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_UINT8);
            m_Size = sizeof(value);
        }
        m_Value.u8 = value;
        return *this;
    }

    Variant &operator=(int8_t value) {
        if (!IsInt8()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_INT8);
            m_Size = sizeof(value);
        }
        m_Value.i8 = value;
        return *this;
    }

    Variant &operator=(uint16_t value) {
        if (!IsUint16()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_UINT16);
            m_Size = sizeof(value);
        }
        m_Value.u16 = value;
        return *this;
    }

    Variant &operator=(int16_t value) {
        if (!IsInt16()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_INT16);
            m_Size = sizeof(value);
        }
        m_Value.i16 = value;
        return *this;
    }

    Variant &operator=(uint32_t value) {
        if (!IsUint32()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_UINT32);
            m_Size = sizeof(value);
        }
        m_Value.u32 = value;
        return *this;
    }

    Variant &operator=(int32_t value) {
        if (!IsInt32()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_INT32);
            m_Size = sizeof(value);
        }
        m_Value.i32 = value;
        return *this;
    }

    Variant &operator=(uint64_t value) {
        if (!IsUint64()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_UINT64);
            m_Size = sizeof(value);
        }
        m_Value.u64 = value;
        return *this;
    }

    Variant &operator=(int64_t value) {
        if (!IsInt64()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_INT64);
            m_Size = sizeof(value);
        }
        m_Value.i64 = value;
        return *this;
    }

    Variant &operator=(float value) {
        if (!IsFloat64()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_FLOAT32);
            m_Size = sizeof(value);
        }
        m_Value.f32 = value;
        return *this;
    }

    Variant &operator=(double value) {
        if (!IsFloat64()) {
            Clear();
            SetType(VAR_TYPE_NUM, VAR_SUBTYPE_FLOAT64);
            m_Size = sizeof(value);
        }
        m_Value.f64 = value;
        return *this;
    }

    Variant& operator=(const char *value);

    Variant& operator=(void *value) {
            if (!IsPtr()) {
                Clear();
                SetType(VAR_TYPE_PTR, VAR_SUBTYPE_NONE);
                m_Size = sizeof(void *);
            }
            m_Value.ptr = value;
            return *this;
    }

    Variant& operator=(Variant &&rhs) noexcept;

    bool operator==(const Variant &rhs) const;
    bool operator==(bool value) const { return IsBool() && m_Value.b == value; }
    bool operator==(char value) const { return IsChar() && m_Value.c == value; }
    bool operator==(uint8_t value) const { return IsUint8() && m_Value.u8 == value; }
    bool operator==(int8_t value) const { return IsInt8() && m_Value.i8 == value; }
    bool operator==(uint16_t value) const { return IsUint16() && m_Value.u16 == value; }
    bool operator==(int16_t value) const { return IsInt16() && m_Value.i16 == value; }
    bool operator==(uint32_t value) const { return IsUint32() && m_Value.u32 == value; }
    bool operator==(int32_t value) const { return IsInt32() && m_Value.i32 == value; }
    bool operator==(uint64_t value) const { return IsUint64() && m_Value.u64 == value; }
    bool operator==(int64_t value) const { return IsInt64() && m_Value.i64 == value; }
    bool operator==(float value) const { return IsFloat32() && std::fabs(m_Value.f32 - value) < std::numeric_limits<float>::epsilon(); }
    bool operator==(double value) const { return IsFloat64() && std::fabs(m_Value.f64 - value) < std::numeric_limits<double>::epsilon(); }
    bool operator==(const char *value) const;
    bool operator==(void *value) const { return IsPtr() && m_Value.ptr == value; }

    bool operator!=(const Variant &rhs) const { return !(*this == rhs); }
    bool operator!=(bool value) const { return !(*this == value); }
    bool operator!=(char value) const { return !(*this == value); }
    bool operator!=(uint8_t value) const { return !(*this == value); }
    bool operator!=(int8_t value) const { return !(*this == value); }
    bool operator!=(uint16_t value) const { return !(*this == value); }
    bool operator!=(int16_t value) const { return !(*this == value); }
    bool operator!=(uint32_t value) const { return !(*this == value); }
    bool operator!=(int32_t value) const { return !(*this == value); }
    bool operator!=(uint64_t value) const { return !(*this == value); }
    bool operator!=(int64_t value) const { return !(*this == value); }
    bool operator!=(float value) const { return !(*this == value); }
    bool operator!=(double value) const { return !(*this == value); }
    bool operator!=(const char *value) const { return !(*this == value); }
    bool operator!=(void *value) const { return !(*this == value); }

    bool IsNone() const {
        return GetType() == VAR_TYPE_NONE;
    }

    bool IsBool() const {
        return GetType() == VAR_TYPE_BOOL;
    }

    bool IsChar() const {
        return GetType() == VAR_TYPE_CHAR;
    }

    bool IsNum() const {
        return GetType() == VAR_TYPE_NUM;
    }

    bool IsUint8() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_UINT8);
    }

    bool IsInt8() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_INT8);
    }

    bool IsUint16() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_UINT16);
    }

    bool IsInt16() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_INT16);
    }

    bool IsUint32() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_UINT32);
    }

    bool IsInt32() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_INT32);
    }

    bool IsUint64() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_UINT64);
    }

    bool IsInt64() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_INT64);
    }

    bool IsFloat32() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_FLOAT32);
    }

    bool IsFloat64() const {
        return GetTag() == (VAR_TYPE_NUM | VAR_SUBTYPE_FLOAT64);
    }

    bool IsString() const {
        return GetType() == VAR_TYPE_STR;
    }

    bool IsBuffer() const {
        return GetType() == VAR_TYPE_BUF;
    }

    bool IsPtr() const {
        return GetType() == VAR_TYPE_PTR;
    }

    uint8_t GetTag() const {
        return (uint8_t)(((uint8_t)m_Tag) & VAR_TAG_MASK);
    }

    VariantType GetType() const {
        return (VariantType)(((uint8_t)m_Tag) & VAR_TYPE_MASK);
    }

    VariantSubtype GetSubtype() const {
        return (VariantSubtype)(((uint8_t)m_Tag) & VAR_SUBTYPE_MASK);
    }

    void SetTag(uint8_t tag) {
        uint32_t newTag = m_Tag;
        newTag = (newTag & (~(uint32_t)VAR_TAG_MASK)) | (uint32_t)tag;
        m_Tag = newTag;
    }

    void SetType(VariantType type, VariantSubtype subtype) {
        uint8_t tag = (type | subtype);
        uint32_t newTag = m_Tag;
        newTag = (newTag & (~(uint32_t)VAR_TAG_MASK)) | (uint32_t)tag;
        m_Tag = newTag;
    }

    size_t GetSize() const {
        return m_Size;
    }

    VariantValue &GetValue() {
        return m_Value;
    }

    bool GetBool() const {
        return m_Value.b;
    }

    char GetChar() const {
        return m_Value.c;
    }

    uint8_t GetUint8() const {
        return m_Value.u8;
    }

    int8_t GetInt8() const {
        return m_Value.i8;
    }

    uint16_t GetUint16() const {
        return m_Value.u16;
    }

    int16_t GetInt16() const {
        return m_Value.i16;
    }

    uint32_t GetUint32() const {
        return m_Value.u32;
    }

    int32_t GetInt32() const {
        return m_Value.i32;
    }

    uint64_t GetUint64() const {
        return m_Value.u64;
    }

    int64_t GetInt64() const {
        return m_Value.i64;
    }

    float GetFloat32() const {
        if (IsFloat64())
            return (float) m_Value.f64;
        return m_Value.f32;
    }

    double GetFloat64() const {
        if (IsFloat32())
            return m_Value.f32;
        return m_Value.f64;
    }

    const char *GetString() const {
        return (IsString()) ? m_Value.str : nullptr;
    }

    const void *GetBuffer() const {
        return (IsBuffer()) ? m_Value.buf : nullptr;
    }

    void *GetPtr() const {
        return (IsPtr()) ? m_Value.ptr : nullptr;
    }

    void SetBuffer(const void *buf, size_t size);

private:
    uint32_t m_Tag = VAR_TYPE_NONE;
    size_t m_Size = 0;
    VariantValue m_Value = {};
};

#endif // BP_VARIANT_H

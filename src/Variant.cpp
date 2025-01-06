#include "Variant.h"

#include <cstdlib>
#include <cstring>

Variant::Variant() = default;

Variant::Variant(const void *buf, size_t size) {
    if (buf && size != 0) {
        SetType(VAR_TYPE_BUF, VAR_SUBTYPE_NONE);
        m_Size = size;
        m_Value.buf = malloc(size);
        if (m_Value.buf) {
            memcpy(m_Value.buf, buf, m_Size);
            return;
        }
    }
    Clear();
}

Variant::Variant(const Variant &rhs) {
    switch (GetType()) {
        case VAR_TYPE_BOOL:
            *this = rhs.m_Value.b;
            break;
        case VAR_TYPE_CHAR:
            *this = rhs.m_Value.c;
            break;
        case VAR_TYPE_NUM:
            switch (GetSubtype()) {
                case VAR_SUBTYPE_UINT8:
                    *this = rhs.GetUint8();
                    break;
                case VAR_SUBTYPE_INT8:
                    *this = rhs.GetInt8();
                    break;
                case VAR_SUBTYPE_UINT16:
                    *this = rhs.GetUint16();
                    break;
                case VAR_SUBTYPE_INT16:
                    *this = rhs.GetInt16();
                    break;
                case VAR_SUBTYPE_UINT32:
                    *this = rhs.GetUint32();
                    break;
                case VAR_SUBTYPE_INT32:
                    *this = rhs.GetInt32();
                    break;
                case VAR_SUBTYPE_UINT64:
                    *this = rhs.GetUint64();
                    break;
                case VAR_SUBTYPE_INT64:
                    *this = rhs.GetInt64();
                    break;
                case VAR_SUBTYPE_FLOAT32:
                    *this = rhs.GetFloat32();
                    break;
                case VAR_SUBTYPE_FLOAT64:
                    *this = rhs.GetFloat64();
                    break;
                default:
                    break;
            }
        case VAR_TYPE_STR:
            *this = rhs.m_Value.str;
            break;
        case VAR_TYPE_BUF:
            Clear();
            SetType(VAR_TYPE_BUF, VAR_TYPE_NONE);
            m_Size = rhs.m_Size;
            m_Value.buf = (uint8_t *) malloc(rhs.m_Size);
            if (m_Value.buf) {
                memcpy(m_Value.buf, rhs.m_Value.buf, m_Size);
            } else {
                Clear();
            }
            break;
        case VAR_TYPE_PTR:
            m_Size = rhs.m_Size;
            m_Value.ptr = rhs.m_Value.ptr;
            break;
        default:
            break;
    }
}

Variant::Variant(Variant &&rhs) noexcept {
    switch (GetType()) {
        case VAR_TYPE_BOOL:
            *this = rhs.m_Value.b;
            break;
        case VAR_TYPE_CHAR:
            *this = rhs.m_Value.c;
            break;
        case VAR_TYPE_NUM:
            switch (GetSubtype()) {
                case VAR_SUBTYPE_UINT8:
                    *this = rhs.GetUint8();
                    break;
                case VAR_SUBTYPE_INT8:
                    *this = rhs.GetInt8();
                    break;
                case VAR_SUBTYPE_UINT16:
                    *this = rhs.GetUint16();
                    break;
                case VAR_SUBTYPE_INT16:
                    *this = rhs.GetInt16();
                    break;
                case VAR_SUBTYPE_UINT32:
                    *this = rhs.GetUint32();
                    break;
                case VAR_SUBTYPE_INT32:
                    *this = rhs.GetInt32();
                    break;
                case VAR_SUBTYPE_UINT64:
                    *this = rhs.GetUint64();
                    break;
                case VAR_SUBTYPE_INT64:
                    *this = rhs.GetInt64();
                    break;
                case VAR_SUBTYPE_FLOAT32:
                    *this = rhs.GetFloat32();
                    break;
                case VAR_SUBTYPE_FLOAT64:
                    *this = rhs.GetFloat64();
                    break;
                default:
                    break;
            }
        case VAR_TYPE_STR:
            Clear();
            SetType(VAR_TYPE_STR, VAR_TYPE_NONE);
            m_Size = rhs.m_Size;
            m_Value.str = rhs.m_Value.str;
            rhs.SetType(VAR_TYPE_NONE, VAR_TYPE_NONE);
            rhs.m_Size = 0;
            rhs.m_Value.str = nullptr;
            break;
        case VAR_TYPE_BUF:
            Clear();
            SetType(VAR_TYPE_BUF, VAR_TYPE_NONE);
            m_Size = rhs.m_Size;
            m_Value.buf = rhs.m_Value.buf;
            rhs.SetType(VAR_TYPE_NONE, VAR_TYPE_NONE);
            rhs.m_Size = 0;
            rhs.m_Value.buf = nullptr;
            break;
        case VAR_TYPE_PTR:
            m_Size = rhs.m_Size;
            m_Value.ptr = rhs.m_Value.ptr;
            break;
        default:
            break;
    }
}

Variant::~Variant() {
    Clear();
}

Variant &Variant::operator=(const Variant &rhs) {
    if (*this == rhs)
        return *this;

    switch (GetType()) {
        case VAR_TYPE_BOOL:
            *this = rhs.m_Value.b;
            break;
        case VAR_TYPE_CHAR:
            *this = rhs.m_Value.c;
            break;
        case VAR_TYPE_NUM:
            switch (GetSubtype()) {
                case VAR_SUBTYPE_UINT8:
                    *this = rhs.GetUint8();
                    break;
                case VAR_SUBTYPE_INT8:
                    *this = rhs.GetInt8();
                    break;
                case VAR_SUBTYPE_UINT16:
                    *this = rhs.GetUint16();
                    break;
                case VAR_SUBTYPE_INT16:
                    *this = rhs.GetInt16();
                    break;
                case VAR_SUBTYPE_UINT32:
                    *this = rhs.GetUint32();
                    break;
                case VAR_SUBTYPE_INT32:
                    *this = rhs.GetInt32();
                    break;
                case VAR_SUBTYPE_UINT64:
                    *this = rhs.GetUint64();
                    break;
                case VAR_SUBTYPE_INT64:
                    *this = rhs.GetInt64();
                    break;
                case VAR_SUBTYPE_FLOAT32:
                    *this = rhs.GetFloat32();
                    break;
                case VAR_SUBTYPE_FLOAT64:
                    *this = rhs.GetFloat64();
                    break;
                default:
                    break;
            }
        case VAR_TYPE_STR:
            *this = rhs.m_Value.str;
            break;
        case VAR_TYPE_BUF: {
            auto *buf = (uint8_t *) malloc(rhs.m_Size);
            if (buf) {
                Clear();
                SetType(VAR_TYPE_BUF, VAR_SUBTYPE_NONE);
                m_Size = rhs.m_Size;
                m_Value.buf = buf;
                memcpy(m_Value.buf, rhs.m_Value.buf, m_Size);
            }
        }
            break;
        case VAR_TYPE_PTR:
            m_Size = rhs.m_Size;
            m_Value.ptr = rhs.m_Value.ptr;
            break;
        default:
            break;
    }

    return *this;
}

Variant &Variant::operator=(const char *value) {
    if (value) {
        char *str = _strdup(value);
        if (str) {
            Clear();
            SetType(VAR_TYPE_STR, VAR_TYPE_NONE);
            m_Size = strlen(str);
            m_Value.str = str;
        }
    }
    return *this;
}

Variant &Variant::operator=(Variant &&rhs) noexcept {
    if (*this == rhs)
        return *this;

    switch (GetType()) {
        case VAR_TYPE_BOOL:
            *this = rhs.m_Value.b;
            break;
        case VAR_TYPE_CHAR:
            *this = rhs.m_Value.c;
            break;
        case VAR_TYPE_NUM:
            switch (GetSubtype()) {
                case VAR_SUBTYPE_UINT8:
                    *this = rhs.GetUint8();
                    break;
                case VAR_SUBTYPE_INT8:
                    *this = rhs.GetInt8();
                    break;
                case VAR_SUBTYPE_UINT16:
                    *this = rhs.GetUint16();
                    break;
                case VAR_SUBTYPE_INT16:
                    *this = rhs.GetInt16();
                    break;
                case VAR_SUBTYPE_UINT32:
                    *this = rhs.GetUint32();
                    break;
                case VAR_SUBTYPE_INT32:
                    *this = rhs.GetInt32();
                    break;
                case VAR_SUBTYPE_UINT64:
                    *this = rhs.GetUint64();
                    break;
                case VAR_SUBTYPE_INT64:
                    *this = rhs.GetInt64();
                    break;
                case VAR_SUBTYPE_FLOAT32:
                    *this = rhs.GetFloat32();
                    break;
                case VAR_SUBTYPE_FLOAT64:
                    *this = rhs.GetFloat64();
                    break;
                default:
                    break;
            }
        case VAR_TYPE_STR:
            Clear();
            SetType(VAR_TYPE_STR, VAR_SUBTYPE_NONE);
            m_Size = rhs.m_Size;
            m_Value.str = rhs.m_Value.str;
            rhs.SetType(VAR_TYPE_NONE, VAR_SUBTYPE_NONE);
            rhs.m_Size = 0;
            rhs.m_Value.str = nullptr;
            break;
        case VAR_TYPE_BUF:
            Clear();
            SetType(VAR_TYPE_BUF, VAR_SUBTYPE_NONE);
            m_Size = rhs.m_Size;
            m_Value.buf = rhs.m_Value.buf;
            rhs.SetType(VAR_TYPE_NONE, VAR_SUBTYPE_NONE);
            rhs.m_Size = 0;
            rhs.m_Value.buf = nullptr;
            break;
        case VAR_TYPE_PTR:
            m_Size = rhs.m_Size;
            m_Value.ptr = rhs.m_Value.ptr;
            break;
        default:
            break;
    }

    return *this;
}

bool Variant::operator==(const Variant &rhs) const {
    if (GetType() != rhs.GetType())
        return false;

    switch (GetType()) {
        case VAR_TYPE_BOOL:
            return m_Value.b == rhs.m_Value.b;
        case VAR_TYPE_CHAR:
            return m_Value.c == rhs.m_Value.c;
        case VAR_TYPE_NUM:
            switch (GetSubtype()) {
                case VAR_SUBTYPE_UINT8:
                    return GetUint8() == rhs.GetUint8();
                case VAR_SUBTYPE_INT8:
                    return GetInt8() == rhs.GetInt8();
                case VAR_SUBTYPE_UINT16:
                    return GetUint16() == rhs.GetUint16();
                case VAR_SUBTYPE_INT16:
                    return GetInt16() == rhs.GetInt16();
                case VAR_SUBTYPE_UINT32:
                    return GetUint32() == rhs.GetUint32();
                case VAR_SUBTYPE_INT32:
                    return GetInt32() == rhs.GetInt32();
                case VAR_SUBTYPE_UINT64:
                    return GetUint64() == rhs.GetUint64();
                case VAR_SUBTYPE_INT64:
                    return GetInt64() == rhs.GetInt64();
                case VAR_SUBTYPE_FLOAT32:
                    return GetFloat32() == rhs.GetFloat32();
                case VAR_SUBTYPE_FLOAT64:
                    return GetFloat64() == rhs.GetFloat64();
                default:
                    break;
            }
        case VAR_TYPE_STR:
            return strncmp(m_Value.str, rhs.m_Value.str, (m_Size > rhs.m_Size) ? m_Size : rhs.m_Size) == 0;
        case VAR_TYPE_BUF:
            return memcmp(m_Value.buf, rhs.m_Value.buf, (m_Size > rhs.m_Size) ? m_Size : rhs.m_Size) == 0;
        case VAR_TYPE_PTR:
            return m_Value.ptr == rhs.m_Value.ptr;
        default:
            break;
    }

    return false;
}

bool Variant::operator==(const char *value) const {
    return IsString() && strcmp(m_Value.str, value) == 0;
}

void Variant::SetBuffer(const void *buf, size_t size) {
    if (!buf || size == 0)
        return;

    auto *buffer = malloc(size);
    if (buffer) {
        Clear();
        SetType(VAR_TYPE_BUF, VAR_SUBTYPE_NONE);
        m_Size = size;
        m_Value.buf = buffer;
        memcpy(m_Value.buf, buf, m_Size);
    }
}

void Variant::Clear() {
    if (IsString() && m_Value.str) {
        free(m_Value.str);
    } else if (IsBuffer() && m_Value.buf) {
        free(m_Value.buf);
    }

    SetType(VAR_TYPE_NONE, VAR_SUBTYPE_NONE);
    m_Size = 0;
    memset(&m_Value, 0, sizeof(VariantValue));
}

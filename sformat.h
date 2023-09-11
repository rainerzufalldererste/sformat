#ifndef sformat_h__
#define sformat_h__

#include <stdint.h>
#include <type_traits>
#include <string>
#include <assert.h>

template <typename T, typename U>
constexpr inline auto _max(const T &a, const U &b) -> decltype(a > b ? a : b)
{
  return a > b ? a : b;
}

template <typename T, typename U>
constexpr inline auto _min(const T &a, const U &b) -> decltype(a < b ? a : b)
{
  return a < b ? a : b;
}

template <typename T>
constexpr inline T _clamp(const T &a, const T &min, const T &max)
{
  if (a < min)
    return min;

  if (a > max)
    return max;

  return a;
}

constexpr inline int64_t _isEquivalentIntegerTypeSigned(int64_t) { return true; }
constexpr inline int32_t _isEquivalentIntegerTypeSigned(int32_t) { return true; }
constexpr inline long _isEquivalentIntegerTypeSigned(long) { return true; }
constexpr inline int16_t _isEquivalentIntegerTypeSigned(int16_t) { return true; }
constexpr inline int8_t _isEquivalentIntegerTypeSigned(int8_t) { return true; }
constexpr inline uint64_t _isEquivalentIntegerTypeSigned(uint64_t) { return false; }
constexpr inline uint32_t _isEquivalentIntegerTypeSigned(uint32_t) { return false; }
constexpr inline unsigned long _isEquivalentIntegerTypeSigned(unsigned long) { return false; }
constexpr inline uint16_t _isEquivalentIntegerTypeSigned(uint16_t) { return false; }
constexpr inline uint8_t _isEquivalentIntegerTypeSigned(uint8_t) { return false; }

template <typename T>
struct _enumEquivalentIntegerType
{
  typedef decltype(_isEquivalentIntegerTypeSigned((T)0)) type;
};

template <typename T>
struct _unsignedEquivalent
{

};

template <>
struct _unsignedEquivalent<int64_t>
{
  typedef uint64_t type;
};

template <>
struct _unsignedEquivalent<int32_t>
{
  typedef uint32_t type;
};

template <>
struct _unsignedEquivalent<long>
{
  typedef unsigned long type;
};

template <>
struct _unsignedEquivalent<int16_t>
{
  typedef uint16_t type;
};

template <>
struct _unsignedEquivalent<int8_t>
{
  typedef uint8_t type;
};

template <typename T>
struct _isFloatFormattable_t
{
  static constexpr bool value = std::is_floating_point<T>::value;
};

template <typename T>
struct _isIntegerFormattable_t
{
  static constexpr bool value = std::is_integral<T>::value && !std::is_same<T, bool>::value;
};

template <typename T>
struct _isListFormattable_t
{
  static constexpr bool value = false;
  typedef void value_type;
};

template <typename T>
struct _isVectorFormattable_t
{
  static constexpr bool value = false;
  typedef void value_type;
  static constexpr size_t count = 0;
};

template <typename T>
struct _isMapFormattable_t
{
  static constexpr bool value = false;
  typedef void key_type;
  typedef void value_type;
};

template <typename T>
struct _isFormattable_t
{
  static constexpr bool value = _isIntegerFormattable_t<T>::value || _isFloatFormattable_t<T>::value || _isListFormattable_t<T>::value || _isVectorFormattable_t<T>::value || _isMapFormattable_t<T>::value;
};

template <>
struct _isFormattable_t<bool>
{
  static constexpr bool value = true;
};

template <>
struct _isFormattable_t<char *>
{
  static constexpr bool value = true;
};

template <>
struct _isFormattable_t<wchar_t *>
{
  static constexpr bool value = true;
};

template <>
struct _isFormattable_t<const char *>
{
  static constexpr bool value = true;
};

template <>
struct _isFormattable_t<const wchar_t *>
{
  static constexpr bool value = true;
};

template <size_t count>
struct _isFormattable_t<char[count]>
{
  static constexpr bool value = true;
};

template <size_t count>
struct _isFormattable_t<wchar_t[count]>
{
  static constexpr bool value = true;
};

template <typename T, size_t count>
struct _isListFormattable_t<T[count]>
{
  static constexpr bool value = !std::is_same<T, char>::value && !std::is_same<T, wchar_t>::value && _isFormattable_t<T>::value;
  typedef T value_type;
};

template <typename T>
inline constexpr bool _isFormattable(const T &)
{
  return _isFormattable_t<T>::value;
}

template <typename T>
inline constexpr bool _isIntegerFormattable(const T &)
{
  return _isIntegerFormattable_t<T>::value;
}

template <typename T>
inline constexpr bool _isFloatFormattable(const T &)
{
  return _isFloatFormattable_t<T>::value;
}

template <typename T>
inline constexpr bool _isListFormattable(const T &)
{
  return _isListFormattable_t<T>::value && _isFormattable_t<_isListFormattable_t<T>::value_type>::value;
}

template <typename T>
inline constexpr bool _isVectorFormattable(const T &)
{
  return _isVectorFormattable_t<T>::value && _isFormattable_t<_isVectorFormattable_t<T>::value_type>::value;
}

template <typename T>
inline constexpr bool _isMapFormattable(const T &)
{
  return _isMapFormattable_t<T>::value && _isFormattable_t<_isMapFormattable_t<T>::key_type>::value && _isFormattable_t<_isMapFormattable_t<T>::value_type>::value;
}

enum sformatSignOption
{
  FSO_NegativeOnly,
  FSO_NegativeOrFill,
  FSO_Both,
  FSO_Never
};

enum sformatBaseOption
{
  FBO_Decimal = 10,
  FBO_Hexadecimal = 0x10,
  FBO_Binary = 0b10
};

enum sformatAlign
{
  FA_Left,
  FA_Right,
  FA_Center
};

enum sformatDigitGroupingOption
{
  FDGO_Thousand, // SI, commonly used throughout the western world. (1.234.567 / 1,234,567 / 1 234 567 / 1'234'567)
  FDGO_TenThousand, // For Chinese Numerals. (1,2345,6789 / 1亿2345万6789 / 1億2345萬6789)
  FDGO_Indian // For the Indian numbering system. (12,34,56,789)
};

// For Hexadecimal & Binary Values, Strings.
enum sformatOverflowBehaviour
{
  FOB_AlignRight,
  FOB_AlignLeft
};

struct sformat_allocator
{
  bool (*alloc)(void **, const size_t);
  bool (*realloc)(void **, const size_t);
  void (*free)(void *);
};

extern sformat_allocator _default_sformat_allocator;

struct sformatState
{
  sformat_allocator *pAllocator = &_default_sformat_allocator;
  char *textStart = nullptr;
  size_t textCapacity = 0;
  size_t textPosition = 0;
  bool inFormatStatement = false;
  char fillCharacter = ' ';
  bool fillCharacterIsZero = false;
  size_t decimalSeparatorLength = 1;
  char digitGroupingChars[5] = { ',', '\0' };
  bool groupDigits = false;
  size_t digitGroupingCharLength = 1;
  sformatDigitGroupingOption digitGroupingOption = FDGO_Thousand;
  char decimalSeparatorChars[5] = { '.', '\0' };
  char listMapStartChar = '{';
  char listMapEndChar = '}';
  char listMapSeparatorChar = ',';
  char mapEntryStartChar = '[';
  char mapEntryEndChar = ']';
  char mapEntryKeyValueSeparator = ':';
  char vectorStartChar = '[';
  char vectorEndChar = ']';
  char vectorSeparatorChar = ',';
  bool listMapSpaceAfterStart = true;
  bool listMapSpaceBeforeEnd = true;
  bool mapEntrySpaceAfterStart = true;
  bool mapEntrySpaceBeforeEnd = true;
  bool vectorSpaceAfterStart = false;
  bool vectorSpaceBeforeEnd = false;
  bool listMapSpaceAfterSeparator = true;
  bool vectorSpaceAfterSeparator = true;
  bool mapSpaceAfterKeyValueSeparator = true;
  size_t minChars = 0;
  size_t maxChars = INT64_MAX; // yes, not uint64_t max.
  size_t fractionalDigits = 5;
  sformatSignOption signOption = FSO_NegativeOnly;
  bool alignSign = true;
  sformatBaseOption integerBaseOption = FBO_Decimal;
  bool hexadecimalUpperCase = true;
  sformatAlign stringAlign = FA_Left;
  sformatAlign numberAlign = FA_Right;
  bool stringOverflowEllipsis = true;
  size_t stringOverflowEllipsisLength = 3;
  size_t stringOverflowEllipsisCount = 3;
  char stringOverflowEllipsisChars[8] = { '.', '.', '.', ' ', '\0' };
  sformatOverflowBehaviour numberOverflow = FOB_AlignRight;
  size_t listMapMaxLength = 25;
  size_t listMapContinuationLength = 3; // doesn't include a null terminator.
  char listMapContinuation[5] = { '.', '.', '.', ' ', '\0' };
  size_t infinityCount = 8;
  size_t infinityBytes = 8;
  char infinityChars[32] = "Infinity";
  size_t negativeInfinityCount = 9;
  size_t negativeInfinityBytes = 9;
  char negativeInfinityChars[32] = "-Infinity";
  size_t nanCount = 3;
  size_t nanBytes = 3;
  char nanChars[32] = "NaN";
  char exponentChar = 'e';
  bool scientificNotation = false;
  size_t trueCount = 4;
  size_t trueBytes = 4;
  char trueChars[32] = "true";
  size_t falseCount = 5;
  size_t falseBytes = 5;
  char falseChars[32] = "false";
  bool adaptiveFractionalDigits = true;
  bool adaptiveFloatScientificNotation = false;
  int16_t adaptiveScientificNotationPositiveExponentThreshold = 10;
  int16_t adaptiveScientificNotationNegativeExponentThreshold = -4;

  sformatState() = default;
  sformatState(const sformatState &copy) = default;

  // This should only be used
  inline sformatState operator = (const sformatState &copy)
  {
    new (this) sformatState(copy);

    textStart = nullptr;
    textCapacity = 0;
    textPosition = 0;
    inFormatStatement = false;
    pAllocator = &_default_sformat_allocator;
  }

  inline void SetTo(const sformatState &copy)
  {
    bool previousInFormatStatement = inFormatStatement;
    sformat_allocator *pPreviousAllocator = pAllocator;
    char *previousTextStart = textStart;
    size_t previousTextCapacity = textCapacity;
    size_t previousTextPosition = textPosition;

    new (this) sformatState(copy);

    inFormatStatement = previousInFormatStatement;
    pAllocator = pPreviousAllocator;
    textStart = previousTextStart;
    textCapacity = previousTextCapacity;
    textPosition = previousTextPosition;
  }
};

sformatState &sformat_GetState();
sformatState &sformat_GetGlobalState();
void sformatState_ResetCulture();

#pragma warning (push)
#pragma warning (disable: 4702)

template <typename T, typename std::enable_if<std::is_integral<T>::value &&std::is_signed<T>::value && !std::is_same<bool, T>::value && !std::is_same<char, T>::value && !std::is_enum<T>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T &value, const sformatState &fs)
{
  size_t signChars = 0;
  size_t numberChars;

  switch (fs.integerBaseOption)
  {
  default:
  case FBO_Decimal:
  {
    switch (fs.signOption)
    {
    case FSO_Both:
    case FSO_NegativeOrFill:
    case FSO_NegativeOnly:
      signChars = 1;
      break;
    }

    if constexpr (sizeof(value) == 1)
      numberChars = 3;
    else if constexpr (sizeof(value) == 2)
      numberChars = 5;
    else if constexpr (sizeof(value) == 4)
      numberChars = 10;
    else
      numberChars = 19;

      if (fs.groupDigits)
      {
        size_t groupingChars;

        switch (fs.digitGroupingOption)
        {
        default:
        case FDGO_Thousand:
          groupingChars = (numberChars - 1) / 3;
          break;

        case FDGO_TenThousand:
          groupingChars = (numberChars - 1) / 4;
          break;

        case FDGO_Indian:
          groupingChars = (size_t)!!((numberChars - 1) / 3);

          if (groupingChars)
            groupingChars += (numberChars - 4) / 2;

          break;
        }

        return signChars + _clamp(numberChars * fs.digitGroupingCharLength, fs.minChars, fs.maxChars);
      }
      else
      {
        return signChars + _clamp(numberChars, fs.minChars, fs.maxChars);
      }

      break;
  }

  case FBO_Hexadecimal:
  case FBO_Binary:
  {
    return sformat_GetCount((typename _unsignedEquivalent<T>::type)value, fs);
  }
  }
}

template <typename T, typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T &value, const sformatState &fs)
{
  return sformat_GetMaxBytes((typename _enumEquivalentIntegerType<T>::type)value, fs);
}

template <typename T, typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
inline size_t sformat_GetCount(const T &value, const sformatState &fs)
{
  return sformat_GetCount((typename _enumEquivalentIntegerType<T>::type)value, fs);
}

template <typename T, typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
inline size_t _sformat_Append(const T &value, const sformatState &fs, char *text)
{
  return _sformat_Append((typename _enumEquivalentIntegerType<T>::type)value, fs, text);
}

inline size_t _sformat_GetDigitGroupingCharCount(const size_t numberChars, const sformatState &fs)
{
  if (numberChars == 0)
    return 0;

  switch (fs.digitGroupingOption)
  {
  default:
  case FDGO_Thousand:
    return (numberChars - 1) / 3;

  case FDGO_TenThousand:
    return (numberChars - 1) / 4;

  case FDGO_Indian:
    size_t groupingChars = (size_t)!!((numberChars - 1) / 3);

    if (groupingChars)
      groupingChars += (numberChars - 4) / 2;

    return groupingChars;
  }
}

template <typename T, typename std::enable_if<std::is_integral<T>::value &&std::is_unsigned<T>::value && !std::is_same<bool, T>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T &value, const sformatState &fs)
{
  size_t signChars = 0;
  size_t numberChars;

  switch (fs.integerBaseOption)
  {
  default:
  case FBO_Decimal:
  {
    switch (fs.signOption)
    {
    case FSO_Both:
    case FSO_NegativeOrFill:
      signChars = 1;
      break;
    }

    if constexpr (sizeof(value) == 1)
      numberChars = 3;
    else if constexpr (sizeof(value) == 2)
      numberChars = 5;
    else if constexpr (sizeof(value) == 4)
      numberChars = 10;
    else
      numberChars = 20;

      if (fs.groupDigits)
        return signChars + _clamp(numberChars + _sformat_GetDigitGroupingCharCount(numberChars, fs) * fs.digitGroupingCharLength, fs.minChars, fs.maxChars);
      else
        return signChars + _clamp(numberChars, fs.minChars, fs.maxChars);

      break;
  }

  case FBO_Hexadecimal:
  {
    return sizeof(value) * 2;
  }

  case FBO_Binary:
  {
    return sizeof(value) * 8;
  }
  }
}

template <typename T, typename std::enable_if<std::is_same<bool, T>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T &, const sformatState &fs)
{
  return _max(fs.trueBytes, fs.falseBytes);
}

template <typename T, typename std::enable_if<std::is_same<bool, T>::value>::type * = nullptr>
inline size_t sformat_GetCount(const T &value, const sformatState &fs)
{
  return value ? fs.trueChars : fs.falseChars;
}

inline size_t sformat_GetMaxBytes(const char &, const sformatState &)
{
  return 1;
}

inline size_t sformat_GetCount(const char &value, const sformatState &)
{
  if (value == 0)
    return 0;

  return 1;
}

inline size_t sformat_GetMaxBytes(const wchar_t &, const sformatState &)
{
  return 3; // i.e. 0x2026 will become 0xE2 0x80 0xA6 in UTF-8
}

size_t _sformat_GetStringCount(const char *value, const size_t length);

inline size_t sformat_GetCount(const char *value, const sformatState &fs)
{
  if (value == nullptr)
    return 0;

  return _clamp(_sformat_GetStringCount(value, strlen(value)), fs.minChars, fs.maxChars);
}

template <typename T, typename std::enable_if<std::is_same<T, char *>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(T value, const sformatState &fs)
{
  if (value == nullptr)
    return 0;

  return _clamp(strlen(value), fs.minChars * 4, fs.maxChars * 4);
}

template <typename T, typename std::enable_if<std::is_same<T, const char *>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(T value, const sformatState &fs)
{
  if (value == nullptr)
    return 0;

  return _clamp(strlen(value), fs.minChars * 4, fs.maxChars * 4);
}

template <typename T, typename std::enable_if<std::is_same<T, const wchar_t *>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T value, const sformatState &fs)
{
  if (value == nullptr)
    return 0;

  return _clamp(wcslen(value) * mString_MaxUtf16CharInUtf8Chars + 1, fs.minChars * 4, fs.maxChars * 4);
}

template <typename T, typename std::enable_if<std::is_same<T, wchar_t *>::value>::type * = nullptr>
inline size_t sformat_GetMaxBytes(const T value, const sformatState &fs)
{
  if (value == nullptr)
    return 0;

  return _clamp(wcslen(value) * mString_MaxUtf16CharInUtf8Chars + 1, fs.minChars * 4, fs.maxChars * 4);
}

template <size_t TCount>
inline size_t sformat_GetCount(char(&value)[TCount], const sformatState &fs)
{
  return _clamp(_sformat_GetStringCount(value, TCount), fs.minChars, fs.maxChars);
}

template <size_t TCount>
inline size_t sformat_GetMaxBytes(char(&)[TCount], const sformatState &fs)
{
  return _clamp(TCount, fs.minChars * 4, fs.maxChars * 4);
}

template <size_t TCount>
inline size_t sformat_GetMaxBytes(wchar_t(&)[TCount], const sformatState &fs)
{
  return _clamp(TCount * mString_MaxUtf16CharInUtf8Chars + 1, fs.minChars * 4, fs.maxChars * 4);
}

template <size_t TCount>
inline size_t sformat_GetCount(const char(&value)[TCount], const sformatState &fs)
{
  return _clamp(_sformat_GetStringCount(value, TCount), fs.minChars, fs.maxChars);
}

template <size_t TCount>
inline size_t sformat_GetMaxBytes(const char(&)[TCount], const sformatState &fs)
{
  return _clamp(TCount, fs.minChars * 4, fs.maxChars * 4);
}

template <size_t TCount>
inline size_t sformat_GetMaxBytes(const wchar_t(&)[TCount], const sformatState &fs)
{
  return _clamp(TCount * mString_MaxUtf16CharInUtf8Chars + 1, fs.minChars * 4, fs.maxChars * 4);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_signed<T>::value && !std::is_same<bool, T>::value>::type * = nullptr>
inline size_t sformat_GetCount(const T &value, const sformatState &fs)
{
  size_t signChars = 0;
  size_t numberChars = 0;

  switch (fs.integerBaseOption)
  {
  default:
  case FBO_Decimal:
  {
    switch (fs.signOption)
    {
    case FSO_Both:
    case FSO_NegativeOrFill:
      signChars = 1;
      break;
    }

    if constexpr (sizeof(value) == 1)
      goto one_byte_decimal;
    else if constexpr (sizeof(value) == 2)
      goto two_bytes_decimal;
    else if constexpr (sizeof(value) == 4)
      goto four_bytes_decimal;

      if (value >= 10000000000000000000) { numberChars = 20; break; }
      if (value >= 1000000000000000000) { numberChars = 19; break; }
      if (value >= 100000000000000000) { numberChars = 18; break; }
      if (value >= 10000000000000000) { numberChars = 17; break; }
      if (value >= 1000000000000000) { numberChars = 16; break; }
      if (value >= 100000000000000) { numberChars = 15; break; }
      if (value >= 10000000000000) { numberChars = 14; break; }
      if (value >= 1000000000000) { numberChars = 13; break; }
      if (value >= 100000000000) { numberChars = 12; break; }
      if (value >= 10000000000) { numberChars = 11; break; }

      goto four_bytes_decimal;
    four_bytes_decimal:

      if (value >= 1000000000) { numberChars = 10; break; }
      if (value >= 100000000) { numberChars = 9; break; }
      if (value >= 10000000) { numberChars = 8; break; }
      if (value >= 1000000) { numberChars = 7; break; }
      if (value >= 100000) { numberChars = 6; break; }

      goto two_bytes_decimal;
    two_bytes_decimal:

      if (value >= 10000) { numberChars = 5; break; }
      if (value >= 1000) { numberChars = 4; break; }

      goto one_byte_decimal;
    one_byte_decimal:

      if (value >= 100) { numberChars = 3; break; }
      if (value >= 10) { numberChars = 2; break; }

      numberChars = 1;
      break;
  }

  case FBO_Hexadecimal:
  {
    numberChars = 0;
    T tmp = value;

    while (tmp)
    {
      numberChars++;
      tmp >>= 4;
    }

    return _clamp(numberChars, fs.minChars, fs.maxChars);
  }

  case FBO_Binary:
  {
    numberChars = 0;
    T tmp = value;

    while (tmp)
    {
      numberChars++;
      tmp >>= 1;
    }

    return _clamp(numberChars, fs.minChars, fs.maxChars);
  }
  }

  if (fs.groupDigits)
    return _clamp(signChars + numberChars + _sformat_GetDigitGroupingCharCount(numberChars, fs) * fs.digitGroupingCharLength, fs.minChars, fs.maxChars);
  else
    return _clamp(signChars + numberChars, fs.minChars, fs.maxChars);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value &&std::is_signed<T>::value && !std::is_same<bool, T>::value>::type * = nullptr>
inline size_t sformat_GetCount(const T &value, const sformatState &fs)
{
  size_t signChars = 0;
  size_t numberChars = 0;

  switch (fs.integerBaseOption)
  {
  default:
  case FBO_Decimal:
  {
    switch (fs.signOption)
    {
    case FSO_Both:
    case FSO_NegativeOrFill:
      signChars = 1;
      break;

    case FSO_NegativeOnly:
      if (value < 0)
        signChars = 1;
      break;
    }

    const T negativeAbs = value < 0 ? value : -value; // because otherwise the minimum value couldn't be converted to a valid signed equivalent.

    if constexpr (sizeof(value) == 1)
      goto one_byte_decimal;
    else if constexpr (sizeof(value) == 2)
      goto two_bytes_decimal;
    else if constexpr (sizeof(value) == 4)
      goto four_bytes_decimal;

      if (negativeAbs <= -1000000000000000000) { numberChars = 19; break; }
      if (negativeAbs <= -100000000000000000) { numberChars = 18; break; }
      if (negativeAbs <= -10000000000000000) { numberChars = 17; break; }
      if (negativeAbs <= -1000000000000000) { numberChars = 16; break; }
      if (negativeAbs <= -100000000000000) { numberChars = 15; break; }
      if (negativeAbs <= -10000000000000) { numberChars = 14; break; }
      if (negativeAbs <= -1000000000000) { numberChars = 13; break; }
      if (negativeAbs <= -100000000000) { numberChars = 12; break; }
      if (negativeAbs <= -10000000000) { numberChars = 11; break; }

      goto four_bytes_decimal;
    four_bytes_decimal:

      if (negativeAbs <= -1000000000) { numberChars = 10; break; }
      if (negativeAbs <= -100000000) { numberChars = 9; break; }
      if (negativeAbs <= -10000000) { numberChars = 8; break; }
      if (negativeAbs <= -1000000) { numberChars = 7; break; }
      if (negativeAbs <= -100000) { numberChars = 6; break; }

      goto two_bytes_decimal;
    two_bytes_decimal:

      if (negativeAbs <= -10000) { numberChars = 5; break; }
      if (negativeAbs <= -1000) { numberChars = 4; break; }

      goto one_byte_decimal;
    one_byte_decimal:

      if (negativeAbs <= -100) { numberChars = 3; break; }
      if (negativeAbs <= -10) { numberChars = 2; break; }

      numberChars = 1;
      break;
  }

  case FBO_Hexadecimal:
  case FBO_Binary:
  {
    return sformat_GetCount((_unsignedEquivalent<T>::type)value, fs);
  }
  }

  if (fs.groupDigits)
    return _clamp(signChars + numberChars + _sformat_GetDigitGroupingCharCount(numberChars, fs) * fs.digitGroupingCharLength, fs.minChars, fs.maxChars);
  else
    return _clamp(signChars + numberChars, fs.minChars, fs.maxChars);
}

#pragma warning (pop)

inline size_t sformat_GetMaxBytes(const float_t, const sformatState &fs)
{
  constexpr size_t maxDigits = sizeof("340282346638528859811704183484516925440") - 1;

  if (fs.scientificNotation)
    return _clamp(1 + 1 + fs.decimalSeparatorLength + fs.fractionalDigits + 1 + 1 + 10, fs.minChars, _max(fs.maxChars, 6ULL)); // sign + digit + decimalSeparator + decimalDigits + e + sign + exponent.
  else
    return _clamp(1 /* sign */ + maxDigits + _sformat_GetDigitGroupingCharCount(maxDigits, fs) * fs.digitGroupingCharLength + fs.decimalSeparatorLength + fs.fractionalDigits, fs.minChars, fs.maxChars);
}

inline size_t sformat_GetMaxBytes(const double_t, const sformatState &fs)
{
  constexpr size_t maxDigits = sizeof("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368") - 1;

  if (fs.scientificNotation)
    return _clamp(1 + 1 + fs.decimalSeparatorLength + fs.fractionalDigits + 1 + 1 + 10, fs.minChars, _max(fs.maxChars, 7ULL)); // sign + digit + decimalSeparator + decimalDigits + e + sign + exponent.
  else
    return _clamp(1 /* sign */ + maxDigits + _sformat_GetDigitGroupingCharCount(maxDigits, fs) * fs.digitGroupingCharLength + fs.decimalSeparatorLength + fs.fractionalDigits, fs.minChars, fs.maxChars);
}

template <typename T>
size_t _sformat_AppendVector(const T *pFirstValue, const size_t count, const sformatState &fs, char *text)
{
  size_t ret = 1;

  *text = fs.vectorStartChar;
  text++;

  if (fs.vectorSpaceAfterStart)
  {
    ret++;
    *text = ' ';
    text++;
  }

  for (size_t i = 0; i < count; i++)
  {
    if (i != 0)
    {
      *text = fs.vectorSeparatorChar;
      text++;
      ret++;

      if (fs.vectorSpaceAfterSeparator)
      {
        ret++;
        *text = ' ';
        text++;
      }
    }

    const size_t length = _sformat_Append(*pFirstValue, fs, text);

    text += length;
    ret += length;
    pFirstValue++;
  }

  if (fs.vectorSpaceBeforeEnd)
  {
    ret++;
    *text = ' ';
    text++;
  }

  *text = fs.vectorEndChar;
  text++;
  ret++;

  return ret;
}

inline size_t _sformat_Append(const char value, const sformatState &fs, char *text)
{
  if (fs.maxChars == 0 || value == 0)
    return 0;

  *text = value;
  return 1;
}

size_t _sformat_Append(const int64_t value, const sformatState &fs, char *text);
size_t _sformat_Append(const uint64_t value, const sformatState &fs, char *text);
size_t _sformat_Append(const float_t value, const sformatState &fs, char *text);
size_t _sformat_Append(const double_t value, const sformatState &fs, char *text);
size_t _sformat_Append(const wchar_t value, const sformatState &fs, char *text);

size_t _sformat_AppendWStringWithLength(const wchar_t *string, const size_t charCount, const sformatState &fs, char *text);

template <typename T, typename std::enable_if<std::is_same<T, wchar_t *>::value>::type * = nullptr>
inline size_t _sformat_Append(const T value, const sformatState &fs, char *text)
{
  if (value == nullptr)
    return 0;

  return _sformat_AppendWStringWithLength(value, wcslen(value), fs, text);
}

template <typename T, typename std::enable_if<std::is_same<T, const wchar_t *>::value>::type * = nullptr>
inline size_t _sformat_Append(const T value, const sformatState &fs, char *text)
{
  if (value == nullptr)
    return 0;

  return _sformat_AppendWStringWithLength(value, wcslen(value), fs, text);
}

template <size_t TCount>
inline size_t _sformat_Append(wchar_t(&value)[TCount], const sformatState &fs, char *text)
{
  return _sformat_AppendWStringWithLength(value, wcsnlen_s(value, TCount), fs, text);
}

template <size_t TCount>
inline size_t _sformat_Append(const wchar_t(&value)[TCount], const sformatState &fs, char *text)
{
  return _sformat_AppendWStringWithLength(value, wcsnlen_s(value, TCount), fs, text);
}

size_t _sformat_AppendBool(const bool value, const sformatState &fs, char *text);

template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type * = nullptr>
inline size_t _sformat_Append(const T value, const sformatState &fs, char *text)
{
  return _sformat_AppendBool(value, fs, text);
}

inline size_t _sformat_Append(const int32_t value, const sformatState &fs, char *text) { return _sformat_Append((int64_t)value, fs, text); }
inline size_t _sformat_Append(const int16_t value, const sformatState &fs, char *text) { return _sformat_Append((int64_t)value, fs, text); }
inline size_t _sformat_Append(const int8_t value, const sformatState &fs, char *text) { return _sformat_Append((int64_t)value, fs, text); }
inline size_t _sformat_Append(const uint32_t value, const sformatState &fs, char *text) { return _sformat_Append((uint64_t)value, fs, text); }
inline size_t _sformat_Append(const uint16_t value, const sformatState &fs, char *text) { return _sformat_Append((uint64_t)value, fs, text); }
inline size_t _sformat_Append(const uint8_t value, const sformatState &fs, char *text) { return _sformat_Append((uint64_t)value, fs, text); }

size_t _sformat_AppendStringWithLength(const char *value, const size_t length, const sformatState &fs, char *text);

template <size_t TCount>
inline size_t _sformat_Append(char(&value)[TCount], const sformatState &fs, char *text)
{
  if (TCount == 0)
    return 0;

  return _sformat_AppendStringWithLength(value, strnlen(value, TCount - 1), fs, text);
}

template <size_t TCount>
inline size_t _sformat_Append(const char(&value)[TCount], const sformatState &fs, char *text)
{
  if constexpr (TCount == 0)
    return 0;

  return _sformat_AppendStringWithLength(value, strnlen(value, TCount - 1), fs, text);
}

template <typename T, typename std::enable_if<std::is_same<T, char *>::value>::type * = nullptr>
inline size_t _sformat_Append(const T value, const sformatState &fs, char *text)
{
  if (value == nullptr)
    return 0;

  const size_t length = strlen(value);

  return _sformat_AppendStringWithLength(value, length, fs, text);
}

template <typename T, typename std::enable_if<std::is_same<T, const char *>::value>::type * = nullptr>
inline size_t _sformat_Append(const T value, const sformatState &fs, char *text)
{
  if (value == nullptr)
    return 0;

  const size_t length = strlen(value);

  return _sformat_AppendStringWithLength(value, length, fs, text);
}

size_t _sformat_AppendInplaceString(const char *string, const size_t count, const size_t length, const sformatState &fs, char *text);

template <typename T>
size_t _sformat_GetMaxBytes(const sformatState &fs, const T &param)
{
  return sformat_GetMaxBytes(param, fs);
}

template <typename T, typename... Args>
size_t _sformat_GetMaxBytes(const sformatState &fs, const T &param, Args && ... args)
{
  return sformat_GetMaxBytes(param, fs) + _sformat_GetMaxBytes(fs, args...);
}

template <typename T, typename... Args>
size_t _sformat_Append_Internal(const sformatState &fs, char *text, const T &param)
{
  return _sformat_Append(param, fs, text);
}

template <typename T, typename... Args>
size_t _sformat_Append_Internal(const sformatState &fs, char *text, const T &param, Args && ... args)
{
  const size_t offset = _sformat_Append(param, fs, text);

  text += offset;

  return offset + _sformat_Append_Internal(fs, text, args...);
}

// sformat is a general purpose replacement for sprintf() (to char *). The buffer that is returned will be overwritten whenever sformat is called by the same thread. Please don't pass around the returned pointer. The returned pointer does not need to be `free`d.
template <typename... Args>
inline const char *sformat(Args && ...args)
{
  sformatState &fs = sformat_GetState();

  if (fs.inFormatStatement)
  {
    assert(false && "Recursive sformat is not supported.");
    return "";
  }

  fs.inFormatStatement = true;

  fs.textPosition = 0;

  const size_t maxCapacityRequired = _sformat_GetMaxBytes(fs, args...) + 1;

  if (fs.textCapacity < maxCapacityRequired)
  {
    const size_t nextCapacity = (maxCapacityRequired + (maxCapacityRequired - fs.textCapacity) * 2 + 1023) & ~(size_t)1023;

    if (!fs.pAllocator->realloc(reinterpret_cast<void **>(&fs.textStart), nextCapacity))
    {
      fs.inFormatStatement = false;
      return "<ERROR: MEMORY_ALLOCATION_FAILURE>";
    }

    fs.textCapacity = nextCapacity;
  }

  const size_t size = _sformat_Append_Internal(fs, fs.textStart, args...);

  fs.textStart[size] = '\0';
  fs.textPosition = size + 1;

  fs.inFormatStatement = false;
  return fs.textStart;
}

template <typename ...Args>
inline size_t sformat_capacity(Args && ...args)
{
  return _sformat_GetMaxBytes(sformat_GetState(), args...) + 1;
}

template <typename ...Args>
inline bool sformat_to(char *destination, const size_t capacity, Args && ...args)
{
  if (destination == nullptr)
    return false;

  sformatState &fs = sformat_GetState();

  const size_t maxCapacityRequired = _sformat_GetMaxBytes(fs, args...) + 1;
  const bool fitsInPlace = maxCapacityRequired <= capacity;

  if (fitsInPlace)
  {
    const size_t length = _sformat_Append_Internal(fs, destination, args...);
    destination[length] = '\0';
  }
  else
  {
    const char *result = sformat(args...);
    const size_t length = fs.textPosition;

    if (result == nullptr || length == 0 || capacity < length)
      return false;

    memcpy(destination, result, length);
  }

  return true;
}

template <typename T>
void _sformat_ApplyFormat(sformatState &fs)
{
  T::ApplyFormat(fs);
}

template <typename T, typename T2, typename ... Args>
void _sformat_ApplyFormat(sformatState &fs)
{
  T::ApplyFormat(fs);

  _sformat_ApplyFormat<T2, Args...>(fs);
}

template <typename T, typename ... Args>
struct _sformatType_Wrapper
{
  const T &value;

  _sformatType_Wrapper(const T &value) : value(value) { }
};

template <typename T, typename ... Args>
struct _sformatTypeInstance_Wrapper
{
  const T value;

  _sformatTypeInstance_Wrapper(const T value) : value(value) { }
};

#define _M_FORMAT_DEFINE_SPECIALIZED_ALIAS(name, type) \
  template <typename ... Args> struct name : public _sformatTypeInstance_Wrapper<type, Args...> \
  { inline name(const type v) : _sformatTypeInstance_Wrapper<type, Args...>(v) {} }

_M_FORMAT_DEFINE_SPECIALIZED_ALIAS(FInt, int64_t);
_M_FORMAT_DEFINE_SPECIALIZED_ALIAS(FUInt, uint64_t);
_M_FORMAT_DEFINE_SPECIALIZED_ALIAS(FFloat, float_t);
_M_FORMAT_DEFINE_SPECIALIZED_ALIAS(FDouble, double_t);
_M_FORMAT_DEFINE_SPECIALIZED_ALIAS(FBool, bool);

template <typename T, typename ... Args>
_sformatType_Wrapper<T, Args...> FString(const T &string, const Args && ...)
{
  return _sformatType_Wrapper<T, Args...>(string);
}

template <typename T, typename ... Args>
_sformatType_Wrapper<T, Args...> FVector(const T &vector, const Args && ...)
{
  return _sformatType_Wrapper<T, Args...>(vector);
}

template <typename T, typename ... Args>
size_t sformat_GetMaxBytes(const _sformatType_Wrapper<T, Args...> &value, const sformatState &fs)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return sformat_GetMaxBytes(value.value, localFS);
}

template <typename T, typename ... Args>
size_t sformat_GetCount(const _sformatType_Wrapper<T, Args...> &value, const sformatState &fs)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return sformat_GetCount(value.value, localFS);
}

template <typename T, typename ... Args>
size_t _sformat_Append(const _sformatType_Wrapper<T, Args...> &value, const sformatState &fs, char *text)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return _sformat_Append(value.value, localFS, text);
}

template <typename T, typename ... Args>
size_t sformat_GetMaxBytes(const _sformatTypeInstance_Wrapper<T, Args...> &value, const sformatState &fs)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return sformat_GetMaxBytes(value.value, localFS);
}

template <typename T, typename ... Args>
size_t sformat_GetCount(const _sformatTypeInstance_Wrapper<T, Args...> &value, const sformatState &fs)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return sformat_GetCount(value.value, localFS);
}

template <typename T, typename ... Args>
size_t _sformat_Append(const _sformatTypeInstance_Wrapper<T, Args...> &value, const sformatState &fs, char *text)
{
  sformatState localFS(fs);

  _sformat_ApplyFormat<Args...>(localFS);

  return _sformat_Append(value.value, localFS, text);
}

template <size_t maxDigits>
struct FMaxDigits
{
  static void ApplyFormat(sformatState &fs) { fs.maxChars = maxDigits; }
};

#define FMaxChars FMaxDigits

template <size_t minDigits>
struct FMinDigits
{
  static void ApplyFormat(sformatState &fs) { fs.minChars = minDigits; }
};

#define FMinChars FMinDigits

template <size_t fractionalDigits>
struct FFractionalDigits
{
  static void ApplyFormat(sformatState &fs) { fs.fractionalDigits = fractionalDigits; }
};

struct FHex
{
  static void ApplyFormat(sformatState &fs) { fs.integerBaseOption = FBO_Hexadecimal; }
};

struct FHexUppercase
{
  static void ApplyFormat(sformatState &fs) { fs.hexadecimalUpperCase = true; }
};

struct FHexLowercase
{
  static void ApplyFormat(sformatState &fs) { fs.hexadecimalUpperCase = false; }
};

struct FBinary
{
  static void ApplyFormat(sformatState &fs) { fs.integerBaseOption = FBO_Binary; }
};

struct FDecimal
{
  static void ApplyFormat(sformatState &fs) { fs.integerBaseOption = FBO_Decimal; }
};

struct FAlignNumRight
{
  static void ApplyFormat(sformatState &fs) { fs.numberAlign = FA_Right; }
};

struct FAlignNumLeft
{
  static void ApplyFormat(sformatState &fs) { fs.numberAlign = FA_Left; }
};

struct FAlignNumCenter
{
  static void ApplyFormat(sformatState &fs) { fs.numberAlign = FA_Center; }
};

struct FAlignStringRight
{
  static void ApplyFormat(sformatState &fs) { fs.stringAlign = FA_Right; }
};

struct FAlignStringLeft
{
  static void ApplyFormat(sformatState &fs) { fs.stringAlign = FA_Left; }
};

struct FAlignStringCenter
{
  static void ApplyFormat(sformatState &fs) { fs.stringAlign = FA_Center; }
};

struct FSignBoth
{
  static void ApplyFormat(sformatState &fs) { fs.signOption = FSO_Both; }
};

struct FSignNever
{
  static void ApplyFormat(sformatState &fs) { fs.signOption = FSO_Never; }
};

struct FSignNegativeOrFill
{
  static void ApplyFormat(sformatState &fs) { fs.signOption = FSO_NegativeOrFill; }
};

struct FSignNegativeOnly
{
  static void ApplyFormat(sformatState &fs) { fs.signOption = FSO_NegativeOnly; }
};

struct FSignAligned
{
  static void ApplyFormat(sformatState &fs) { fs.alignSign = true; }
};

struct FSignNotAligned
{
  static void ApplyFormat(sformatState &fs) { fs.alignSign = false; }
};

struct FFillZeroes
{
  static void ApplyFormat(sformatState &fs) { fs.fillCharacter = '0'; fs.fillCharacterIsZero = true; }
};

struct FFillWhitespace
{
  static void ApplyFormat(sformatState &fs) { fs.fillCharacter = ' '; fs.fillCharacterIsZero = false; }
};

struct FEllipsis
{
  static void ApplyFormat(sformatState &fs) { fs.stringOverflowEllipsis = true; }
};

struct FNoEllipsis
{
  static void ApplyFormat(sformatState &fs) { fs.stringOverflowEllipsis = false; }
};

struct FGroupDigits
{
  static void ApplyFormat(sformatState &fs) { fs.groupDigits = true; }
};

struct FNoExponent
{
  static void ApplyFormat(sformatState &fs) { fs.scientificNotation = false; }
};

struct FExponent
{
  static void ApplyFormat(sformatState &fs) { fs.scientificNotation = true; }
};

struct FExponentAdaptive
{
  static void ApplyFormat(sformatState &fs) { fs.adaptiveFloatScientificNotation = true; }
};

struct FExponentNotAdaptive
{
  static void ApplyFormat(sformatState &fs) { fs.adaptiveFloatScientificNotation = false; }
};

struct FFractionalDigitsAdaptive
{
  static void ApplyFormat(sformatState &fs) { fs.adaptiveFractionalDigits = true; }
};

struct FFractionalDigitsFixed
{
  static void ApplyFormat(sformatState &fs) { fs.adaptiveFractionalDigits = false; }
};

#define _SFORMAT_EXPAND(x) x

#ifndef _MSC_VER

#define _SFORMAT_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define _SFORMAT_FOR_EACH_0(_call, ...)
#define _SFORMAT_FOR_EACH_1(_call, x) _call(x)
#define _SFORMAT_FOR_EACH_2(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_1(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_3(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_2(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_4(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_3(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_5(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_4(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_6(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_5(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_7(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_6(_call, __VA_ARGS__)
#define _SFORMAT_FOR_EACH_8(_call, x, ...) _call(x) , _SFORMAT_FOR_EACH_7(_call, __VA_ARGS__)

#define SFORMAT_MACRO_COMMA_FOR_EACH(x, ...) _SFORMAT_GET_NTH_ARG(IGNORED, ##__VA_ARGS__, _SFORMAT_FOR_EACH_8, _SFORMAT_FOR_EACH_7, _SFORMAT_FOR_EACH_6, _SFORMAT_FOR_EACH_5, _SFORMAT_FOR_EACH_4, _SFORMAT_FOR_EACH_3, _SFORMAT_FOR_EACH_2, _SFORMAT_FOR_EACH_1, _SFORMAT_FOR_EACH_0)(x, ##__VA_ARGS__)

#define _SFORMAT_ARG_COUNT(...) _SFORMAT_ARG_COUNT_INTERNAL(0, ## __VA_ARGS__, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _SFORMAT_ARG_COUNT_INTERNAL(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count

#else

#define _SFORMAT_FOR_EACH_1(what, x, ...) what(x)
#define _SFORMAT_FOR_EACH_2(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_1(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_3(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_2(what, __VA_ARGS__))
#define _SFORMAT_FOR_EACH_4(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_3(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_5(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_4(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_6(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_5(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_7(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_6(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_8(what, x, ...)\
  what(x),\
  _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_7(what,  __VA_ARGS__))
#define _SFORMAT_FOR_EACH_NARG(...) _SFORMAT_FOR_EACH_NARG_(__VA_ARGS__, _SFORMAT_FOR_EACH_RSEQ_N())
#define _SFORMAT_FOR_EACH_NARG_(...) _SFORMAT_EXPAND(_SFORMAT_FOR_EACH_ARG_N(__VA_ARGS__))
#define _SFORMAT_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define _SFORMAT_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _SFORMAT_FOR_EACH_(N, what, ...) _SFORMAT_EXPAND(_CONCAT_LITERALS_INTERNAL(_SFORMAT_FOR_EACH_, N)(what, __VA_ARGS__))
#define SFORMAT_MACRO_COMMA_FOR_EACH(what, ...) _SFORMAT_FOR_EACH_(_SFORMAT_FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define _SFORMAT_ARG_COUNT(...)  _SFORMAT_ARG_COUNT_INTERNAL_EXPAND_ARGS_PRIVATE(_SFORMAT_ARG_COUNT_INTERNAL_ARGS_AUGMENTER(__VA_ARGS__))

#define _SFORMAT_ARG_COUNT_INTERNAL_ARGS_AUGMENTER(...) unused, __VA_ARGS__
#define _SFORMAT_ARG_COUNT_INTERNAL_EXPAND(x) x
#define _SFORMAT_ARG_COUNT_INTERNAL_EXPAND_ARGS_PRIVATE(...) _SFORMAT_ARG_COUNT_INTERNAL_EXPAND(_SFORMAT_ARG_COUNT_INTERNAL_GET_ARG_COUNT_PRIVATE(__VA_ARGS__, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define _SFORMAT_ARG_COUNT_INTERNAL_GET_ARG_COUNT_PRIVATE(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count

#endif

#define _SFORMAT_SHORT_Hex FHex
#define _SFORMAT_SHORT_x FHexLowercase
#define _SFORMAT_SHORT_X FHexUppercase
#define _SFORMAT_SHORT_Bin FBinary
#define _SFORMAT_SHORT_Frac(k) FFractionalDigits< k >
#define _SFORMAT_SHORT_Min(k) FMinDigits< k >
#define _SFORMAT_SHORT_Max(k) FMaxDigits< k >
#define _SFORMAT_SHORT_Fill0 FFillZeroes
#define _SFORMAT_SHORT_AllFrac FFractionalDigitsFixed
#define _SFORMAT_SHORT_Exp FExponent
#define _SFORMAT_SHORT_SBoth FSignBoth
#define _SFORMAT_SHORT_Center FAlignNumCenter
#define _SFORMAT_SHORT_Left FAlignNumLeft
#define _SFORMAT_SHORT_Right FAlignNumRight
#define _SFORMAT_SHORT_Group FGroupDigits

#define _SFORMAT_SHORT_STRING_Max(k) FMaxChars< k >
#define _SFORMAT_SHORT_STRING_Min(k) FMinChars< k >
#define _SFORMAT_SHORT_STRING_Center FAlignStringCenter
#define _SFORMAT_SHORT_STRING_Left FAlignStringLeft
#define _SFORMAT_SHORT_STRING_Right FAlignStringRight

#define _CONCAT_LITERALS_INTERNAL(x, y) x ## y
#define _CONCAT_LITERALS(x, y) _CONCAT_LITERALS_INTERNAL(x, y)
#define _SFORMAT_TO_SHORT_FORMAT(x) _CONCAT_LITERALS(_SFORMAT_SHORT_, x)
#define _SFORMAT_TO_STRING_SHORT_FORMAT(x) _CONCAT_LITERALS(_SFORMAT_SHORT_STRING_, x)()

#define _SFORMAT_XX_UNRAVEL(...) SFORMAT_MACRO_COMMA_FOR_EACH(_SFORMAT_TO_SHORT_FORMAT, __VA_ARGS__)
#define _SFORMAT_XX_UNRAVEL_STRING(...) SFORMAT_MACRO_COMMA_FOR_EACH(_SFORMAT_TO_STRING_SHORT_FORMAT, __VA_ARGS__)

#define FX_COMMA_OR_EMPTY_0
#define FX_COMMA_OR_EMPTY_1 ,
#define FX_COMMA_OR_EMPTY_2 ,
#define FX_COMMA_OR_EMPTY_3 ,
#define FX_COMMA_OR_EMPTY_4 ,
#define FX_COMMA_OR_EMPTY_5 ,
#define FX_COMMA_OR_EMPTY_6 ,
#define FX_COMMA_OR_EMPTY_7 ,
#define FX_COMMA_OR_EMPTY_8 ,
#define FX_COMMA_OR_EMPTY_9 ,

#define FI(...) FInt< _SFORMAT_XX_UNRAVEL(__VA_ARGS__) >
#define FU(...) FUInt< _SFORMAT_XX_UNRAVEL(__VA_ARGS__) >
#define FF(...) FFloat< _SFORMAT_XX_UNRAVEL(__VA_ARGS__) >
#define FD(...) FDouble< _SFORMAT_XX_UNRAVEL(__VA_ARGS__) >
#define FX(...) FUInt<_SFORMAT_XX_UNRAVEL(Hex _CONCAT_LITERALS(FX_COMMA_OR_EMPTY_, _SFORMAT_ARG_COUNT(__VA_ARGS__)) __VA_ARGS__) >
#define FS(string, ...) FString(string, _SFORMAT_XX_UNRAVEL_STRING(__VA_ARGS__))

#endif // sformat_h__

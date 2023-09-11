#include "sformat.h"

#define print(...) fputs(sformat(__VA_ARGS__), stdout)

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct vec2t
{
#pragma warning(push)
#pragma warning(disable: 4201)
  union
  {
    T asArray[2];

    struct
    {
      T x, y;
    };
  };
#pragma warning(pop)

  inline vec2t() : x(0), y(0) {}
  inline explicit vec2t(T _v) : x(_v), y(_v) {}
  inline vec2t(T _x, T _y) : x(_x), y(_y) {}
};

template <typename T>
struct vec3t
{
#pragma warning(push)
#pragma warning(disable: 4201)
  union
  {
    T asArray[3];

    struct
    {
      T x, y, z;
    };
  };
#pragma warning(pop)

  constexpr inline vec3t() : x(0), y(0), z(0) {}
  constexpr inline explicit vec3t(T _v) : x(_v), y(_v), z(_v) {}

  constexpr inline vec3t(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
  constexpr inline explicit vec3t(vec2t<T> vector2, T _z) : x(vector2.x), y(vector2.y), z(_z) {}
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline size_t sformat_GetMaxBytes(const vec2t<T> &, const sformatState &fs)
{
  constexpr size_t dimensions = 2;

  return 1 + (size_t)fs.vectorSpaceAfterStart + dimensions * sformat_GetMaxBytes((T)0, fs) + (dimensions - 1) * ((size_t)fs.vectorSpaceAfterSeparator + 1) + (size_t)fs.vectorSpaceBeforeEnd + 1;
}

template <typename T>
inline size_t _sformat_Append(const vec2t<T> &value, const sformatState &fs, char *text)
{
  return _sformat_AppendVector(value.asArray, 2, fs, text);
}

template <typename T>
inline size_t sformat_GetMaxBytes(const vec3t<T> &, const sformatState &fs)
{
  constexpr size_t dimensions = 3;

  return 1 + (size_t)fs.vectorSpaceAfterStart + dimensions * sformat_GetMaxBytes((T)0, fs) + (dimensions - 1) * ((size_t)fs.vectorSpaceAfterSeparator + 1) + (size_t)fs.vectorSpaceBeforeEnd + 1;
}

template <typename T>
inline size_t _sformat_Append(const vec3t<T> &value, const sformatState &fs, char *text)
{
  return _sformat_AppendVector(value.asArray, 3, fs, text);
}

////////////////////////////////////////////////////////////////////////////////

enum
{
  EnumValue = 0xBADF00D
};

////////////////////////////////////////////////////////////////////////////////

int main()
{
  print("hello", " ", "world", "!", "\n");
  print("this is the ", 2, "nd line.\n");

  print("decimal: ", 123, " / ", EnumValue, "\n");
  print("hex: ", FX()(123), "\n");
  print("lowercase hex: ", FX(x)(123), "\n");
  print("binary: ", FI(Bin)(123), "\n");

  print("we can also print floating point values: ", 0.124f, ".\n");
  print("sformat handles max char restrictions properly:\n", 
    FD(Group)(1234567.1234), "\n", 
    FD(Group, Max(11))(1234567.1234), "\n", 
    FD(Group, Max(10))(1234567.1234), "\n", 
    FD(Group, Max(9))(1234567.1234), "\n", 
    FD(Group, Max(8))(1234567.1234), "\n", 
    FD(Group, Max(5))(1234567.1234), "\n");

  print("even with UTF-8 chars:\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(30)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(29)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(16)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(10)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(7)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(6)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(3)), "\n",
    FS("🌵中𓁃א are four utf-8 characters", Max(2)), "\n");

  print(FS("left", Left, Min(8)), " aligned: '", FI(Left, Min(10))(12345), "'\n");
  print(FS("right", Right, Min(8)), " aligned: '", FI(Right, Min(10))(12345), "'\n");
  print(FS("center", Center, Min(8)), " aligned: '", FI(Center, Min(10))(12345), "'\n");

  print("no sign: ", 123, "\n");
  print("negative sign: ", -123, "\n");
  print("forced sign: ", FI(SBoth)(123), "\n");

  print("no fractional digits (rounded): ", FD(Frac(0))(1.98765), " / ", FD(Frac(0))(0.999), "\n");
  print("some fractional digits (rounded): ", FD(Frac(3))(1.98765), " / ", FD(Frac(3))(0.999), "\n");
  print("all fractional digits: ", FD(AllFrac)(1.98765), " / ", FD(AllFrac)(0.999), "\n");

  print("exponential fp notation: ", FD(Exp)(1.98765), " / ", FD(Exp, AllFrac)(4.72135e9), "\n");

  print("Spaces: '", FI(Min(10))(1234), "'\n");
  print("Zeroes: '", FI(Min(10), Fill0)(1234), "'\n");
  print("Grouped: '", FI(Min(10), Fill0, Group)(1234), "'\n");

  print("Custom Types: ", vec2t<float>(0.1f, -0.5f), " or ", vec3t<double>(1.4, -2.5, 0.), ".\n");

  return 0;
}

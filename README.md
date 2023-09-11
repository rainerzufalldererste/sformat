# sformat
#### An easily-integrable, extendable, typesafe, UTF-8 compliant, non-copyleft string formatter for C++17 (for Windows, but easily portable)

## Usage:

```c++
#define print(...) puts(sformat(__VA_ARGS__))

  print("this is an int: ", 2, ".");
  print("this is a float: ", 2.1, ".");
```

for easy formatting sformat provides various macros for quickly specifying the desired formatting style.

```c++
  print("decimal: ", 123);
  print("hex: ", FX()(123));
  print("lowercase hex: ", FX(x)(123));
  print("uppercase hex: ", FX(X)(123));
  print("binary: ", FI(Bin)(123));
```

**sformat** also allows for more complex formatting like
- left, right & center aligned

```c++
  print(FS("left", Left, Min(8)), " aligned: '", FI(Left, Min(10))(12345), "'");
  print(FS("right", Right, Min(8)), " aligned: '", FI(Right, Min(10))(12345), "'");
  print(FS("center", Center, Min(8)), " aligned: '", FI(Center, Min(10))(12345), "'");

  // prints:
  // left     aligned: '12345     '
  //    right aligned: '     12345'
  //  center  aligned: '  12345   '
```

- auto detected culture compliant decimal separators (can be reset with `sformatState_ResetCulture`)
- optional auto detected culture compliant digit grouping separators (SI, Chinese Numerals, Indian Numbering System, can be reset with `sformatState_ResetCulture`)
- handling maximum character restrictions properly

```c++
  print(
    FD(Group)(1234567.1234), "\n", 
    FD(Group, Max(11))(1234567.1234), "\n", 
    FD(Group, Max(10))(1234567.1234), "\n", 
    FD(Group, Max(9))(1234567.1234), "\n", 
    FD(Group, Max(8))(1234567.1234), "\n", 
    FD(Group, Max(5))(1234567.1234));

  // prints (with German decimal separator and SI grouping):
  // 1.234.567,1234
  // 1.234.567,1
  // 1.234.567
  // 1.234.567
  // >999.999
  // > 999

  // even with UTF-8 characters:
  print(
    FS("🌵中𓁃א are four utf-8 characters", Max(30)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(29)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(16)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(10)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(7)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(6)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(3)), "\n", 
    FS("🌵中𓁃א are four utf-8 characters", Max(2)));

  // prints (to a UTF-8 terminal):
  // 🌵中𓁃א are four utf-8 characters
  // 🌵中𓁃א are four utf-8 charac...
  // 🌵中𓁃א are four...
  // 🌵中𓁃א ar...
  // 🌵中𓁃א...
  // 🌵中𓁃...
  // ...
  // 🌵中
```

- forcing signs for positive and negative values

```c++
  print("no sign: ", 123);
  print("negative sign: ", -123);
  print("forced sign: ", FI(SBoth)(123));

  // prints:
  // no sign: 123
  // negative sign: -123
  // forced sign: +123
```

- specifying number of fractional digits

```c++
  print("no fractional digits (rounded): ", FD(Frac(0))(1.98765), " / ", FD(Frac(0))(0.999));
  print("some fractional digits (rounded): ", FD(Frac(3))(1.98765), " / ", FD(Frac(3))(0.999));
  print("all fractional digits: ", FD(AllFrac)(1.98765), " / ", FD(AllFrac)(0.999));

  // prints:
  // no fractional digits (rounded): 2 / 1
  // some fractional digits (rounded): 1,988 / 0,999
  // all fractional digits: 1,98765 / 0,99900
```

- exponential floating point notation

```c++
  print("exponential fp notation: ", FD(Exp)(1.98765), " / ", FD(Exp, AllFrac)(4.72135e9));
  
  // prints:
  // exponential fp notation: 1,9876e+0 / 4,72135e+9
```

- filling with zeroes

```c++
  print("Spaces: '", FI(Min(10))(1234), "'");
  print("Zeroes: '", FI(Min(10), Fill0)(1234), "'"); // `Fill0` with `Center` / `Left` is invalid!
  print("Grouped: '", FI(Min(10), Fill0, Group)(1234), "'");

  // prints:
  // Spaces: '      1234'
  // Zeroes: '0000001234'
  // Grouped: '00.001.234'
```

- custom types

```c++
template <typename T>
struct vec2t
{
  T asArray[2];
  inline vec2t(T x, T y) { asArray[0] = x; asArray[1] = y; }
};

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

  print("custom vector type: ", vec2t<float>(0.1f, -0.5f));

  // prints (with US decimal separator):
  // custom vector type: [0.1, -0.5]
```

## Buffer Options
By default sformat uses an internal buffer for formatting that is allocated with a specified allocator, but sformat can also
- calculate the maximum number of required bytes with `sformat_capacity`.
- output to a pre-allocated external buffer with `sformat_to`.

## Full Macro Feature List:
### Base Macros
| Macro | Description |
| - | - |
| **FI**(_options_)(_int_) | format signed integer with specified options |
| **FU**(_options_)(_uint_) | format unsigned integer with specified options |
| **FF**(_options_)(_float_) | format `float` with specified options |
| **FD**(_options_)(_double_) | format `double` with specified options |
| **FX**(_options_)(_uint_) | format unsigned integer as uppercase with optional specified options |
| **FS**(_string_, _options_) | format `char *` with specified options |

### Formatting Macros for everything except `FS`
these can only be used in the _options_-Field of a Base Macro (see above).
| Macro | Description |
| - | - |
| **Hex** | hexadecimal (for integers) |
| **x** | lowercase hexadecimal (for integers) |
| **X** | uppercase hexadecimal (for integers) |
| **Bin** | binary (for integers) |
| **Frac**(_uint_) | specify number of fractional digits to use (for floating point) |
| **Min**(_uint_) | minimum number of chars to output |
| **Max**(_uint_) | maximum number of chars to output |
| **Fill0** | (only if right aligned:) fill remaining space with zeroes |
| **AllFrac** | show all retrievable floating point digits (for floating point) |
| **Exp** | exponential notation (for floating point) |
| **SBoth** | show sign for positive and negative values |
| **Center** | center align |
| **Left** | align left |
| **Right** | align right |
| **Group** | group digits based on current culture |

### Formatting Macros for `FS`
these can only be used in the _options_-Field of `FS` (see above).
| Macro | Description |
| - | - |
| **Min**(_uint_) | minimum number of chars to output |
| **Max**(_uint_) | maximum number of chars to output |
| **Center** | center align |
| **Left** | align left |
| **Right** | align right |

#### Dependencies:
- [dragonbox](https://github.com/jk-jeon/dragonbox) for fast floating point to chars
- [utf8-iteraotor](https://github.com/adrianwk94/utf8-iterator) for simple utf-8 handling (easily replaceable)
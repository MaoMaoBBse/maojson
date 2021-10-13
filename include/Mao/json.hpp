#pragma once

#include <any>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace mao {

class Json {
protected:
  // 字符串
  using String = std::string;

  // key值
  class Key : public std::string {};

  // 值
  using Value = std::any;

  // 对象
  using Object = std::map<Key, Value>;

  // 数组
  using Array = std::vector<Value>;

  /**
   * 维护一个静态set容器，用于辅助 read_appoint_char() 函数
   * 释放时自动调用内部的set的clear函数。
   */
  class Chars {
  public:
    auto& getSet() {
      static std::set<char> cs;
      return cs;
    }
    ~Chars() { getSet().clear(); }
  };

private:
  // 用以存储数据的单位
  Value m_value;

protected:
  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Object：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为对象数据，当收到'}'返回。
   */
  template <typename T>
  friend T& operator>>(T&, Object&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Value：存储数据的对象
   * 作用：判断流内数据对象类型，调用对应流入函数。
   */
  template <typename T>
  friend T& operator>>(T&, Value&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Array：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为数组数据，当收到']'返回。
   */
  template <typename T>
  friend T& operator>>(T&, Array&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Key：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为字符串数据，当收到'"'返回。
   */
  template <typename T>
  friend T& operator>>(T&, Key&);

  /**
   * 重载 << 函数
   * 参数T：输出流对象
   * 参数Value：需要序列化的数据
   * 作用：将 Value 序列化并流入输出流中
   */
  template <typename T>
  friend T& operator<<(T&, const Value&);

  /**
   * 从流中读取指定字符
   * 参数T：输入流
   * 参数std::initializer_list<char>：一堆字符
   * 作用：从流中依次读取一个字符，直到读取的字符在参数中能找到。
   * 返回值：第一个布尔值为是否找到，如果找到第二个值为找到的那个字符。
   */
  template <typename T>
  static std::pair<bool, char> read_appoint_char(
      T&,
      const std::initializer_list<char>);

  Json& operator=(const Array&);
  Json& operator=(const Object&);

public:
  // 默认构造函数，以value的默认构造函数初始化自身
  Json() = default;

  // 拷贝构造
  Json(const Json&) = default;
  Json& operator=(const Json&) = default;

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 作用：将流对象中的数据反序列化并存储到 m_value 当中,
   * 流对象中的数据必须由花括号或者方括号包裹起来，非包裹起来的数据将识别为错误数据忽视处理。
   * 使用：std::cin >> json;
   */
  template <typename T>
  friend T& operator>>(T&, Json&);

  /**
   * 重载 << 函数
   * 参数T：输出流对象
   * 作用：将本身的数据序列化到输出流对象当中。
   * 使用：std::cout << json;
   */
  template <typename T>
  friend T& operator<<(T&, const Json&);

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为数字类型。
   */
  inline bool is_number() const;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为字符串类型。
   */
  inline bool is_string() const;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为数组类型。
   */
  inline bool is_array() const noexcept;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为对象。
   */
  inline bool is_object() const noexcept;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否空。
   */
  inline bool is_empty() const noexcept;

  /**
   * 清空函数
   * 作用：清空m_value所含的值
   */
  inline void reset() noexcept;
};

/**
 * 重载 << 函数
 * 参数T：输出流对象
 * 参数Value：需要序列化的数据
 * 作用：将 Value 序列化并流入输出流中
 */
template <typename T>
T& operator<<(T& os, const Json::Value& value) {
  // 当值为空时
  if (!value.has_value()) {
    // 不作任何处理

  } else if (value.type() == typeid(Json::Object)) {
    os << '{';
    auto falg{false};
    auto& obj{std::any_cast<const Json::Object&>(value)};
    for (const auto& p : obj) {
      os << '"' << dynamic_cast<const std::string&>(p.first)
         << "\":" << p.second << ',';
      if (!falg)
        falg = true;
    }
    if (falg)
      os << '\b';
    os << '}';

  } else if (value.type() == typeid(Json::Array)) {
    os << '[';
    auto falg{false};
    auto& array{std::any_cast<const Json::Array&>(value)};
    for (const auto& v : array) {
      os << v << ',';
      if (!falg)
        falg = true;
    }
    if (falg)
      os << '\b';
    os << "]";

  } else if (value.type() == typeid(int)) {
    os << std::any_cast<const int&>(value);

  } else if (value.type() == typeid(double)) {
    os << std::any_cast<const double&>(value);

  } else if (value.type() == typeid(std::string)) {
    os << '"' << std::any_cast<const std::string&>(value) << '"';

  } else if (value.type() == typeid(bool)) {
    if (std::any_cast<bool>(value))
      os << "true";
    else
      os << "false";
  } else {
    // 忽略
  }
  return os;
}

/**
 * 重载 << 函数
 * 参数T：输出流对象
 * 作用：将本身的数据序列化到输出流对象当中。
 * 使用：std::cout << json;
 */
template <typename T>
T& operator<<(T& os, const Json& json) {
  os << json.m_value;
  return os;
}

/**
 * 判断函数
 * 作用：判断当前储存的值是否为数字类型。
 */
bool Json::is_number() const {
  return m_value.type() == typeid(double) || m_value.type() == typeid(int);
}

/**
 * 判断函数
 * 作用：判断当前储存的值是否为字符串类型。
 */
bool Json::is_string() const {
  return m_value.type() == typeid(std::string);
}

/**
 * 判断函数
 * 作用：判断当前储存的值是否为数组类型。
 */
bool Json::is_array() const noexcept {
  return m_value.type() == typeid(Array);
}

/**
 * 判断函数
 * 作用：判断当前储存的值是否为对象。
 */
bool Json::is_object() const noexcept {
  return m_value.type() == typeid(Object);
}

/**
 * 判断函数
 * 作用：判断当前储存的值是否空。
 */
bool Json::is_empty() const noexcept {
  return m_value.has_value();
}

/** 清空函数 **/
void Json::reset() noexcept {
  if (!m_value.has_value())
    m_value.reset();
}

Json& Json::operator=(const Array& array) {
  m_value = array;
  return *this;
}

Json& Json::operator=(const Object& object) {
  m_value = object;
  return *this;
}

/** 重载 >> 函数  参数：输入流 与 Value。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Value& value) {
  char c;
  is >> c;

  if (!is.good())
    return is;

  auto numChar{[](const char c_) { return c_ >= '0' && c_ <= '9'; }};

  // 判断数据类型为字符串
  if (c == '"') {
    value = Json::Key{};
    is >> std::any_cast<Json::Key&>(value);
    value = dynamic_cast<std::string&>(std::any_cast<Json::Key&>(value));

    // 判断数据类型为对象
  } else if (c == '{') {
    value = Json::Object{};
    is >> std::any_cast<Json::Object&>(value);

    // 判断数据类型为数组
  } else if (c == '[') {
    value = Json::Array{};
    is >> std::any_cast<Json::Array&>(value);

    // 判断数据类型为布尔值并且为true
  } else if (c == 't') {
    is >> c;
    if (c == 'r') {
      is >> c;
      if (c == 'u') {
        is >> c;
        if (c == 'e') {
          value = true;
        }
      }
    }

    // 判断数据类型为布尔值并且为false
  } else if (c == 'f') {
    is >> c;
    if (c == 'a') {
      is >> c;
      if (c == 'l') {
        is >> c;
        if (c == 's') {
          is >> c;
          if (c == 'e') {
            value = false;
          }
        }
      }
    }

    // 判断数据类型为数字
  } else if (numChar(c)) {
    // 使用seekg()函数时，如果是fstream打开开方式需要是ios::binary
    is.seekg(-1, std::ios::cur);
    double num;

    is >> num;
    if (num == static_cast<int>(num)) {
      value = static_cast<int>(num);
    } else {
      value = num;
    }
  }

  return is;
}

/** 重载 >> 函数  参数：输入流 与 Key。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Key& key) {
  char c;
  while (true) {
    is >> c;

    // 判断是否是转义字符，如果是无条件记录下一个字符
    if (is.good() && c == '\\') {
      is >> c;
      if (is.good()) {
        key += c;
        is >> c;
      }
    }

    // 异常流对象或者接收到双引号，退出循环
    if (!is.good() || c == '"')
      break;

    key += c;
  }
  return is;
}

/** 重载 >> 函数  参数：输入流 与 Array。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Array& array) {
  if (!is.good())
    return is;

  Json::Value value;
  while (true) {
    is >> value;
    array.push_back(value);
    auto reuslt{Json::read_appoint_char(is, {',', ']'})};

    if ((!is.good()) || (reuslt.first && reuslt.second == ']'))
      break;
  }

  return is;
}

/** 重载 >> 函数  参数：输入流 与 Object。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Object& object) {
  if (!is.good())
    return is;

  while (true) {
    // 忽略 字符串左 '"'
    if (!Json::read_appoint_char(is, {'"'}).first)
      return is;  // 输入流异常，直接返回

    // 读取 Key
    Json::Key key;
    is >> key;

    // 忽略 中间字符 ':'
    if (!Json::read_appoint_char(is, {':'}).first)
      return is;  // 输入流异常，直接返回

    Json::Value value;
    is >> value;

    if (value.has_value())
      object.insert(std::make_pair(std::move(key), std::move(value)));

    auto result{Json::read_appoint_char(is, {',', '}'})};

    if (!result.first || result.second == '}')
      break;  // 输入流异常或者找到右花括号结束循环
  }
  return is;
}

/** json 的重载 >> 函数，详细见声明。 **/
template <typename T>
T& operator>>(T& is, Json& json) {
  // 判断输入流是否正常,异常则直接退出。
  if (!is.good())
    return is;

  // 清空旧数据
  json.reset();

  // 清空错误数据，并找到合适开头
  auto reuslt = Json::read_appoint_char(is, {'[', '{'});

  // 回退流内置指针， 保障后面流入时数据是由括号包裹。
  if (reuslt.first && reuslt.second == '[') {
    Json::Array array;
    is >> array;
    json = std::move(array);

  } else if (reuslt.first && reuslt.second == '{') {
    Json::Object object;
    is >> object;
    json = std::move(object);
  }

  return is;
}

/** 读取指定字符函数的实现，详细见声明。 **/
template <typename T>
std::pair<bool, char> Json::read_appoint_char(
    T& is,
    const std::initializer_list<char> init_chars) {
  char c;

  // 构建一个由一堆需要查找的字符组成的树，便于后续查找
  Chars chars;
  auto& set_chars{chars.getSet()};
  set_chars.insert(init_chars.begin(), init_chars.end());

  while (true) {
    if (!is.good()) {
      return std::make_pair<bool, char>(false, char{});
    }
    is >> c;
    auto iter{set_chars.find(c)};
    if (iter != set_chars.end())
      break;
  }

  return std::make_pair(true, c);
}
}  // namespace mao
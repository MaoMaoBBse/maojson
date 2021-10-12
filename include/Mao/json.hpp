#pragma once
#include <any>
#include <initializer_list>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mao {

class Json {
private:
  /**
   * json 基本数据类型
   */
  // 数字型
  using Number = std::variant<int, double, float, unsigned int, long long>;

  // 字符串
  using String = std::string;

  // key值
  using Key = std::string;

  // 值
  using Value = std::any;

  // 对象
  using Object = std::unordered_map<Key, Value>;

  // 数组
  using Array = std::vector<Value>;

  // 用以存储数据的单位
  Value m_value;

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Object：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为对象数据，也就是说输入流中有由花括号包裹起来的数据。
   */
  template <typename T>
  friend T& operator>>(T&, Object&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Array：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为数组数据，也就是说输入流中有由方括号包裹起来的数据。
   */
  template <typename T>
  friend T& operator>>(T&, Array&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数String：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为字符串数据，也就是说输入流中有由双引号包裹起来的数据。
   */
  template <typename T>
  friend T& operator>>(T&, String&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数bool：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为布尔值数据，也就是说输入流中有由
   * true/false 的连续字符数据。
   */
  template <typename T>
  friend T& operator>>(T&, bool&);

  /**
   * 重载 >> 函数
   * 参数T: 输入流对象
   * 参数Number：存储数据的对象
   * 作用：此时已经知道需要反序列化的数据为数字数据，也就是说输入流中有由全数字的连续字符数据。
   */
  template <typename T>
  friend T& operator>>(T&, Number&);

  /**
   * 从流中读取指定字符
   * 参数T：输入流
   * 参数std::initializer_list<char>&：一堆字符
   * 作用：从流中依次读取一个字符，直到读取的字符在参数中能找到。
   * 返回值：第一个布尔值为是否找到，如果找到第二个值为找到的那个字符。
   */
  template <typename T>
  static std::pair<bool, char> read_appoint_char(
      T&,
      const std::initializer_list<char>&);

public:
  // 默认构造函数，以value的默认构造函数初始化自身
  Json() = default;

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
   * 作用：判断当前储存的值是否为布尔值类型。
   */
  bool is_bool() const;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为数字类型。
   */
  bool is_number() const;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为字符串类型。
   */
  bool is_string() const;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为数组类型。
   */
  bool is_array() const noexcept;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否为对象。
   */
  bool is_object() const noexcept;

  /**
   * 判断函数
   * 作用：判断当前储存的值是否空。
   */
  bool is_empty() const noexcept;

  /**
   * 清空函数
   * 作用：清空m_value所含的值
   */
  void reset() noexcept;
};

/** 重载 >> 函数  参数：输入流 与 Array。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Array& array) {
  if (!is.good())
    return is;

  char c;
  // 忽略第一个字符 '['
  is >> c;

  Json::Value value;
  do {
    is >> value;
    array.push_back(value);
    auto reuslt{Json::read_appoint_char(is, ',', ']')};
  } while (is.good() && reuslt.first && reuslt.second != ']');
  return is;
}

/** 重载 >> 函数  参数：输入流 与 Object。详细见声明 **/
template <typename T>
T& operator>>(T& is, Json::Object& object) {
  if (!is.good())
    return is;

  char c;
  // 忽略第一个字符 '{'
  is >> c;

  // 读取 Key
}

/** json 的重载 >> 函数，详细见声明。 **/
template <typename T>
T& operator>>(T& is, Json& json) {
  // 判断输入流是否正常,异常则直接退出。
  if (is.good())
    return is;

  // 清空旧数据
  json.reset();

  // 清空错误数据，并找到合适开头
  auto reuslt = Json::read_appoint_char(is, '[', '{');
  if (reuslt.first && reuslt.second == '[') {
    // 回退流内置指针， 保障后面流入时数据是由方括号包裹。
    is.seekg(-1, std::ios::cur);

    Json::Array array;
    is >> array;
    json = std::move(array);
  } else {
    // 回退流内置指针， 保障后面流入时数据是由花括号包裹。
    is.seekg(-1, std::ios::cur);

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
    const std::initializer_list<char>& init_chars) {
  char c;

  // 构建一个由一堆需要查找的字符组成的树，便于后续查找
  std::set<char> set_chars{init_chars.begin(), init_chars.end()};

  while (true) {
    if (!is.good()) {
      return std::make_pair<bool, char>(false, char{});
    }
    is >> c;
    auto iter{set_chars.find(c)};
    if (iter != set_chars.end())
      break;
  }

  return std::make_pair<bool, char>(true, c);
}
}  // namespace mao
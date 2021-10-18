# maojson

# 简介

使用C++编写的简单json库，需要C++17编译支持。只需包含头文件 include”mao/json.hpp“ 即可。

---

# 使用方法

## 反序列化

你可以使用标准库提供的输入流进行反序列化。

```C++
std::ifstream ifile{"test.txt"};
mao::json json;
ifile >> json;
```

## 序列化

同样，你也可以以流的形式序列化。

```C++
std::cout << json << std::endl;
```




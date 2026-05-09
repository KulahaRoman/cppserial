#pragma once
#include <map>
#include <memory>

namespace CppSerial {
class Context {
  friend class Serializer;

 public:
  Context() = default;
  Context(const Context& other) = default;
  Context(Context&& other) = default;

 private:
  int referenceCount = 0;

  std::map<std::shared_ptr<void>, int> referenceMap;
  std::map<int, std::shared_ptr<void>> reverseReferenceMap;
};
}  // namespace CppSerial

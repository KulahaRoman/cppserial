#pragma once
#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "binaryarchive.h"
#include "endiannessprovider.h"
#include "serializable.h"

namespace CppSerial {
// Singletone class that does actual serialization.
// Supports integral types, enums, strings, containers and
// custom Serializable class.

class Serializer {
 public:
  template <class T>
  static void Serialize(const T value, BinaryArchive& archive) {
    Context context;
    Serialize(value, archive, context);
  }

  template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  static void Serialize(const T value, BinaryArchive& archive,
                        const Context& context) {
    try {
      T temp{};

      if (EndiannessProvider::GetSystemEndianness() ==
          EndiannessProvider::Endianness::LITTLE) {
        temp = swapBytes(value);
      } else {
        temp = value;
      }

      archive.Write(reinterpret_cast<const unsigned char*>(&temp),
                    sizeof(temp));
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize integral type value: " +
                               std::string{e.what()});
    }
  }

  template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
  static void Serialize(const T value, BinaryArchive& archive,
                        const Context& context) {
    try {
      Serialize(static_cast<std::underlying_type_t<T>>(value), archive,
                context);
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize enum type value: " +
                               std::string{e.what()});
    }
  }

  static void Serialize(const std::string& str, BinaryArchive& archive,
                        const Context& context) {
    try {
      auto strSizeInBytes = static_cast<uint64_t>(str.size());

      Serialize(strSizeInBytes, archive, context);

      archive.Write(reinterpret_cast<const unsigned char*>(str.data()),
                    static_cast<std::size_t>(strSizeInBytes));
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::string: " +
                               std::string{e.what()});
    }
  }

  static void Serialize(const std::wstring& wstr, BinaryArchive& archive,
                        const Context& context) {
    try {
      auto strSizeInBytes =
          static_cast<uint64_t>(wstr.size() * sizeof(wchar_t));

      Serialize(strSizeInBytes, archive, context);

      archive.Write(reinterpret_cast<const unsigned char*>(wstr.data()),
                    static_cast<std::size_t>(strSizeInBytes));
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::wstring: " +
                               std::string{e.what()});
    }
  }

  template <class T1, class T2>
  static void Serialize(const std::pair<T1, T2>& pair, BinaryArchive& archive,
                        Context& context) {
    try {
      Serialize(pair.first, archive, context);
      Serialize(pair.second, archive, context);
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::pair<T1,T2>: " +
                               std::string{e.what()});
    }
  }

  template <class T, std::size_t N>
  static void Serialize(const std::array<T, N>& array, BinaryArchive& archive,
                        Context& context) {
    try {
      for (const auto& value : array) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::array<T,N>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Compare, class Allocator>
  static void Serialize(const std::map<K, V, Compare, Allocator>& map,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(map.size()), archive, context);

      for (const auto& [key, value] : map) {
        Serialize(key, archive, context);
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::map<K,V>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Compare, class Allocator>
  static void Serialize(const std::multimap<K, V, Compare, Allocator>& multimap,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(multimap.size()), archive, context);

      for (const auto& [key, value] : multimap) {
        Serialize(key, archive, context);
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::multimap<K,V>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Hash, class KeyEqual, class Allocator>
  static void Serialize(
      const std::unordered_map<K, V, Hash, KeyEqual, Allocator>& umap,
      BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(umap.size()), archive, context);

      for (const auto& [key, value] : umap) {
        Serialize(key, archive, context);
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::unordered_map<K,V>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Hash, class KeyEqual, class Allocator>
  static void Serialize(
      const std::unordered_multimap<K, V, Hash, KeyEqual, Allocator>& umultimap,
      BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(umultimap.size()), archive, context);

      for (const auto& [key, value] : umultimap) {
        Serialize(key, archive, context);
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to serialize std::unordered_multimap<K,V>: " +
          std::string{e.what()});
    }
  }

  template <class T, class Compare, class Allocator>
  static void Serialize(const std::set<T, Compare, Allocator>& set,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(set.size()), archive, context);

      for (const auto& value : set) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::set<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Compare, class Allocator>
  static void Serialize(const std::multiset<T, Compare, Allocator>& multiset,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(multiset.size()), archive, context);

      for (const auto& value : multiset) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::multiset<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Hash, class KeyEqual, class Allocator>
  static void Serialize(
      const std::unordered_set<T, Hash, KeyEqual, Allocator>& uset,
      BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(uset.size()), archive, context);

      for (const auto& value : uset) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::unordered_set<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Hash, class KeyEqual, class Allocator>
  static void Serialize(
      const std::unordered_multiset<T, Hash, KeyEqual, Allocator>& umultiset,
      BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(umultiset.size()), archive, context);

      for (const auto& value : umultiset) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to serialize std::unordered_multiset<T>: " +
          std::string{e.what()});
    }
  }

  template <class T, class Container>
  static void Serialize(const std::stack<T, Container>& stack,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(stack.size()), archive, context);

      auto stackCopy = stack;

      while (!stackCopy.empty()) {
        Serialize(stackCopy.top(), archive, context);
        stackCopy.pop();
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::stack<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Container>
  static void Serialize(const std::queue<T, Container>& queue,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(queue.size()), archive, context);

      auto queueCopy = queue;

      while (!queueCopy.empty()) {
        Serialize(queueCopy.front(), archive, context);
        queueCopy.pop();
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::queue<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Container, class Compare>
  static void Serialize(
      const std::priority_queue<T, Container, Compare>& pqueue,
      BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(pqueue.size()), archive, context);

      auto pqueueCopy = pqueue;

      while (!pqueueCopy.empty()) {
        Serialize(pqueueCopy.top(), archive, context);
        pqueueCopy.pop();
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::priority_queue<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Serialize(const std::deque<T, Allocator>& deque,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(deque.size()), archive, context);

      for (const auto& value : deque) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::deque<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Serialize(const std::list<T, Allocator>& list,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(list.size()), archive, context);

      for (const auto& value : list) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::list<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Serialize(const std::forward_list<T, Allocator>& flist,
                        BinaryArchive& archive, Context& context) {
    try {
      auto initialArchivePosition = archive.GetWritePosition();

      auto size = 0ull;
      Serialize(size, archive, context);

      for (const auto& value : flist) {
        Serialize(value, archive, context);
        size++;
      }

      auto currentArchivePosition = archive.GetWritePosition();
      archive.SetWritePosition(initialArchivePosition);

      Serialize(size, archive, context);

      archive.SetWritePosition(currentArchivePosition);
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::forward_list<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Serialize(const std::vector<T, Allocator>& vector,
                        BinaryArchive& archive, Context& context) {
    try {
      Serialize(static_cast<uint64_t>(vector.size()), archive, context);

      for (const auto& value : vector) {
        Serialize(value, archive, context);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to serialize std::vector<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Deleter>
  static void Serialize(const std::unique_ptr<T, Deleter>& pointer,
                        BinaryArchive& archive, Context& context) {
    auto pointerState = pointer ? PointerState::VALID : PointerState::INVALID;
    Serialize(pointerState, archive, context);

    if (pointerState == PointerState::VALID) {
      Serialize(*pointer, archive, context);
    }
  }

  template <class T>
  static void Serialize(const std::shared_ptr<T>& pointer,
                        BinaryArchive& archive, Context& context) {
    auto pointerState = pointer ? PointerState::VALID : PointerState::INVALID;
    Serialize(pointerState, archive, context);

    auto erasedSharedPointer = std::static_pointer_cast<void>(pointer);

    if (pointerState == PointerState::VALID) {
      if (!context.referenceMap.count(erasedSharedPointer)) {
        context.referenceMap.emplace(erasedSharedPointer,
                                     context.referenceCount);

        Serialize(context.referenceCount++, archive, context);
        Serialize(*pointer, archive, context);
      } else {
        auto index = context.referenceMap[erasedSharedPointer];
        Serialize(index, archive, context);
      }
    }
  }

  template <class T>
  static void Serialize(const std::weak_ptr<T>& pointer, BinaryArchive& archive,
                        Context& context) {
    auto lockedSharedPointer = pointer.lock();
    if (lockedSharedPointer) {
      Serialize(lockedSharedPointer, archive, context);
    }
  }

  static void Serialize(const Serializable& object, BinaryArchive& archive,
                        Context& context) {
    object.Serialize(archive, context);
  }

  template <class T>
  static void Deserialize(T& value, BinaryArchive& archive) {
    Context context;
    Deserialize(value, archive, context);
  }

  template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  static void Deserialize(T& value, BinaryArchive& archive,
                          const Context& context) {
    try {
      T temp{};

      archive.Read(reinterpret_cast<unsigned char*>(&temp), sizeof(temp));

      if (EndiannessProvider::GetSystemEndianness() ==
          EndiannessProvider::Endianness::LITTLE) {
        value = swapBytes(temp);
      } else {
        value = temp;
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize integral type value: " +
                               std::string{e.what()});
    }
  }

  template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
  static void Deserialize(T& value, BinaryArchive& archive,
                          const Context& context) {
    try {
      Deserialize(reinterpret_cast<std::underlying_type_t<T>&>(value), archive,
                  context);
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize enum type value: " +
                               std::string{e.what()});
    }
  }

  static void Deserialize(std::string& str, BinaryArchive& archive,
                          const Context& context) {
    try {
      auto strSizeInBytes = 0ull;

      Deserialize(strSizeInBytes, archive, context);

      str.resize(static_cast<std::size_t>(strSizeInBytes));

      archive.Read(reinterpret_cast<unsigned char*>(str.data()),
                   static_cast<std::size_t>(strSizeInBytes));
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::string: " +
                               std::string{e.what()});
    }
  }

  static void Deserialize(std::wstring& wstr, BinaryArchive& archive,
                          const Context& context) {
    try {
      auto strSizeInBytes = 0ull;

      Deserialize(strSizeInBytes, archive, context);

      wstr.resize(static_cast<std::size_t>(strSizeInBytes / sizeof(wchar_t)));

      archive.Read(reinterpret_cast<unsigned char*>(wstr.data()),
                   static_cast<std::size_t>(strSizeInBytes));
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::wstring: " +
                               std::string{e.what()});
    }
  }

  template <class T1, class T2>
  static void Deserialize(std::pair<T1, T2>& pair, BinaryArchive& archive,
                          Context& context) {
    try {
      T1&& first{};
      T2&& second{};

      Deserialize(first, archive, context);
      Deserialize(second, archive, context);

      pair.first = first;
      pair.second = second;
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::pair<T1,T2>: " +
                               std::string{e.what()});
    }
  }

  template <class T, std::size_t N>
  static void Deserialize(std::array<T, N>& array, BinaryArchive& archive,
                          Context& context) {
    try {
      for (auto i = 0ull; i < N; i++) {
        T&& value{};
        Deserialize(value, archive, context);
        array[i] = std::move(value);
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::array<T,N>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Compare, class Allocator>
  static void Deserialize(std::map<K, V, Compare, Allocator>& map,
                          BinaryArchive& archive, Context& context) {
    auto mapSize = 0ull;

    try {
      Deserialize(mapSize, archive, context);

      for (auto i = 0ull; i < mapSize; i++) {
        K&& key{};
        V&& value{};

        Deserialize(key, archive, context);
        Deserialize(value, archive, context);

        map.emplace(std::move(key), std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::map<K,V>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Compare, class Allocator>
  static void Deserialize(std::multimap<K, V, Compare, Allocator>& multimap,
                          BinaryArchive& archive, Context& context) {
    auto mapSize = 0ull;

    try {
      Deserialize(mapSize, archive, context);

      for (auto i = 0ull; i < mapSize; i++) {
        K&& key{};
        V&& value{};

        Deserialize(key, archive, context);
        Deserialize(value, archive, context);

        multimap.emplace(std::move(key), std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::multimap<K,V>: " +
                               std::string{e.what()});
    }
  }

  template <class K, class V, class Hash, class KeyEqual, class Allocator>
  static void Deserialize(
      std::unordered_map<K, V, Hash, KeyEqual, Allocator>& umap,
      BinaryArchive& archive, Context& context) {
    auto mapSize = 0ull;

    try {
      Deserialize(mapSize, archive, context);

      for (auto i = 0ull; i < mapSize; i++) {
        K&& key{};
        V&& value{};

        Deserialize(key, archive, context);
        Deserialize(value, archive, context);

        umap.emplace(std::move(key), std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to deserialize std::unordered_map<K,V>: " +
          std::string{e.what()});
    }
  }

  template <class K, class V, class Hash, class KeyEqual, class Allocator>
  static void Deserialize(
      std::unordered_multimap<K, V, Hash, KeyEqual, Allocator>& umultimap,
      BinaryArchive& archive, Context& context) {
    auto mapSize = 0ull;

    try {
      Deserialize(mapSize, archive, context);

      for (auto i = 0ull; i < mapSize; i++) {
        K&& key{};
        V&& value{};

        Deserialize(key, archive, context);
        Deserialize(value, archive, context);

        umultimap.emplace(std::move(key), std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to deserialize std::unordered_multimap<K,V>: " +
          std::string{e.what()});
    }
  }

  template <class T, class Compare, class Allocator>
  static void Deserialize(std::set<T, Compare, Allocator>& set,
                          BinaryArchive& archive, Context& context) {
    auto setSize = 0ull;

    try {
      Deserialize(setSize, archive, context);

      for (auto i = 0ull; i < setSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        set.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::set<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Compare, class Allocator>
  static void Deserialize(std::multiset<T, Compare, Allocator>& multiset,
                          BinaryArchive& archive, Context& context) {
    auto setSize = 0ull;

    try {
      Deserialize(setSize, archive, context);

      for (auto i = 0ull; i < setSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        multiset.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::multiset<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Hash, class KeyEqual, class Allocator>
  static void Deserialize(
      std::unordered_set<T, Hash, KeyEqual, Allocator>& uset,
      BinaryArchive& archive, Context& context) {
    auto setSize = 0ull;

    try {
      Deserialize(setSize, archive, context);

      for (auto i = 0ull; i < setSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        uset.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::unordered_set<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Hash, class KeyEqual, class Allocator>
  static void Deserialize(
      std::unordered_multiset<T, Hash, KeyEqual, Allocator>& umultiset,
      BinaryArchive& archive, Context& context) {
    auto setSize = 0ull;

    try {
      Deserialize(setSize, archive, context);

      for (auto i = 0ull; i < setSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        umultiset.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to deserialize std::unordered_multiset<T>: " +
          std::string{e.what()});
    }
  }

  template <class T, class Container>
  static void Deserialize(std::stack<T, Container>& stack,
                          BinaryArchive& archive, Context& context) {
    auto stackSize = 0ull;

    std::stack<T> helper;

    try {
      Deserialize(stackSize, archive, context);

      for (auto i = 0ull; i < stackSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        helper.emplace(std::move(value));
      }

      while (!helper.empty()) {
        stack.emplace(helper.top());
        helper.pop();
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::stack<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Container>
  static void Deserialize(std::queue<T, Container>& queue,
                          BinaryArchive& archive, Context& context) {
    auto queueSize = 0ull;

    try {
      Deserialize(queueSize, archive, context);

      for (auto i = 0ull; i < queueSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        queue.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::queue<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Container, class Compare>
  static void Deserialize(std::priority_queue<T, Container, Compare>& pqueue,
                          BinaryArchive& archive, Context& context) {
    auto pqueueSize = 0ull;

    try {
      Deserialize(pqueueSize, archive, context);

      for (auto i = 0ull; i < pqueueSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        pqueue.emplace(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "failed to deserialize std::priority_queue<T>: " +
          std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Deserialize(std::deque<T, Allocator>& deque,
                          BinaryArchive& archive, Context& context) {
    auto dequeSize = 0ull;

    try {
      Deserialize(dequeSize, archive, context);

      for (auto i = 0ull; i < dequeSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        deque.emplace_back(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::deque<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Deserialize(std::list<T, Allocator>& list, BinaryArchive& archive,
                          Context& context) {
    auto listSize = 0ull;

    try {
      Deserialize(listSize, archive, context);

      for (auto i = 0ull; i < listSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        list.emplace_back(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::list<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Deserialize(std::forward_list<T, Allocator>& flist,
                          BinaryArchive& archive, Context& context) {
    auto listSize = 0ull;

    try {
      Deserialize(listSize, archive, context);

      for (auto i = 0ull; i < listSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        flist.emplace_front(std::move(value));
      }

      flist.reverse();

    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::forward_list<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Allocator>
  static void Deserialize(std::vector<T, Allocator>& vector,
                          BinaryArchive& archive, Context& context) {
    auto vectorSize = 0ull;

    try {
      Deserialize(vectorSize, archive, context);

      for (auto i = 0ull; i < vectorSize; i++) {
        T&& value{};

        Deserialize(value, archive, context);

        vector.emplace_back(std::move(value));
      }
    } catch (const std::exception& e) {
      throw std::runtime_error("failed to deserialize std::vector<T>: " +
                               std::string{e.what()});
    }
  }

  template <class T, class Deleter>
  static void Deserialize(std::unique_ptr<T, Deleter>& pointer,
                          BinaryArchive& archive, Context& context) {
    auto pointerState = PointerState::NONE;
    Deserialize(pointerState, archive, context);

    if (pointerState == PointerState::VALID) {
      pointer = std::make_unique<T>();
      Deserialize(*pointer, archive, context);
    }
  }

  template <class T>
  static void Deserialize(std::shared_ptr<T>& pointer, BinaryArchive& archive,
                          Context& context) {
    auto pointerState = PointerState::NONE;
    Deserialize(pointerState, archive, context);

    if (pointerState == PointerState::VALID) {
      auto index = UNDEFINED_REFERENCE_INDEX;
      Deserialize(index, archive, context);

      if (!context.reverseReferenceMap.count(index)) {
        pointer = std::make_shared<T>();

        auto erasedSharedPointer = std::static_pointer_cast<void>(pointer);

        context.reverseReferenceMap.emplace(index, erasedSharedPointer);

        Deserialize(*pointer, archive, context);
      } else {
        pointer =
            std::static_pointer_cast<T>(context.reverseReferenceMap[index]);
      }
    }
  }

  template <class T>
  static void Deserialize(std::weak_ptr<T>& pointer, BinaryArchive& archive,
                          Context& context) {
    auto lockedSharedPointer = pointer.lock();
    Deserialize(lockedSharedPointer, archive, context);

    pointer = lockedSharedPointer;
  }

  static void Deserialize(Serializable& object, BinaryArchive& archive,
                          Context& context) {
    object.Deserialize(archive, context);
  }

 private:
  template <class T, class Enable = typename std::enable_if<
                         std::is_integral<T>::value>::type>
  static T swapBytes(T value) noexcept {
    T temp{value};

    unsigned char* start = reinterpret_cast<unsigned char*>(&temp);
    unsigned char* end = start + sizeof(temp);

    std::reverse(start, end);

    return temp;
  }

  static const int UNDEFINED_REFERENCE_INDEX;
  static const int UNDEFINED_OBJECT_UNIQUE_ID;

  enum class PointerState { NONE, VALID, INVALID };
};
}  // namespace CppSerial

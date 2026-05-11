#pragma once
#include "binaryarchive.h"
#include "context.h"

namespace CppSerial {
// Represents interface that marks entity so it can be serialized/deserialized.
// Each class which derives 'Serializable' must provide a default
// constructor, and must override all of pure virtual methods.

class Serializable {
 public:
  virtual void Serialize(BinaryArchive& archive,
                         Context& context = Context()) const = 0;
  virtual void Deserialize(BinaryArchive& archive,
                           Context& context = Context()) = 0;

  virtual ~Serializable() = default;
};
}  // namespace CppSerial

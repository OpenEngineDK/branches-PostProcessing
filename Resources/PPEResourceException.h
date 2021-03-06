#ifndef __PPERESOURCEEXCEPTION_H__
#define __PPERESOURCEEXCEPTION_H__

// we use this instead of Resources/Exceptions.h, as #ifndef (etc) are missing from Resources/Exception.h, leading to
// multiple class definition errors.

#include <Core/Exceptions.h>

namespace OpenEngine {
namespace Resources {

using OpenEngine::Core::Exception;

class PPEResourceException : public Exception {
  private:
  const char* message;
  public:
    PPEResourceException() : Exception() {};
    PPEResourceException(const char* message);
    const char* toString();
};

} // NS Resources
} // NS OpenEngine

#endif

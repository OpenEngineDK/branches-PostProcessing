#ifndef __POSTPROCESSINGEXCEPTION_H__
#define __POSTPROCESSINGEXCEPTION_H__

#include <Core/Exceptions.h>

namespace OpenEngine {
namespace PostProcessing {

using OpenEngine::Core::Exception;
using std::string;

/** Exception used by post-processing classes
 *  @author Bjarke N. Laustsen
 */
class PostProcessingException : public Exception {
  public:
    PostProcessingException();
    PostProcessingException(std::string msg);
    virtual ~PostProcessingException() throw () {}
};

} // NS PostProcessing
} // NS OpenEngine

#endif


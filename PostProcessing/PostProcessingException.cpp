/* @author Bjarke N. Laustsen
 */

#include "PostProcessingException.h"
#include <stdio.h>
#include <Logging/Logger.h>

namespace OpenEngine {
namespace PostProcessing {

PostProcessingException::PostProcessingException() : Exception() {
}

PostProcessingException::PostProcessingException(std::string msg) : Exception(msg) {}

} // NS PostProcessing
} // NS OpenEngine

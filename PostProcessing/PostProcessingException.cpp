/* @author Bjarke N. Laustsen
 */

#include "PostProcessingException.h"
#include <stdio.h>

namespace OpenEngine {
namespace PostProcessing {

PostProcessingException::PostProcessingException() : Exception() {
}

PostProcessingException::PostProcessingException(string msg) : Exception(msg) {
    this->msg = msg;

    // the following is temp!
    printf("PostProcessingException: %s\n", msg);

    // the following is EXTREMELY temp! (to make visual studio start its debugger)
    PostProcessingException* null = NULL;
    null->toString();
}

string PostProcessingException::toString() {
    return msg;
}

} // NS PostProcessing
} // NS OpenEngine

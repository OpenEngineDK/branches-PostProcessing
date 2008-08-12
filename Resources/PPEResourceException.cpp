#include "PPEResourceException.h"
#include <stdio.h>

namespace OpenEngine {
namespace Resources {

PPEResourceException::PPEResourceException(char* message) : Exception(message) {
    this->message = message;

    // the following is temp!
    printf("PPEResourceException: %s\n", message);

    PPEResourceException* null = NULL;
    null->toString();
}

char* PPEResourceException::toString() {
    return message;
}

} // NS Resources
} // NS OpenEngine

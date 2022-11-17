#include "pxt.h"
#include "customlib.h"

namespace customlib {

//================================================================
#if MICROBIT_CODAL
//================================================================
    
    /**
     * counter
     * for micro:bit v2
     * 
     */
    int counter(int value) {
        return --value < minvalue ? maxvalue : value;
    }

//================================================================
#else // MICROBIT_CODAL
//================================================================
    
    /**
     * counter
     * for micro:bit v1
     * 
     */
    int counter(int value) {
        return ++value > maxvalue ? minvalue : value;
    }

//================================================================
#endif // MICROBIT_CODAL
//================================================================

}
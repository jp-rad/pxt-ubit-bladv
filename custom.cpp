#include "pxt.h"
#include "lib/customlib.h"

namespace custom {
    
    //%
    int bar(int v) {
        return customlib::counter(v);
    }

}
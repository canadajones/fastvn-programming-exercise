
#include <cstdint>
#include <unordered_map>


#include "metasyntax/metasyntax.h"

#include "scriptflow/scriptflow.h"


namespace translation {


// TODO: Minimise type switching, ideally we'd handle it as a value problem

struct Object {
    std::unordered_map<std::string, void*> data;
};

}
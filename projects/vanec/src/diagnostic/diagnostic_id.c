#include "vanec/diagnostic/diagnostic_id.h"

#include "vanec/utils/defines.h"

const char* get_diagnostic_text(const DiagnosticId id) {
    switch (id) {
#define DIAG(ID, TYPE, LEVEL, MSG) case ID: return MSG;
#include "vanec/diagnostic/diagnostic.def"
    };
    return NULL;
}

const char* get_diagnostic_type(const DiagnosticId id) {
    switch (id) {
#define DIAG(ID, TYPE, LEVEL, MSG) case ID: return #TYPE;
#include "vanec/diagnostic/diagnostic.def"
    };
    return NULL;
}

const char* get_diagnostic_level(const DiagnosticId id) {
    switch (id) {
#define DIAG(ID, TYPE, LEVEL, MSG) case ID: return #LEVEL;
#include "vanec/diagnostic/diagnostic.def"
    };
    return NULL;
}
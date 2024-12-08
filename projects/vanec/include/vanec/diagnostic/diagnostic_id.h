#pragma once

typedef enum {
#define DIAG(ID, TYPE, LEVEL, MSG) ID,
#include "vanec/diagnostic/diagnostic.def"
} DiagnosticId;

const char* get_diagnostic_text(const DiagnosticId id);

const char* get_diagnostic_type(const DiagnosticId id);

const char* get_diagnostic_level(const DiagnosticId id);
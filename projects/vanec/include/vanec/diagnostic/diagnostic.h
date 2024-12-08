#pragma once

#include "vanec/utils/vector.h"

#include "vanec/diagnostic/diagnostic_id.h"
#include "vanec/utils/source_loc.h"

typedef struct {
    DiagnosticId id;
    char* msg;
    SourceLoc loc;
} DiagnosticMsg;

typedef struct {
    Vector msgs;
} DiagnosticEngine;

DiagnosticMsg* diagnostic_msg_create(const DiagnosticId id, char* msg, const SourceLoc loc);

void diagnostic_msg_free(DiagnosticMsg* msg);

DiagnosticEngine diagnostic_engine_create();

void diagnostic_engine_clear(DiagnosticEngine* diag);

void diagnostic_engine_free(DiagnosticEngine* diag);

void diagnostic_engine_report(DiagnosticEngine* engine, const DiagnosticId id, const SourceLoc loc, ...);

void diagnostic_engine_print_all(DiagnosticEngine* diag);
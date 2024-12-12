#include "vanec/diagnostic/diagnostic.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

DiagnosticMsg* diagnostic_msg_create(const DiagnosticId id, char* msg, const SourceLoc loc) {
    DiagnosticMsg* dmsg = malloc(sizeof(DiagnosticMsg));
    assert(dmsg != NULL);

    dmsg->id = id;
    dmsg->msg = msg;
    dmsg->loc = loc;

    return dmsg;
}

void diagnostic_msg_free(DiagnosticMsg* msg) {
    if (msg == NULL) {
        return;
    }

    free(msg->msg);
    free(msg);
}

DiagnosticEngine* diagnostic_engine_create() {
    DiagnosticEngine* engine = malloc(sizeof(DiagnosticEngine));
    assert(engine != NULL);

    *engine = (DiagnosticEngine) {
        .msgs = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(DiagnosticMsg*), &diagnostic_msg_free, true),
    };

    return engine;
}

void diagnostic_engine_clear(DiagnosticEngine* diag) {
    if (diag == NULL) {
        return;
    }
    vector_clear(&diag->msgs);
}

void diagnostic_engine_free(DiagnosticEngine* diag) {
    if (diag == NULL) {
        return;
    }
    vector_free(&diag->msgs);
    free(diag);
}

void diagnostic_engine_report(DiagnosticEngine* diag, const DiagnosticId id, const SourceLoc loc, ...) {
    assert(diag != NULL);

    const char* format = get_diagnostic_text(id);

    va_list args = { 0 };

    va_start(args, loc);
    i32 count = vsnprintf(NULL, 0, format, args);
    va_end(args);

    assert(count >= 0);

    char* buf = malloc(((u64)count + 1) * sizeof(char));
    assert(buf != NULL);

    va_start(args, loc);
    count = vsnprintf(buf, (u64)count + 1, format, args);
    va_end(args);

    assert(count >= 0);

    DiagnosticMsg* msg = diagnostic_msg_create(id, buf, loc);

    vector_push_back(&diag->msgs, &msg);
}

static void print_diagnostic_msg(const DiagnosticMsg* msg) {
    assert(msg != NULL);

    printf("%s(%ld,%ld): %s %s: %s.\n",
        msg->loc.filepath,
        msg->loc.start.row,
        msg->loc.start.col,
        get_diagnostic_type(msg->id),
        get_diagnostic_level(msg->id),
        msg->msg
    );
}


void diagnostic_engine_print_all(DiagnosticEngine* diag) {
    assert(diag != NULL);

    for (u64 i = 0; i < diag->msgs.items_count; ++i) {
        const DiagnosticMsg* msg = vector_get_ref(&diag->msgs, i);

        print_diagnostic_msg(msg);
    }
}
#pragma once

#include <string>
#include <vector>

namespace c2triggers {

enum class TriggerKind {
    Wait,
    Move,
    Rotate,
    Alpha,
    Toggle,
    Unknown
};

struct TriggerStep {
    TriggerKind kind = TriggerKind::Unknown;
    double time = 0.0;
    double x = 0.0;
    double y = 0.0;
    double angle = 0.0;
    double opacity = 1.0;
    int groupId = 0;
    std::string source;
    std::string error;
};

struct TranslationResult {
    std::vector<TriggerStep> steps;
    std::vector<std::string> errors;
    bool ok() const { return errors.empty(); }
};

// Translates a deliberately small, non-executing C-like command subset.
// Supported calls: wait(seconds), move(group, x, y, seconds),
// rotate(group, degrees, seconds), alpha(group, opacity, seconds),
// toggle(group, enabled). This parses commands; it never compiles or runs C.
TranslationResult translate(const std::string& source);

} // namespace c2triggers

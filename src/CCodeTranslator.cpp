#include "CCodeTranslator.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace c2triggers {
namespace {

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool number(const std::string& token, double& out) {
    const auto value = trim(token);
    if (value.empty()) return false;
    char* end = nullptr;
    out = std::strtod(value.c_str(), &end);
    return end == value.c_str() + value.size();
}

std::vector<std::string> arguments(const std::string& text) {
    std::vector<std::string> out;
    std::stringstream stream(text);
    std::string part;
    while (std::getline(stream, part, ',')) out.push_back(trim(part));
    if (text.empty()) out.clear();
    return out;
}

bool parseArgs(const std::vector<std::string>& args, size_t count,
               std::vector<double>& values) {
    if (args.size() != count) return false;
    for (const auto& arg : args) {
        double value = 0.0;
        if (!number(arg, value)) return false;
        values.push_back(value);
    }
    return true;
}

} // namespace

TranslationResult translate(const std::string& source) {
    TranslationResult result;
    std::stringstream input(source);
    std::string line;
    size_t lineNo = 0;

    while (std::getline(input, line)) {
        ++lineNo;
        line = trim(line);
        if (line.empty() || line.rfind("//", 0) == 0 ||
            line == "{" || line == "}") continue;

        // Allow simple C-style comments and optional trailing semicolons.
        const auto comment = line.find("//");
        if (comment != std::string::npos) line = trim(line.substr(0, comment));
        if (!line.empty() && line.back() == ';') line.pop_back();
        line = trim(line);
        if (line.empty()) continue;

        const auto open = line.find('(');
        const auto close = line.rfind(')');
        if (open == std::string::npos || close == std::string::npos ||
            close < open || !trim(line.substr(close + 1)).empty()) {
            result.errors.push_back("Línea " + std::to_string(lineNo) +
                ": se esperaba una llamada simple como move(1, 20, 0, 1).");
            continue;
        }

        const std::string name = trim(line.substr(0, open));
        const auto args = arguments(line.substr(open + 1, close - open - 1));
        std::vector<double> values;
        TriggerStep step;
        step.source = line;

        if (name == "wait") {
            if (!parseArgs(args, 1, values) || values[0] < 0) {
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": wait requiere un tiempo >= 0.");
                continue;
            }
            step.kind = TriggerKind::Wait;
            step.time = values[0];
        } else if (name == "move") {
            if (!parseArgs(args, 4, values) || values[0] < 1 || values[0] > 9999 || values[3] < 0) {
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": move requiere (groupId, x, y, segundos); groupId >= 1.");
                continue;
            }
            step.kind = TriggerKind::Move;
            step.groupId = static_cast<int>(values[0]);
            step.x = values[1]; step.y = values[2]; step.time = values[3];
        } else if (name == "rotate") {
            if (!parseArgs(args, 3, values) || values[0] < 1 || values[0] > 9999 || values[2] < 0) {
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": rotate requiere (groupId, grados, segundos).");
                continue;
            }
            step.kind = TriggerKind::Rotate;
            step.groupId = static_cast<int>(values[0]);
            step.angle = values[1]; step.time = values[2];
        } else if (name == "alpha") {
            if (!parseArgs(args, 3, values) || values[0] < 1 || values[0] > 9999 ||
                values[1] < 0 || values[1] > 1 || values[2] < 0) {
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": alpha requiere (groupId, opacidad 0..1, segundos).");
                continue;
            }
            step.kind = TriggerKind::Alpha;
            step.groupId = static_cast<int>(values[0]);
            step.opacity = values[1]; step.time = values[2];
        } else if (name == "toggle") {
            if (args.size() != 2 || !number(args[0], values.emplace_back())) {
                values.clear();
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": toggle requiere (groupId, 0 o 1).");
                continue;
            }
            double enabled = 0;
            if (!number(args[1], enabled) || values[0] < 1 || values[0] > 9999 ||
                (enabled != 0 && enabled != 1)) {
                result.errors.push_back("Línea " + std::to_string(lineNo) +
                    ": toggle requiere (groupId, 0 o 1).");
                continue;
            }
            step.kind = TriggerKind::Toggle;
            step.groupId = static_cast<int>(values[0]);
            step.opacity = enabled;
        } else {
            result.errors.push_back("Línea " + std::to_string(lineNo) +
                ": comando no compatible: " + name + ".");
            continue;
        }

        result.steps.push_back(step);
    }

    return result;
}

} // namespace c2triggers

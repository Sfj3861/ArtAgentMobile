#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Initial project scaffold: confirms the mod loads and exposes an entry point.
// The decoration generator and editor integration will be implemented in later steps.
class $modify(ArtAgentMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        log::info("Art Agent Mobile loaded.");
        return true;
    }
};

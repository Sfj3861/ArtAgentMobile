#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// First runnable milestone: expose an Art Agent entry point in the main menu.
// Prompt parsing, preview, and native editor-object insertion are not implemented yet.
class $modify(ArtAgentMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");
        if (menu) {
            auto label = CCLabelBMFont::create("ART AGENT", "bigFont.fnt");
            if (label) {
                auto button = CCMenuItemLabel::create(
                    label,
                    this,
                    menu_selector(ArtAgentMenuLayer::onArtAgent)
                );
                if (button) {
                    button->setID("art-agent-mobile-button"_spr);
                    menu->addChild(button);
                    menu->updateLayout();
                }
            }
        }

        log::info("Art Agent Mobile loaded.");
        return true;
    }

    void onArtAgent(CCObject*) {
        FLAlertLayer::create(
            "Art Agent Mobile",
            "Prototype loaded!\n\nNext milestone: describe a decoration, preview generated Geometry Dash objects, then insert them into the editor.\n\nExample prompt: neon blue tech blocks with glowing edges",
            "OK"
        )->show();
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

// Editor-only entry point. Decoration generation/insertion is a later milestone.
class $modify(ArtAgentEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto color = Mod::get()->getSettingValue<std::string>("logo-color");
        auto frame = (color == "pink")
            ? "GJ_button_02.png"
            : "GJ_button_04.png";

        auto icon = CCSprite::createWithSpriteFrameName(frame);
        if (icon) {
            icon->setScale(0.72f);
            auto sparkle = CCLabelBMFont::create("✦", "bigFont.fnt");
            if (sparkle) {
                sparkle->setScale(0.55f);
                sparkle->setPosition(icon->getContentSize() / 2);
                icon->addChild(sparkle);
            }

            auto button = CCMenuItemSpriteExtra::create(
                icon,
                this,
                menu_selector(ArtAgentEditorUI::onArtAgent)
            );
            if (button) {
                button->setID("art-agent-mobile-floating-button"_spr);
                auto menu = CCMenu::create();
                menu->setID("art-agent-mobile-floating-menu"_spr);
                menu->setPosition(CCPointZero);
                menu->addChild(button);
                this->addChild(menu, 9999);
                auto winSize = CCDirector::sharedDirector()->getWinSize();
                button->setPosition({winSize.width - 42.f, winSize.height * 0.62f});
            }
        }

        log::info("Art Agent Mobile editor button loaded.");
        return true;
    }

    void onArtAgent(CCObject*) {
        FLAlertLayer::create(
            "Art Agent",
            "DECORATION GENERATOR\n\n"
            "1. Select the existing objects you want to decorate in the editor.\n"
            "2. Choose the target editor layer for the generated decoration.\n"
            "3. Describe the style in English for now.\n\n"
            "Example: neon cyan tech blocks with glowing edges.\n\n"
            "This build is the editor-entry prototype: generation, layer selection, "
            "object preview, and insertion are not implemented yet.",
            "Got it"
        )->show();
    }
};

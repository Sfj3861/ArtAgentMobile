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
        auto icon = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        if (icon) {
            icon->setColor(color == "pink" ? ccc3(255, 105, 205) : ccc3(0, 220, 255));
            icon->setOpacity(190);
            icon->setScale(0.72f);
            auto sparkle = CCLabelBMFont::create("✦", "bigFont.fnt");
            if (sparkle) {
                sparkle->setScale(0.55f);
                sparkle->setPosition(CCPoint(icon->getContentSize().width / 2.f, icon->getContentSize().height / 2.f));
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

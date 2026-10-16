#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/GameEvent.hpp>

using namespace geode::prelude;

// The generator itself is still a prototype. This layer adds the configurable
// editor entry point and its editor-scoped keybind.
class $modify(ArtAgentEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        // The keybind is scoped to the editor category in mod.json.
        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "open-bind"),
            [this](Keybind const&, bool down, bool repeat, double) {
                if (down && !repeat) {
                    this->openArtAgent();
                }
            }
        );

        if (!Mod::get()->getSettingValue<bool>("show-button")) {
            return true;
        }

        auto color = Mod::get()->getSettingValue<std::string>("logo-color");
        auto opacity = static_cast<GLubyte>(
            Mod::get()->getSettingValue<double>("button-opacity")
        );
        auto scale = static_cast<float>(
            Mod::get()->getSettingValue<double>("button-size")
        );

        // Use a built-in Geometry Dash sprite, tinted with the chosen accent.
        auto icon = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        if (!icon) {
            log::warn("Art Agent could not create its floating icon.");
            return true;
        }

        icon->setColor(color == "pink" ? ccc3(255, 105, 205) : ccc3(0, 220, 255));
        icon->setOpacity(opacity);
        icon->setScale(scale);

        auto sparkle = CCLabelBMFont::create("✦", "bigFont.fnt");
        if (sparkle) {
            sparkle->setScale(0.55f);
            sparkle->setOpacity(opacity);
            sparkle->setPosition(CCPoint(
                icon->getContentSize().width / 2.f,
                icon->getContentSize().height / 2.f
            ));
            icon->addChild(sparkle);
        }

        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(ArtAgentEditorUI::onArtAgent)
        );
        if (!button) {
            return true;
        }

        button->setID("art-agent-mobile-floating-button"_spr);
        auto menu = CCMenu::create();
        if (!menu) {
            return true;
        }

        menu->setID("art-agent-mobile-floating-menu"_spr);
        menu->setPosition(CCPointZero);
        menu->addChild(button);
        this->addChild(menu, 9999);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        button->setPosition({winSize.width - 42.f, winSize.height * 0.62f});

        log::info("Art Agent editor entry point loaded.");
        return true;
    }

    void onArtAgent(CCObject*) {
        this->openArtAgent();
    }

    void openArtAgent() {
        FLAlertLayer::create(
            "Art Agent",
            "DECORATION GENERATOR\n\n"
            "Before generating, the intended workflow is to select the objects "
            "you want to decorate and choose a target editor layer.\n\n"
            "Prompt language: English for now. Spanish and French may be added later.\n\n"
            "Example: neon cyan tech blocks with glowing edges.\n\n"
            "This prototype does not yet read the selected objects, choose a layer, "
            "generate a preview, or insert decoration.",
            "OK"
        )->show();
    }
};

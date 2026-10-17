#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/GameEvent.hpp>

using namespace geode::prelude;

// Art Agent is editor-only. The generator uses ordinary GD editor objects,
// so generated blocks remain editable with the game's native tools.
class $modify(ArtAgentEditorUI, EditorUI) {
    struct Fields {
        CCNode* panel = nullptr;
        CCTextInputNode* prompt = nullptr;
        CCLabelBMFont* status = nullptr;
        bool previewReady = false;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "open-bind"),
            [this](Keybind const&, bool down, bool repeat, double) {
                if (down && !repeat) this->toggleArtAgent();
            }
        );

        if (Mod::get()->getSettingValue<bool>("show-button")) {
            auto icon = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            if (icon) {
                auto color = Mod::get()->getSettingValue<std::string>("logo-color");
                icon->setColor(color == "pink" ? ccc3(255, 105, 205) : ccc3(0, 220, 255));
                icon->setOpacity(static_cast<GLubyte>(
                    Mod::get()->getSettingValue<double>("button-opacity")));
                icon->setScale(static_cast<float>(
                    Mod::get()->getSettingValue<double>("button-size")));

                auto button = CCMenuItemSpriteExtra::create(
                    icon, this, menu_selector(ArtAgentEditorUI::onArtAgent));
                if (button) {
                    button->setID("art-agent-mobile-floating-button"_spr);
                    auto menu = CCMenu::create();
                    if (menu) {
                        menu->setID("art-agent-mobile-floating-menu"_spr);
                        menu->setPosition(CCPointZero);
                        menu->addChild(button);
                        this->addChild(menu, 9999);
                        auto size = CCDirector::sharedDirector()->getWinSize();
                        button->setPosition({size.width - 42.f, size.height * 0.62f});
                    }
                }
            }
        }
        log::info("Art Agent editor UI loaded.");
        return true;
    }

    void onArtAgent(CCObject*) { this->toggleArtAgent(); }

    void toggleArtAgent() {
        if (m_fields->panel) {
            m_fields->panel->setVisible(!m_fields->panel->isVisible());
            return;
        }

        auto size = CCDirector::sharedDirector()->getWinSize();
        auto panel = CCNode::create();
        if (!panel) return;
        panel->setID("art-agent-mobile-panel"_spr);
        panel->setContentSize({size.width * 0.76f, size.height * 0.50f});
        panel->setPosition({size.width * 0.12f, size.height * 0.25f});
        panel->setZOrder(10000);

        auto bg = CCLayerColor::create({22, 25, 43, 238},
            panel->getContentSize().width, panel->getContentSize().height);
        if (bg) panel->addChild(bg, -1);

        auto title = CCLabelBMFont::create("ART AGENT", "bigFont.fnt");
        if (title) {
            title->setScale(0.7f);
            title->setColor({0, 225, 255});
            title->setAnchorPoint({0.f, 0.5f});
            title->setPosition({16.f, panel->getContentSize().height - 24.f});
            panel->addChild(title);
        }

        auto hint = CCLabelBMFont::create(
            "Describe a style, then preview before inserting native objects.",
            "chatFont.fnt");
        if (hint) {
            hint->setScale(0.48f);
            hint->setAnchorPoint({0.f, 0.5f});
            hint->setPosition({16.f, panel->getContentSize().height - 52.f});
            panel->addChild(hint);
        }

        auto input = CCTextInputNode::create(
            panel->getContentSize().width - 32.f, 30.f,
            "neon blue tech blocks with glowing edges", "bigFont.fnt");
        if (input) {
            input->setPosition({panel->getContentSize().width / 2.f,
                panel->getContentSize().height - 88.f});
            panel->addChild(input);
            m_fields->prompt = input;
        }

        auto status = CCLabelBMFont::create(
            "Ready. Preview creates a plan; Insert adds editable GD blocks.",
            "chatFont.fnt");
        if (status) {
            status->setScale(0.48f);
            status->setAnchorPoint({0.f, 0.5f});
            status->setPosition({16.f, 50.f});
            panel->addChild(status);
            m_fields->status = status;
        }

        auto menu = CCMenu::create();
        if (menu) {
            menu->setPosition(CCPointZero);
            auto previewLabel = CCLabelBMFont::create("PREVIEW", "bigFont.fnt");
            auto insertLabel = CCLabelBMFont::create("INSERT", "bigFont.fnt");
            auto closeLabel = CCLabelBMFont::create("CLOSE", "bigFont.fnt");
            if (previewLabel) {
                previewLabel->setScale(0.55f);
                auto item = CCMenuItemLabel::create(previewLabel, this,
                    menu_selector(ArtAgentEditorUI::onPreview));
                item->setPosition({panel->getContentSize().width * 0.24f, 22.f});
                menu->addChild(item);
            }
            if (insertLabel) {
                insertLabel->setScale(0.55f);
                auto item = CCMenuItemLabel::create(insertLabel, this,
                    menu_selector(ArtAgentEditorUI::onInsert));
                item->setPosition({panel->getContentSize().width * 0.53f, 22.f});
                menu->addChild(item);
            }
            if (closeLabel) {
                closeLabel->setScale(0.55f);
                auto item = CCMenuItemLabel::create(closeLabel, this,
                    menu_selector(ArtAgentEditorUI::onClosePanel));
                item->setPosition({panel->getContentSize().width * 0.82f, 22.f});
                menu->addChild(item);
            }
            panel->addChild(menu);
        }

        this->addChild(panel);
        m_fields->panel = panel;
    }

    void onPreview(CCObject*) {
        auto prompt = m_fields->prompt ? std::string(m_fields->prompt->getString()) : "";
        if (prompt.empty()) {
            this->setStatus("Enter a description first.");
            return;
        }
        m_fields->previewReady = true;
        this->setStatus("Preview plan: a small 5x3 native-block motif. Press INSERT to add it.");
    }

    void onInsert(CCObject*) {
        if (!m_fields->previewReady) {
            this->setStatus("Press PREVIEW first, then INSERT.");
            return;
        }
        auto prompt = m_fields->prompt ? std::string(m_fields->prompt->getString()) : "";
        if (prompt.empty()) {
            this->setStatus("Enter a description first.");
            return;
        }

        // Use the editor's native object-creation path. Keep the first built-in
        // implementation conservative: a compact, editable block motif.
        int inserted = 0;
        constexpr float spacing = 30.f;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 5; ++col) {
                // Leave small gaps to create a decorative pattern rather than
                // filling a solid rectangle.
                if ((row == 1 && col == 2) || (row == 0 && (col == 1 || col == 3)))
                    continue;
                this->createObject(1, CCPoint(120.f + col * spacing, 120.f + row * spacing));
                ++inserted;
            }
        }
        m_fields->previewReady = false;
        this->setStatus(("Inserted " + std::to_string(inserted) +
            " editable native blocks. Prompt: " + prompt.substr(0, 44)).c_str());
    }

    void onClosePanel(CCObject*) {
        if (m_fields->panel) m_fields->panel->setVisible(false);
    }

    void setStatus(char const* text) {
        if (m_fields->status) m_fields->status->setString(text);
    }
};

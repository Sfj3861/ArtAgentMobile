#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace geode::prelude;

// Art Agent is editor-only. The generator uses ordinary GD editor objects,
// so generated blocks remain editable with the game's native tools.
class $modify(ArtAgentEditorUI, EditorUI) {
    struct Fields {
        CCNode* panel = nullptr;
        CCTextInputNode* prompt = nullptr;
        CCLabelBMFont* status = nullptr;
        bool previewReady = false;
        int pattern = 0;
        int density = 1;
        std::string previewDescription;
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
        std::transform(prompt.begin(), prompt.end(), prompt.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        // A small offline prompt parser maps common style words to deterministic
        // editable patterns. It does not pretend to be a remote AI model.
        m_fields->pattern = 0; // tech/grid
        if (prompt.find("diamond") != std::string::npos ||
            prompt.find("diamante") != std::string::npos) m_fields->pattern = 1;
        else if (prompt.find("circle") != std::string::npos ||
                 prompt.find("círculo") != std::string::npos ||
                 prompt.find("circulo") != std::string::npos ||
                 prompt.find("ring") != std::string::npos) m_fields->pattern = 2;
        else if (prompt.find("wave") != std::string::npos ||
                 prompt.find("onda") != std::string::npos ||
                 prompt.find("zigzag") != std::string::npos) m_fields->pattern = 3;
        else if (prompt.find("star") != std::string::npos ||
                 prompt.find("estrella") != std::string::npos) m_fields->pattern = 4;
        else if (prompt.find("line") != std::string::npos ||
                 prompt.find("línea") != std::string::npos ||
                 prompt.find("linea") != std::string::npos) m_fields->pattern = 5;

        m_fields->density = 1;
        if (prompt.find("dense") != std::string::npos ||
            prompt.find("many") != std::string::npos ||
            prompt.find("denso") != std::string::npos ||
            prompt.find("muchos") != std::string::npos) m_fields->density = 2;
        if (prompt.find("sparse") != std::string::npos ||
            prompt.find("few") != std::string::npos ||
            prompt.find("pocos") != std::string::npos ||
            prompt.find("simple") != std::string::npos) m_fields->density = 0;

        static char const* names[] = {"tech grid", "diamond", "ring", "wave", "star", "line"};
        m_fields->previewDescription = prompt;
        m_fields->previewReady = true;
        this->setStatus((std::string("Plan: ") + names[m_fields->pattern] +
            (m_fields->density == 2 ? " / dense" : m_fields->density == 0 ? " / sparse" : " / normal") +
            ". INSERT adds native blocks.").c_str());
    }

    void onInsert(CCObject*) {
        if (!m_fields->previewReady) {
            this->setStatus("Press PREVIEW first, then INSERT.");
            return;
        }

        int inserted = 0;
        constexpr float spacing = 30.f;
        auto add = [&](float x, float y) {
            this->createObject(1, CCPoint(x, y));
            ++inserted;
        };
        int step = m_fields->density == 2 ? 1 : m_fields->density == 0 ? 3 : 2;

        // The design is generated locally, without an API key or network.
        // All pieces are normal GD block objects, so they remain editable.
        switch (m_fields->pattern) {
            case 1: { // diamond outline
                for (int d = 0; d <= 4; d += step) {
                    add(180.f + d * spacing, 180.f + (4 - d) * spacing);
                    add(180.f + d * spacing, 180.f - (4 - d) * spacing);
                    if (d != 0 && d != 4) {
                        add(180.f - d * spacing, 180.f + (4 - d) * spacing);
                        add(180.f - d * spacing, 180.f - (4 - d) * spacing);
                    }
                }
                break;
            }
            case 2: { // square-ish ring
                for (int i = -4; i <= 4; i += step) {
                    add(180.f + i * spacing, 180.f + 4 * spacing);
                    add(180.f + i * spacing, 180.f - 4 * spacing);
                    add(180.f + 4 * spacing, 180.f + i * spacing);
                    add(180.f - 4 * spacing, 180.f + i * spacing);
                }
                break;
            }
            case 3: { // zigzag / wave
                for (int i = 0; i < 13; i += step) {
                    float y = 180.f + ((i % 4 < 2) ? 1.f : -1.f) * (i % 2 ? 30.f : 60.f);
                    add(100.f + i * spacing, y);
                    if (m_fields->density == 2) add(100.f + i * spacing, y + 30.f);
                }
                break;
            }
            case 4: { // pixel star
                int const points[][2] = {{0,3},{0,2},{-1,1},{-2,1},{-1,0},{-1,-1},{0,-2},{1,-1},{1,0},{2,1},{1,1},{0,1}};
                for (auto const& p : points) {
                    add(210.f + p[0] * spacing, 180.f + p[1] * spacing);
                    if (m_fields->density == 2)
                        add(210.f + p[0] * spacing + 12.f, 180.f + p[1] * spacing + 12.f);
                }
                break;
            }
            case 5: { // horizontal rails
                int count = m_fields->density == 0 ? 5 : m_fields->density == 2 ? 15 : 9;
                for (int i = 0; i < count; ++i) {
                    add(100.f + i * spacing, 150.f);
                    add(100.f + i * spacing, 210.f);
                }
                break;
            }
            default: { // layered tech grid with an open center
                int radius = m_fields->density == 0 ? 2 : m_fields->density == 2 ? 5 : 3;
                for (int row = -radius; row <= radius; ++row) {
                    for (int col = -radius; col <= radius; ++col) {
                        if (row == 0 && col == 0) continue;
                        if ((std::abs(row) + std::abs(col)) % step != 0 &&
                            m_fields->density != 2) continue;
                        add(180.f + col * spacing, 180.f + row * spacing);
                    }
                }
                break;
            }
        }
        m_fields->previewReady = false;
        this->setStatus(("Inserted " + std::to_string(inserted) +
            " editable blocks. Save/check placement; this offline generator does not preserve selection groups automatically.").c_str());
    }

    void onClosePanel(CCObject*) {
        if (m_fields->panel) m_fields->panel->setVisible(false);
    }

    void setStatus(char const* text) {
        if (m_fields->status) m_fields->status->setString(text);
    }
};

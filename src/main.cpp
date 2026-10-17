#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <set>
#include <vector>

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
        int detail = 2;
        bool hasLayout = false;
        CCPoint layoutCenter = CCPointZero;
        float layoutMinX = 0.f, layoutMaxX = 0.f, layoutMinY = 0.f, layoutMaxY = 0.f;
        std::string previewDescription;
        std::vector<CCPoint> layoutPoints;
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
            "Select layout objects first; decoration will follow their bounds.",
            "chatFont.fnt");
        if (hint) {
            hint->setScale(0.48f);
            hint->setAnchorPoint({0.f, 0.5f});
            hint->setPosition({16.f, panel->getContentSize().height - 52.f});
            panel->addChild(hint);
        }

        auto input = CCTextInputNode::create(
            panel->getContentSize().width - 32.f, 30.f,
            "realistic stone facade with layered trim and window recesses", "bigFont.fnt");
        if (input) {
            input->setPosition({panel->getContentSize().width / 2.f,
                panel->getContentSize().height - 88.f});
            panel->addChild(input);
            m_fields->prompt = input;
        }

        auto status = CCLabelBMFont::create(
            "Select layout objects, choose detail, then preview.",
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
            auto lowLabel = CCLabelBMFont::create("LOW", "bigFont.fnt");
            auto medLabel = CCLabelBMFont::create("MED", "bigFont.fnt");
            auto highLabel = CCLabelBMFont::create("HIGH", "bigFont.fnt");
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
            auto addDetail = [&](CCLabelBMFont* label, float x, SEL_MenuHandler handler) {
                if (!label) return;
                label->setScale(0.43f);
                auto item = CCMenuItemLabel::create(label, this, handler);
                item->setPosition({x, 49.f});
                menu->addChild(item);
            };
            addDetail(lowLabel, panel->getContentSize().width * 0.27f, menu_selector(ArtAgentEditorUI::onDetailLow));
            addDetail(medLabel, panel->getContentSize().width * 0.40f, menu_selector(ArtAgentEditorUI::onDetailMed));
            addDetail(highLabel, panel->getContentSize().width * 0.53f, menu_selector(ArtAgentEditorUI::onDetailHigh));
            panel->addChild(menu);
        }

        this->addChild(panel);
        m_fields->panel = panel;
    }

    void onDetailLow(CCObject*) { m_fields->detail = 1; this->setStatus("Detail LOW selected."); }
    void onDetailMed(CCObject*) { m_fields->detail = 2; this->setStatus("Detail MEDIUM selected."); }
    void onDetailHigh(CCObject*) { m_fields->detail = 3; this->setStatus("Detail HIGH selected."); }

    void captureSelectedLayout() {
        auto arr = CCArray::create();
        if (m_selectedObjects) arr->addObjectsFromArray(m_selectedObjects);
        if (m_selectedObject && !arr->containsObject(m_selectedObject)) arr->addObject(m_selectedObject);
        m_fields->layoutPoints.clear();
        m_fields->hasLayout = arr && arr->count() > 0;
        if (!m_fields->hasLayout) return;
        bool first = true;
        for (auto obj : CCArrayExt<GameObject*>(arr)) {
            if (!obj) continue;
            auto p = obj->getPosition();
            m_fields->layoutPoints.push_back(p);
            if (first) {
                m_fields->layoutMinX = m_fields->layoutMaxX = p.x;
                m_fields->layoutMinY = m_fields->layoutMaxY = p.y;
                first = false;
            } else {
                m_fields->layoutMinX = std::min(m_fields->layoutMinX, p.x);
                m_fields->layoutMaxX = std::max(m_fields->layoutMaxX, p.x);
                m_fields->layoutMinY = std::min(m_fields->layoutMinY, p.y);
                m_fields->layoutMaxY = std::max(m_fields->layoutMaxY, p.y);
            }
        }
        m_fields->hasLayout = !first && !m_fields->layoutPoints.empty();
        if (m_fields->hasLayout)
            m_fields->layoutCenter = CCPointMake(
                (m_fields->layoutMinX + m_fields->layoutMaxX) / 2.f,
                (m_fields->layoutMinY + m_fields->layoutMaxY) / 2.f
            );
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
        this->captureSelectedLayout();
        if (!m_fields->hasLayout) {
            this->setStatus("Select the layout objects in the editor first.");
            return;
        }
        m_fields->pattern = 0; // realistic architecture default
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
        else if (prompt.find("circuit") != std::string::npos ||
                 prompt.find("circuito") != std::string::npos ||
                 prompt.find("tech") != std::string::npos ||
                 prompt.find("tecnológ") != std::string::npos ||
                 prompt.find("tecnolog") != std::string::npos) m_fields->pattern = 6;
        else if (prompt.find("gothic") != std::string::npos ||
                 prompt.find("gótico") != std::string::npos ||
                 prompt.find("gotico") != std::string::npos ||
                 prompt.find("medieval") != std::string::npos) m_fields->pattern = 7;
        else if (prompt.find("crystal") != std::string::npos ||
                 prompt.find("cristal") != std::string::npos ||
                 prompt.find("ice") != std::string::npos ||
                 prompt.find("hielo") != std::string::npos) m_fields->pattern = 8;
        else if (prompt.find("organic") != std::string::npos ||
                 prompt.find("orgánico") != std::string::npos ||
                 prompt.find("organico") != std::string::npos ||
                 prompt.find("nature") != std::string::npos ||
                 prompt.find("naturaleza") != std::string::npos ||
                 prompt.find("jungle") != std::string::npos ||
                 prompt.find("selva") != std::string::npos) m_fields->pattern = 9;
        else if (prompt.find("steampunk") != std::string::npos ||
                 prompt.find("clockwork") != std::string::npos ||
                 prompt.find("engranaje") != std::string::npos ||
                 prompt.find("reloj") != std::string::npos) m_fields->pattern = 10;
        else if (prompt.find("sci-fi") != std::string::npos ||
                 prompt.find("scifi") != std::string::npos ||
                 prompt.find("futuristic") != std::string::npos ||
                 prompt.find("futurista") != std::string::npos ||
                 prompt.find("spaceship") != std::string::npos ||
                 prompt.find("nave") != std::string::npos) m_fields->pattern = 11;
        else if (prompt.find("ruin") != std::string::npos ||
                 prompt.find("ruina") != std::string::npos ||
                 prompt.find("abandoned") != std::string::npos ||
                 prompt.find("abandonado") != std::string::npos) m_fields->pattern = 12;
        else if (prompt.find("neon") != std::string::npos ||
                 prompt.find("neón") != std::string::npos ||
                 prompt.find("cyberpunk") != std::string::npos) m_fields->pattern = 13;
        else if (prompt.find("underwater") != std::string::npos ||
                 prompt.find("submarino") != std::string::npos ||
                 prompt.find("ocean") != std::string::npos ||
                 prompt.find("océano") != std::string::npos ||
                 prompt.find("oceano") != std::string::npos) m_fields->pattern = 14;
        else if (prompt.find("infernal") != std::string::npos ||
                 prompt.find("hell") != std::string::npos ||
                 prompt.find("demon") != std::string::npos ||
                 prompt.find("infierno") != std::string::npos) m_fields->pattern = 15;
        else if (prompt.find("celestial") != std::string::npos ||
                 prompt.find("heaven") != std::string::npos ||
                 prompt.find("angel") != std::string::npos ||
                 prompt.find("divine") != std::string::npos ||
                 prompt.find("celeste") != std::string::npos) m_fields->pattern = 16;
        else if (prompt.find("industrial") != std::string::npos ||
                 prompt.find("factory") != std::string::npos ||
                 prompt.find("fábrica") != std::string::npos ||
                 prompt.find("fabrica") != std::string::npos ||
                 prompt.find("warehouse") != std::string::npos) m_fields->pattern = 17;

        m_fields->density = 1;
        if (prompt.find("dense") != std::string::npos ||
            prompt.find("many") != std::string::npos ||
            prompt.find("denso") != std::string::npos ||
            prompt.find("muchos") != std::string::npos) m_fields->density = 2;
        if (prompt.find("sparse") != std::string::npos ||
            prompt.find("few") != std::string::npos ||
            prompt.find("pocos") != std::string::npos ||
            prompt.find("simple") != std::string::npos) m_fields->density = 0;

        static char const* names[] = {
            "realistic architecture", "diamond trim", "ring frame", "wave trim",
            "star ornament", "linear trim", "tech circuits", "gothic arches",
            "crystal facets", "organic curves", "steampunk gears", "sci-fi panels",
            "ancient ruins", "neon cyberpunk", "underwater reef", "infernal spikes",
            "celestial halos", "industrial machinery"
        };
        m_fields->previewDescription = prompt;
        m_fields->previewReady = true;
        this->setStatus((std::string("Plan: ") + names[m_fields->pattern] +
            (m_fields->density == 2 ? " / dense" : m_fields->density == 0 ? " / sparse" : " / normal") +
            ". INSERT decorates around selected layout.").c_str());
    }

    void onInsert(CCObject*) {
        if (!m_fields->previewReady) {
            this->setStatus("Press PREVIEW first.");
            return;
        }
        this->captureSelectedLayout();
        if (!m_fields->hasLayout) {
            this->setStatus("Select layout objects first. Nothing inserted.");
            return;
        }
        int inserted = 0;
        constexpr int MAX_GENERATED_OBJECTS = 1200;
        std::set<std::pair<int, int>> occupied;
        bool reachedLimit = false;
        float x0 = m_fields->layoutMinX - 30.f;
        float x1 = m_fields->layoutMaxX + 30.f;
        float y0 = m_fields->layoutMinY - 30.f;
        float y1 = m_fields->layoutMaxY + 30.f;
        float w = std::max(90.f, x1-x0), h = std::max(60.f, y1-y0);
        int stride = m_fields->detail == 1 ? 3 : m_fields->detail == 2 ? 2 : 1;
        if (m_fields->density == 2) stride = std::max(1, stride - 1);
        else if (m_fields->density == 0) stride = std::min(4, stride + 1);
        auto add = [&](float x, float y) {
            if (inserted >= MAX_GENERATED_OBJECTS) {
                reachedLimit = true;
                return;
            }
            // Prevent overlapping duplicates caused by intersections between
            // borders, bays and trim. Quantization keeps the check deterministic.
            auto key = std::make_pair(
                static_cast<int>(std::lround(x * 10.f)),
                static_cast<int>(std::lround(y * 10.f))
            );
            if (!occupied.insert(key).second) return;
            this->createObject(1, CCPoint(x, y));
            ++inserted;
        };

        // v0.7 contour mode: infer occupied grid cells from the selected
        // object positions, then decorate only exposed cell edges. This is a
        // position-based approximation (not a full sprite-shape collision test).
        if (m_fields->pattern == 0) {
            constexpr float CELL = 30.f;
            using Cell = std::pair<int, int>;
            std::set<Cell> cells;
            for (auto const& p : m_fields->layoutPoints) {
                cells.insert({static_cast<int>(std::lround(p.x / CELL)),
                              static_cast<int>(std::lround(p.y / CELL))});
            }
            const Cell neighbors[] = {{1,0},{-1,0},{0,1},{0,-1}};
            for (auto const& cell : cells) {
                for (auto const& n : neighbors) {
                    Cell adjacent = {cell.first + n.first, cell.second + n.second};
                    if (cells.find(adjacent) != cells.end()) continue;
                    float cx = static_cast<float>(cell.first) * CELL;
                    float cy = static_cast<float>(cell.second) * CELL;
                    // Place a short row just outside each exposed side.
                    if (n.first != 0) {
                        float edgeX = cx + n.first * 18.f;
                        add(edgeX, cy - 12.f);
                        add(edgeX, cy);
                        add(edgeX, cy + 12.f);
                    } else {
                        float edgeY = cy + n.second * 18.f;
                        add(cx - 12.f, edgeY);
                        add(cx, edgeY);
                        add(cx + 12.f, edgeY);
                    }
                }
            }
            // Add a restrained second trim pass at medium/high detail.
            if (m_fields->detail >= 2) {
                for (auto const& cell : cells) {
                    float cx = static_cast<float>(cell.first) * CELL;
                    float cy = static_cast<float>(cell.second) * CELL;
                    if (m_fields->detail >= 3) {
                        add(cx - 12.f, cy - 12.f);
                        add(cx + 12.f, cy + 12.f);
                    }
                }
            }
        } else if (m_fields->pattern == 1) {
            int n = std::max(2, static_cast<int>(w/60.f));
            for (int i=0; i<=n; i+=stride) {
                float t = static_cast<float>(i)/n;
                float off = (0.5f-std::abs(t-0.5f))*h*0.7f;
                add(x0+t*w, (y0+y1)/2.f+off);
                add(x0+t*w, (y0+y1)/2.f-off);
            }
        } else if (m_fields->pattern == 2 || m_fields->pattern == 5) {
            for(float x=x0; x<=x1; x+=30.f*stride) { add(x,y0); add(x,y1); }
            for(float y=y0; y<=y1; y+=30.f*stride) { add(x0,y); add(x1,y); }
        } else if (m_fields->pattern == 3) {
            for(float x=x0; x<=x1; x+=30.f*stride) {
                float t=(x-x0)/w;
                add(x,y0+h*(0.35f+0.12f*std::sin(t*12.f)));
                if(m_fields->detail>=2) add(x,y0+h*(0.65f+0.1f*std::sin(t*12.f+1.5f)));
            }
        } else if (m_fields->pattern == 4) {
            const int pts[][2]={{0,3},{1,1},{3,0},{1,-1},{0,-3},{-1,-1},{-3,0},{-1,1}};
            for(auto const& p:pts) add(m_fields->layoutCenter.x+p[0]*30.f,m_fields->layoutCenter.y+p[1]*30.f);
        } else if (m_fields->pattern == 6) {
            // Circuit-board style: a double frame, repeating vertical traces,
            // and short branches that resemble editable PCB paths.
            for (float x=x0; x<=x1; x+=30.f*stride) {
                add(x,y0); add(x,y1);
                float mid=(y0+y1)*0.5f;
                add(x,mid);
                if (m_fields->detail>=2) {
                    add(x+12.f,mid+12.f); add(x-12.f,mid-12.f);
                    add(x,mid+24.f); add(x,mid-24.f);
                }
            }
            for (float y=y0; y<=y1; y+=30.f*stride) {
                add(x0,y); add(x1,y);
            }
            for (float x=x0+60.f; x<x1-30.f; x+=90.f*stride) {
                add(x,y0+h*0.25f); add(x+30.f,y0+h*0.25f);
                add(x,y0+h*0.75f); add(x+30.f,y0+h*0.75f);
            }
        } else if (m_fields->pattern == 7) {
            // Gothic/medieval facade: tall bays with stepped pointed arches.
            for (float x=x0; x<=x1; x+=30.f*stride) { add(x,y0); add(x,y1); }
            float bay=std::max(60.f,w/(m_fields->detail==1?3.f:m_fields->detail==2?5.f:7.f));
            for (float x=x0+bay*0.5f; x<x1-bay*0.25f; x+=bay) {
                float bot=y0+h*0.22f, peak=y0+h*0.72f;
                add(x,bot);
                for (int j=0;j<4;++j) {
                    float side=(j<2)?-1.f:1.f;
                    float rise=(j%2==0)?0.f:18.f;
                    add(x+side*(12.f+j*6.f), peak-std::abs(side*(12.f+j*6.f))*0.8f+rise);
                }
                add(x-bay*0.22f,bot); add(x+bay*0.22f,bot);
                if (m_fields->detail>=2) {
                    add(x-bay*0.22f,bot+30.f); add(x+bay*0.22f,bot+30.f);
                }
            }
            for (int k=0;k<4;++k) {
                float x=(k<2)?x0:x1, y=(k%2==0)?y0:y1;
                for(int j=0;j<5;++j) add(x+((k<2)?1.f:-1.f)*j*12.f,y+((k%2==0)?1.f:-1.f)*j*12.f);
            }
        } else if (m_fields->pattern == 8) {
            // Crystal/ice style: alternating facet diagonals and diamond bands.
            int n=std::max(2,static_cast<int>(w/(30.f*stride)));
            for(int i=0;i<=n;++i) {
                float x=x0+w*static_cast<float>(i)/n;
                add(x,y0); add(x,y1);
                if(i%2==0) { add(x+15.f,y0+h*0.25f); add(x-15.f,y0+h*0.75f); }
                else { add(x-15.f,y0+h*0.25f); add(x+15.f,y0+h*0.75f); }
            }
            for(float y=y0;y<=y1;y+=30.f*stride) { add(x0,y); add(x1,y); }
            for(float x=x0+30.f;x<x1;x+=60.f*stride) {
                add(x,y0+h*0.5f); add(x+15.f,y0+h*0.5f+18.f);
                add(x+30.f,y0+h*0.5f); add(x+15.f,y0+h*0.5f-18.f);
            }
        } else if (m_fields->pattern == 9) {
            // Organic pattern: staggered waves and branch-like curves.
            for(float x=x0;x<=x1;x+=24.f*stride) {
                float t=(x-x0)/w;
                add(x,y0+h*(0.22f+0.06f*std::sin(t*8.f)));
                add(x,y0+h*(0.78f+0.06f*std::sin(t*8.f+1.8f)));
                if(m_fields->detail>=2) add(x,y0+h*(0.5f+0.12f*std::sin(t*10.f)));
            }
            for(float y=y0+30.f;y<y1-20.f;y+=45.f*stride)
                for(float x=x0+30.f;x<x1-20.f;x+=60.f*stride) {
                    add(x,y); add(x+12.f,y+12.f); add(x+24.f,y+18.f);
                }
        } else if (m_fields->pattern == 10) {
            // Steampunk: gear-like rings, spokes and pipe runs.
            for(float cx=x0+45.f;cx<x1;cx+=90.f*stride) {
                for(int j=0;j<12;j+=stride) {
                    float a=static_cast<float>(j)*3.14159265f/6.f;
                    add(cx+24.f*std::cos(a),m_fields->layoutCenter.y+24.f*std::sin(a));
                    if(m_fields->detail>=2) add(cx+36.f*std::cos(a),m_fields->layoutCenter.y+36.f*std::sin(a));
                }
                add(cx,m_fields->layoutCenter.y); add(cx-12.f,m_fields->layoutCenter.y);
                add(cx+12.f,m_fields->layoutCenter.y);
            }
            for(float y=y0;y<=y1;y+=30.f*stride) { add(x0,y); add(x1,y); }
            for(float x=x0;x<=x1;x+=60.f*stride) { add(x,y0+18.f); add(x,y1-18.f); }
        } else if (m_fields->pattern == 11) {
            // Sci-fi: layered hull panels, vents and repeated control nodes.
            for(float x=x0;x<=x1;x+=30.f*stride) {
                add(x,y0); add(x,y1); add(x,y0+18.f); add(x,y1-18.f);
                if(m_fields->detail>=2) {
                    add(x,y0+h*0.33f); add(x,y0+h*0.66f);
                }
            }
            for(float y=y0+30.f*stride;y<y1;y+=45.f*stride) {
                add(x0,y); add(x1,y); add(x0+24.f,y); add(x1-24.f,y);
            }
            for(float x=x0+45.f;x<x1-20.f;x+=90.f*stride) {
                add(x,y0+h*0.5f); add(x+12.f,y0+h*0.5f+12.f);
                add(x+24.f,y0+h*0.5f);
            }
        } else if (m_fields->pattern == 12) {
            // Ruins: broken columns, irregular lintels and scattered rubble.
            for(float x=x0;x<=x1;x+=60.f*stride) {
                add(x,y0); add(x,y0+18.f); add(x+12.f,y0+30.f);
                if(m_fields->detail>=2) {
                    add(x,y0+h*0.35f); add(x+12.f,y0+h*0.38f);
                    add(x,y0+h*0.55f); add(x-12.f,y0+h*0.58f);
                }
            }
            for(float x=x0;x<x1;x+=45.f*stride) {
                add(x,y1); add(x+18.f,y1-12.f);
                if(m_fields->detail>=2) add(x+30.f,y1-24.f);
            }
            for(float x=x0+15.f;x<x1;x+=36.f*stride)
                for(float y=y0+15.f;y<y0+h*0.2f;y+=30.f*stride) add(x,y);
        } else if (m_fields->pattern == 13) {
            // Neon cyberpunk: layered rectangular frames and stepped signage.
            for(float x=x0;x<=x1;x+=24.f*stride) {
                add(x,y0); add(x,y1);
                if(m_fields->detail>=2) { add(x,y0+12.f); add(x,y1-12.f); }
            }
            for(float y=y0;y<=y1;y+=24.f*stride) { add(x0,y); add(x1,y); }
            for(float x=x0+30.f;x<x1-30.f;x+=75.f*stride) {
                float cy=y0+h*0.5f;
                add(x,cy); add(x+24.f,cy); add(x+24.f,cy+24.f);
                add(x+48.f,cy+24.f); add(x+48.f,cy);
                if(m_fields->detail>=3) { add(x+12.f,cy-12.f); add(x+36.f,cy+36.f); }
            }
        } else if (m_fields->pattern == 14) {
            // Underwater: flowing currents, coral branches and bubble columns.
            for(float x=x0;x<=x1;x+=30.f*stride) {
                float t=(x-x0)/w;
                add(x,y0+h*(0.28f+0.08f*std::sin(t*7.f)));
                add(x,y0+h*(0.72f+0.08f*std::sin(t*7.f+2.f)));
                if(m_fields->detail>=2) {
                    add(x,y0+h*0.5f); add(x+12.f,y0+h*0.5f+12.f);
                }
            }
            for(float x=x0+24.f;x<x1;x+=60.f*stride) {
                for(int j=0;j<4;++j) add(x+((j%2)?12.f:-12.f)*j,y0+20.f+j*18.f);
            }
        } else if (m_fields->pattern == 15) {
            // Infernal: jagged spikes, broken bands and angular flame silhouettes.
            for(float x=x0;x<=x1;x+=30.f*stride) {
                add(x,y0); add(x,y1);
                add(x,y0+((static_cast<int>((x-x0)/30.f)%2)?30.f:0.f));
                add(x,y1-((static_cast<int>((x-x0)/30.f)%2)?0.f:30.f));
            }
            for(float x=x0+15.f;x<x1;x+=45.f*stride) {
                add(x,y0+h*0.4f); add(x+15.f,y0+h*0.5f);
                add(x+30.f,y0+h*0.4f);
                if(m_fields->detail>=2) { add(x+15.f,y0+h*0.4f+24.f); add(x+15.f,y0+h*0.4f-24.f); }
            }
        } else if (m_fields->pattern == 16) {
            // Celestial: orbit-like rings, star nodes and radial connectors.
            for(float cx=x0+45.f;cx<x1;cx+=90.f*stride) {
                float cy=m_fields->layoutCenter.y;
                for(int j=0;j<12;j+=stride) {
                    float a=static_cast<float>(j)*3.14159265f/6.f;
                    add(cx+30.f*std::cos(a),cy+30.f*std::sin(a));
                }
                add(cx,cy); add(cx-12.f,cy+12.f); add(cx+12.f,cy-12.f);
            }
            for(float x=x0;x<=x1;x+=30.f*stride) { add(x,y0); add(x,y1); }
            for(float y=y0;y<=y1;y+=30.f*stride) { add(x0,y); add(x1,y); }
        } else if (m_fields->pattern == 17) {
            // Industrial: beams, rivet rows and repeated vertical supports.
            for(float x=x0;x<=x1;x+=30.f*stride) {
                add(x,y0); add(x,y1);
                if(m_fields->detail>=2) { add(x+12.f,y0); add(x+12.f,y1); }
            }
            for(float y=y0;y<=y1;y+=30.f*stride) {
                add(x0,y); add(x1,y);
                if(m_fields->detail>=3) { add(x0+12.f,y); add(x1-12.f,y); }
            }
            for(float x=x0+60.f;x<x1;x+=90.f*stride)
                for(float y=y0+30.f;y<y1;y+=45.f*stride) {
                    add(x,y); add(x+12.f,y+12.f);
                }
        } else {
            for(float x=x0; x<=x1; x+=30.f*stride) {
                add(x,y0); add(x,y1);
                if(m_fields->detail>=2) { add(x,y0+12.f); add(x,y1-12.f); }
            }
            for(float y=y0; y<=y1; y+=30.f*stride) {
                add(x0,y); add(x1,y);
                if(m_fields->detail>=3) { add(x0+12.f,y); add(x1-12.f,y); }
            }
            float pierStep=std::max(90.f,w/(m_fields->detail==1?3.f:m_fields->detail==2?5.f:7.f));
            for(float x=x0+pierStep; x<x1; x+=pierStep)
                for(float y=y0+30.f; y<y1-15.f; y+=30.f*stride) {
                    add(x,y);
                    if(m_fields->detail>=2) add(x+12.f,y);
                }
            float bayW=std::max(60.f,w/(m_fields->detail==1?4.f:m_fields->detail==2?6.f:8.f));
            for(float x=x0+20.f; x+bayW<x1; x+=bayW) {
                float bot=y0+h*0.34f, top=y0+h*0.72f;
                for(float xx=x; xx<=x+bayW*0.55f; xx+=30.f*stride) { add(xx,bot); add(xx,top); }
                add(x,(top+bot)/2.f); add(x+bayW*0.55f,(top+bot)/2.f);
                if(m_fields->detail>=3) { add(x,bot-12.f); add(x+bayW*0.55f,bot-12.f); add(x,top+12.f); add(x+bayW*0.55f,top+12.f); }
            }
            for(int k=0;k<4;++k) {
                float x=(k<2)?x0:x1, y=(k%2==0)?y0:y1;
                for(int j=0;j<(m_fields->detail==1?2:m_fields->detail==2?3:5);++j)
                    add(x+((k<2)?1.f:-1.f)*j*12.f,y+((k%2==0)?1.f:-1.f)*j*12.f);
            }
            if(m_fields->detail>=3)
                for(float x=x0;x<=x1;x+=15.f) { add(x,y1+12.f); add(x,y0-12.f); }
        }
        m_fields->previewReady=false;
        std::string result = "Added " + std::to_string(inserted) +
            " unique editable decoration blocks around the selected layout.";
        if (reachedLimit) result += " Safety cap reached (1200 objects); reduce detail or split the selection.";
        result += " Original layout objects were not intentionally modified; inspect before saving.";
        this->setStatus(result.c_str());
    }

    void onClosePanel(CCObject*) {
        if (m_fields->panel) m_fields->panel->setVisible(false);
    }

    void setStatus(char const* text) {
        if (m_fields->status) m_fields->status->setString(text);
    }
};

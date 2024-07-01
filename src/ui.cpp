#include "ui.h"

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setFramerateLimit(60);
    ImGui::SFML::Init(*window);

    this->is_open_ = window->isOpen();
}

UI::~UI() {
    ImGui::SFML::Shutdown();
    delete window;
}

void UI::render() {
    sf::Event event;
    sf::Clock delta_clock;

    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);

        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }

    ImGui::SFML::Update(*window, delta_clock.restart());

    this->is_open_ = window->isOpen();

    this->render_main_menu_bar();
    this->render_side_section();

    window->clear();
    ImGui::SFML::Render(*window);
    window->display();
}

bool UI::is_open() {
    return this->is_open_; 
}

  ///////////////
 /// PRIVATE ///
///////////////

void UI::render_main_menu_bar() {

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {

    }

    if (ImGui::BeginMenu("Edit")) {

    }
    
    if (ImGui::BeginMenu("View")) {

    }

    ImGui::EndMainMenuBar();
}

void UI::render_side_section() {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImVec2 viewport_pos = ImGui::GetMainViewport()->GetWorkCenter();
    ImVec2 viewport_size = ImGui::GetMainViewport()->WorkSize;

    ImVec2 size;
    size.x = viewport_size.x * 0.2f;
    size.y = viewport_size.y;
    ImGui::SetNextWindowSize(size);

    ImVec2 pos;
    pos.x = viewport_pos.x + viewport_size.x / 2 - size.x;
    pos.y = viewport_pos.y - viewport_size.y / 2;
    ImGui::SetNextWindowPos(pos);

    ImGui::Begin("Side section", &this->is_open_, window_flags);

    if (ImGui::CollapsingHeader("Metadata")) {
        static char title[1024] = "", artist[1024] = "", genre[1024] = "";
        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Artist", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Genre", title, IM_ARRAYSIZE(title));

        const char* modes[] = {"SP", "DP", "PM"};
        static int current_mode = 0;
        ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

        static int bpm = 0, total = 300;
        ImGui::InputInt("BPM", &bpm);
        ImGui::InputInt("Total", &total);

        const char* judges[] = {"Easy", "Normal", "Hard", "Very Hard"};
        static int current_judge = 0;
        ImGui::Combo("Judge", &current_judge, judges, IM_ARRAYSIZE(judges));

    }

    if (ImGui::CollapsingHeader("Keysounds")) {

    }

    ImGui::End();
}


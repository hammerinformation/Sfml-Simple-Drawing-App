#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include "imgui.h"
#include "imgui-SFML.h"
#include <cmath>


class Circle
{
public:
private:
    sf::CircleShape* circle_shape;
public:
    Circle();
    Circle(const Circle&) = delete;
    Circle& operator =(const Circle&) = delete;
    explicit operator sf::CircleShape*() const;
};

Circle::Circle()
{
    circle_shape = new sf::CircleShape;
}

Circle::operator sf::CircleShape*() const
{
    return circle_shape;
}


class Line
{
private:
    sf::Vertex line[2];
public:
    Line() = default;
    Line(const Line&) = delete;
    Line& operator =(const Line&) = delete;
    explicit operator sf::Vertex*();
};

Line::operator sf::Vertex*()
{
    return line;
}
 
sf::Text text;
sf::Font font;

float distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
    const auto vec = v1 - v2;
    return static_cast<float>(sqrt((vec.x * vec.x) + (vec.y * vec.y)));
}

void debug(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
    std::cout << "First Position: " << "x:" << std::to_string(v1.x) << " y:" << std::to_string(v1.y) << std::endl;
    std::cout << "Last Position: " << "x:" << std::to_string(v2.x) << " y:" << std::to_string(v2.y) << std::endl;
    std::cout << "Distance:" << distance(v1, v2) << std::endl;
}

enum class drawing_mode
{
    empty,
    line,
    circle
};

drawing_mode mode = drawing_mode::line;
std::vector<char> shapes;
std::vector<Circle*> circles;
int circle_index{-1};
int point_count{360};
float circle_color[3] = {1, 1, 1};
std::vector<Line*> lines;
int line_index{-1};
bool is_pressed = false;
bool left_control_pressed = false;
auto first_mouse_pos = sf::Vector2f(0, 0);
auto last_mouse_pos = sf::Vector2f(0, 0);
sf::Cursor cross_cursor, arrow_cursor;
sf::CircleShape point(0);

void set_drawing_mode(const drawing_mode& _mode)
{
    is_pressed = false;
    mode = _mode;
}


bool undo()
{
    if (!shapes.empty())
    {
        const auto shape = shapes.back();


        if (shape == 'L' && !lines.empty())
        {
            is_pressed = false;
            point.setRadius(0);
            const Line* line = lines.back();
            lines.pop_back();
            delete line;
            line_index--;
            shapes.pop_back();
            text.setString("");
            return true;
        }
        else if (shape == 'C' && !circles.empty())
        {
            is_pressed = false;
            point.setRadius(0);
            const Circle* circle = circles.back();
            circles.pop_back();
            delete circle;
            circle_index--;
            shapes.pop_back();
            text.setString("");
            return true;
        }
    }
    return false;
}

int main()
{
    font.loadFromFile("res/FredokaOne-Regular.ttf");
    text.setFont(font);
    text.setCharacterSize(25);
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Cad", sf::Style::Fullscreen);
    const auto menu_size = sf::Vector2f(250, 1080);
    cross_cursor.loadFromSystem(sf::Cursor::Cross);
    arrow_cursor.loadFromSystem(sf::Cursor::Arrow);
    point.setFillColor(sf::Color::Red);
    ImGui::SFML::Init(window);
    sf::Clock clock;
    window.setKeyRepeatEnabled(true);
    mode = drawing_mode::circle;
    while (window.isOpen())
    {
        const float delta_time = clock.getElapsedTime().asSeconds();
        int fps = 1.f / delta_time;
        clock.restart();
        sf::Event event;
        const auto mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        if (is_pressed && mousePos.x < menu_size.x)
        {
            const auto y = mousePos.y;
            sf::Mouse::setPosition(sf::Vector2i(menu_size.x - mousePos.x,
                                                static_cast<sf::CircleShape*>((*circles[circle_index]))->getPosition().
                                                y));
        }
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                    break;
                }
                if (event.key.code == sf::Keyboard::Z)
                {
                    if (undo())
                    {
                        window.setMouseCursor(arrow_cursor);
                    }

                    break;
                }
                if (event.key.code == sf::Keyboard::Num3)
                {
                    set_drawing_mode(drawing_mode::empty);

                    break;
                }
                if (event.key.code == sf::Keyboard::Num1)
                {
                    set_drawing_mode(drawing_mode::line);

                    break;
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    set_drawing_mode(drawing_mode::circle);


                    break;
                }
                break;
            case sf::Event::KeyReleased:

                break;


            case sf::Event::MouseButtonPressed:
                if (mode == drawing_mode::line)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (mousePos.x > menu_size.x)
                        {
                            first_mouse_pos = mousePos;
                            point.setRadius(5);

                            point.setPosition(first_mouse_pos);

                            Line* line = new Line;
                            lines.push_back(line);
                            shapes.push_back('L');
                            line_index++;
                            sf::Vertex* v = static_cast<sf::Vertex*>(*line);


                            v->position = first_mouse_pos;
                            is_pressed = true;
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        if (mousePos.x > menu_size.x)
                        {
                            window.setMouseCursor(arrow_cursor);

                            last_mouse_pos = mousePos;
                            sf::Vertex* v = static_cast<sf::Vertex*>(*lines[line_index]);
                            (v + 1)->position = last_mouse_pos;
                            is_pressed = false;
                            text.setString(std::to_string(static_cast<int>(distance(first_mouse_pos, last_mouse_pos))));
                            text.setPosition(last_mouse_pos + sf::Vector2f(10, 0));
                            first_mouse_pos = sf::Vector2f(0, 0);
                            last_mouse_pos = sf::Vector2f(0, 0);
                        }
                    }
                }
                if (mode == drawing_mode::circle)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (mousePos.x > menu_size.x)
                        {
                            first_mouse_pos = mousePos;
                            point.setRadius(5);

                            point.setPosition(first_mouse_pos);

                            Circle* circle = new Circle;

                            circles.push_back(circle);
                            shapes.push_back('C');
                            static_cast<sf::CircleShape*>(*circle)->setPointCount(point_count);
                            circle_index++;

                            sf::CircleShape* shape = static_cast<sf::CircleShape*>(*circle);

                            shape->setPosition(
                                first_mouse_pos + sf::Vector2<float>(-shape->getRadius(), -shape->getRadius()));

                            shape->setRadius(1.f);
                            shape->setPointCount(point_count);
                            shape->setFillColor(sf::Color::Transparent);
                            shape->setOutlineColor(sf::Color(circle_color[0] * 255, circle_color[1] * 255,
                                                             circle_color[2] * 255));
                            shape->setOutlineThickness(1.f);

                            is_pressed = true;
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        if (mousePos.x > menu_size.x)
                        {
                            window.setMouseCursor(arrow_cursor);

                            last_mouse_pos = mousePos;
                            is_pressed = false;
                        }
                    }
                }
                break;
            }
        }

        if (is_pressed && mousePos.x > menu_size.x)
        {
            if (first_mouse_pos.x != 0.f)
            {
                text.setString(std::to_string(static_cast<int>(distance(first_mouse_pos, mousePos))));
                text.setPosition(mousePos + sf::Vector2f(10, 0));
            }
            if (mode == drawing_mode::line)
            {
                window.setMouseCursor(cross_cursor);
                sf::Vertex* v = static_cast<sf::Vertex*>(*lines[line_index]);
                (v + 1)->position = mousePos;

                for (auto l : lines)
                {
                    sf::Vertex* l_vertex = static_cast<sf::Vertex*>(*l);
                    const auto d = distance((v + 1)->position, l_vertex->position);
                    const auto d2 = distance((v + 1)->position, (l_vertex + 1)->position);
                    if (d < 5 || d2 < 5)
                    {
                        point.setFillColor(sf::Color::Green);
                    }
                    else
                    {
                        point.setFillColor(sf::Color::Red);
                    }
                }
            }
            else if (mode == drawing_mode::circle)
            {
                window.setMouseCursor(cross_cursor);
                sf::CircleShape* circle_shape = static_cast<sf::CircleShape*>(*circles[circle_index]);

                circle_shape->setRadius(distance(first_mouse_pos, mousePos));
                circle_shape->setPosition(
                    first_mouse_pos + sf::Vector2<float>(-circle_shape->getRadius(), -circle_shape->getRadius()));
            }
        }

        window.clear(sf::Color::Black);

        ImGui::SFML::Update(window, sf::seconds(delta_time));
        ImGui::Begin("Settings");
        ImGui::SetWindowPos(sf::Vector2f(0, 0));
        ImGui::SetWindowSize(sf::Vector2f(250, 1080));
        ImGui::GetFont()->Scale = 1;
        ImGui::Text("Fps %d", fps);
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Line = %d ", lines.size());
        ImGui::Spacing();
        ImGui::Text("Circle = %d ", circles.size());
        if (mode == drawing_mode::empty && is_pressed == false)
        {
            ImGui::InputInt("Point Count", &point_count, 3, 360);
            ImGui::ColorEdit3("Circle Color", circle_color);
        }
        ImGui::Text("Mouse Position = %d %d", static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
        ImGui::End();
        ImGui::SFML::Render(window);


        for (auto l : lines)
        {
            sf::Vertex* v = static_cast<sf::Vertex*>(*l);
            sf::Vertex temp[2] = {
                sf::Vertex(sf::Vector2f(v->position)),
                sf::Vertex(sf::Vector2f((v + 1)->position))
            };

            window.draw(temp, 2, sf::Lines);
        }
        for (auto c : circles)
        {
            sf::CircleShape* shape = static_cast<sf::CircleShape*>(*c);
            window.draw(*shape);
        }
        window.draw(point);
        window.draw(text);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}

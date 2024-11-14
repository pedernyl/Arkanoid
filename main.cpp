#include <SFML/Graphics.hpp>
#include <vector>

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;
constexpr float paddleWidth = 100.0f;
constexpr float paddleHeight = 20.0f;
constexpr float ballRadius = 10.0f;
constexpr float blockWidth = 60.0f;
constexpr float blockHeight = 20.0f;

class Paddle {
public:
    sf::RectangleShape shape;
    float speed = 600.0f;

    Paddle(float startX, float startY) {
        shape.setPosition(startX, startY);
        shape.setSize({paddleWidth, paddleHeight});
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(paddleWidth / 2.0f, paddleHeight / 2.0f);
    }

    void move(float dx) {
        shape.move(dx * speed, 0.0f);
        if (shape.getPosition().x - paddleWidth / 2 < 0)
            shape.setPosition(paddleWidth / 2, shape.getPosition().y);
        if (shape.getPosition().x + paddleWidth / 2 > windowWidth)
            shape.setPosition(windowWidth - paddleWidth / 2, shape.getPosition().y);
    }
};

class Ball {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity{-300.0f, -300.0f};

    Ball(float startX, float startY) {
        shape.setPosition(startX, startY);
        shape.setRadius(ballRadius);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(ballRadius, ballRadius);
    }

    void update(float deltaTime) {
        shape.move(velocity * deltaTime);

        if (left() < 0 || right() > windowWidth)
            velocity.x = -velocity.x;
        if (top() < 0)
            velocity.y = -velocity.y;
    }

    float x() const { return shape.getPosition().x; }
    float y() const { return shape.getPosition().y; }
    float left() const { return x() - shape.getRadius(); }
    float right() const { return x() + shape.getRadius(); }
    float top() const { return y() - shape.getRadius(); }
    float bottom() const { return y() + shape.getRadius(); }
};

class Block {
public:
    sf::RectangleShape shape;
    bool destroyed{false};

    Block(float startX, float startY) {
        shape.setPosition(startX, startY);
        shape.setSize({blockWidth, blockHeight});
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin(blockWidth / 2.0f, blockHeight / 2.0f);
    }
};

void testCollision(Paddle& paddle, Ball& ball) {
    if (!paddle.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
        return;

    ball.velocity.y = -ball.velocity.y;
    if (ball.x() < paddle.shape.getPosition().x)
        ball.velocity.x = -std::abs(ball.velocity.x);
    else
        ball.velocity.x = std::abs(ball.velocity.x);
}

void testCollision(Block& block, Ball& ball) {
    if (block.destroyed || !block.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
        return;

    block.destroyed = true;
    ball.velocity.y = -ball.velocity.y;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Arkanoid");

    Paddle paddle(windowWidth / 2, windowHeight - 50);
    Ball ball(windowWidth / 2, windowHeight / 2);

    std::vector<Block> blocks;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 5; ++j) {
            blocks.emplace_back((i + 1) * (blockWidth + 3), (j + 1) * (blockHeight + 3));
        }
    }

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            paddle.move(-deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            paddle.move(deltaTime);

        ball.update(deltaTime);

        testCollision(paddle, ball);

        for (auto& block : blocks)
            testCollision(block, ball);

        if (ball.bottom() > windowHeight)
            window.close();

        window.clear();
        window.draw(paddle.shape);
        window.draw(ball.shape);
        for (const auto& block : blocks)
            if (!block.destroyed)
                window.draw(block.shape);
        window.display();
    }

    return 0;
}

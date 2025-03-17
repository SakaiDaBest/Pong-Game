#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>

using namespace sf;

int windowWidth = 1000;
int windowHeight = 700;
int playerScore = 0;
int aiScore = 0;
const int winningScore = 11;

class Bat {
private:
    Vector2f batPosition;
    double batSpeed = 1.5f;
    RectangleShape BatObject;

public:
    Bat(double x, double y);
    Vector2f getBatPosition();
    RectangleShape getBatObject();
    FloatRect getBatFloatRect();
    void moveBatUp();
    void moveBatDown();
    void update();
};

class AIBat {
private:
    Vector2f aiBatPosition;
    double aiBatSpeed = 2.4f;
    RectangleShape aiBatObject;

public:
    AIBat(double x, double y);
    Vector2f getAIBatPosition();
    RectangleShape getAIBatObject();
    FloatRect getAIBatFloatRect();
    void moveAIBatUp();
    void moveAIBatDown();
    void update();
    void followBall(Vector2f ballPosition);
};

class Ball {
private:
    RectangleShape BallObject;
    Vector2f ballPosition;
    double ballVelocityX = 0.5f;
    double ballVelocityY = 0.5f;

public:
    Ball(double x, double y);
    RectangleShape getBallObject();
    FloatRect getBallFloatRect();
    void reboundSides();
    void reboundBatorAI();
    void passTop();
    void passBottom();
    void update();
    void stop();
    void go();
    Vector2f getBallPosition();
};

static const float view_height = 512.0f;
void static ResizeView(const RenderWindow& window, View& view)
{
    float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
    view.setSize(view_height * aspectRatio, view_height);
}

//INT MAIN/////////////////

int main() {



    RenderWindow window(VideoMode(windowWidth, windowHeight), "Pong", sf::Style::Resize | Style::Close);
    View view(Vector2f(0.0f, 0.0f), Vector2f(view_height, view_height));
    // creating objects
    Bat bat(windowWidth - 20, windowHeight / 2); 
    Ball ball(windowWidth / 2, windowHeight / 2 + 50);
    AIBat aibat(10, windowHeight / 2);

    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        //user
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            bat.moveBatUp();
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) {
            bat.moveBatDown();
        }

        //LOGIC//////////
        ball.reboundSides();
        ball.passTop();
        ball.passBottom();

        if (ball.getBallFloatRect().intersects(bat.getBatFloatRect())) {
            ball.reboundBatorAI();
        }

        if (ball.getBallFloatRect().intersects(aibat.getAIBatFloatRect())) {
            ball.reboundBatorAI();
        }

        aibat.followBall(ball.getBallPosition());

        //TEXT and FONT
        std::stringstream ss;
        ss << "\t\t\t\t  AI Score: " << aiScore << "       Player Score: " << playerScore;

        Text text;
        Font font;
        font.loadFromFile("Nexa Light.otf");

        text.setFont(font);
        text.setCharacterSize(45);
        text.setFillColor(sf::Color::White);
        text.setString(ss.str());

        // WIN MESSAGE
        std::stringstream ss2;
        if (playerScore >= winningScore) {
            ss2 << "You win! Press any key to restart.";
        }
        else if (aiScore >= winningScore) {
            ss2 << "AI wins! Press any key to restart.";
        }
        Text winMessage;

        winMessage.setCharacterSize(50);
        winMessage.setPosition(windowWidth / 2 - 400, windowHeight / 2 - 100);
        winMessage.setFont(font);
        winMessage.setString(ss2.str());

        // UPDATE
        ball.update();
        bat.update();
        aibat.update();

        // DRAW
        window.clear(Color(0, 0, 0, 255));

        if (playerScore >= winningScore || aiScore >= winningScore) {
            window.draw(winMessage);
            ball.stop();

            if (event.type == Event::KeyPressed) {
                playerScore = 0;
                aiScore = 0;
                ball.go();
            }
        }
        else {
            window.draw(bat.getBatObject());
            window.draw(ball.getBallObject());
            window.draw(text);
            window.draw(aibat.getAIBatObject());
        }

        window.display();
    }

    return 0;
}

// BAT functions

Bat::Bat(double x, double y) {
    batPosition.x = x;
    batPosition.y = y;
    BatObject.setSize(sf::Vector2f(3, 100));
    BatObject.setPosition(batPosition);
}

Vector2f Bat::getBatPosition() {
    return batPosition;
}

RectangleShape Bat::getBatObject() {
    return BatObject;
}

FloatRect Bat::getBatFloatRect() {
    return BatObject.getGlobalBounds();
}

void Bat::moveBatUp() {
    if (batPosition.y > 0) {
        batPosition.y -= batSpeed;
    }
}

void Bat::moveBatDown() {
    if (batPosition.y + BatObject.getSize().y < windowHeight) {
        batPosition.y += batSpeed;
    }
}

void Bat::update() {
    BatObject.setPosition(batPosition);
}

// AIBAT functions

AIBat::AIBat(double x, double y) {
    aiBatPosition.x = x;
    aiBatPosition.y = y;
    aiBatObject.setSize(sf::Vector2f(3, 100));//size,coordinates
    aiBatObject.setPosition(aiBatPosition);
}

RectangleShape AIBat::getAIBatObject() {
    return aiBatObject;
}

Vector2f AIBat::getAIBatPosition() {
    return aiBatPosition;
}

FloatRect AIBat::getAIBatFloatRect() {
    return aiBatObject.getGlobalBounds();
}

void AIBat::moveAIBatUp() {
    if (aiBatPosition.y > 0) {
        aiBatPosition.y -= aiBatSpeed;
    }
}

void AIBat::moveAIBatDown() {
    if (aiBatPosition.y + aiBatObject.getSize().y < windowHeight) {
        aiBatPosition.y += aiBatSpeed;
    }
}

void AIBat::update() {
    aiBatObject.setPosition(aiBatPosition);
}

void AIBat::followBall(Vector2f ballPosition) {
    if (ballPosition.y < aiBatPosition.y) {
        moveAIBatUp();
    }
    else if (ballPosition.y > aiBatPosition.y + aiBatObject.getSize().y) {
        moveAIBatDown();
    }
}

// BALL

Ball::Ball(double x, double y) {
    ballPosition.x = x;
    ballPosition.y = y;
    BallObject.setSize(sf::Vector2f(10, 10));
    BallObject.setPosition(ballPosition);
    BallObject.setFillColor(sf::Color::White);
}

RectangleShape Ball::getBallObject() {
    return BallObject;
}

FloatRect Ball::getBallFloatRect() {
    return BallObject.getGlobalBounds();
}

void Ball::update() {
    ballPosition.x += ballVelocityX;
    ballPosition.y += ballVelocityY;
    BallObject.setPosition(ballPosition);
    ballVelocityX *= 1.0005;
    ballVelocityY *= 1.0005;
}

void Ball::reboundSides() {
    if (ballPosition.y <= 0 || ballPosition.y + BallObject.getSize().y >= windowHeight) {
        ballVelocityY = -ballVelocityY;
    }
}

void Ball::passTop() {
    if (ballPosition.x <= 0) {
        playerScore++;
        ballVelocityX = 0.5f;
        ballVelocityY = 0.5f;
        ballPosition.x = windowWidth / 2;
        ballPosition.y = windowHeight / 2;
        ballVelocityX = -ballVelocityX;
        if (rand() % 2 == 1) {
            ballVelocityY = -ballVelocityY;
        }
    }
}

void Ball::passBottom() {
    if (ballPosition.x + BallObject.getSize().x >= windowWidth) {
        aiScore++;
        ballVelocityX = 0.5f;
        ballVelocityY = 0.5f;
        ballPosition.x = windowWidth / 2;
        ballPosition.y = windowHeight / 2;
        ballVelocityX = -ballVelocityX;
        if (rand() % 2 == 1) {
            ballVelocityY = -ballVelocityY;
        }
    }
}

void Ball::reboundBatorAI() {
    ballVelocityX = -ballVelocityX;
}

void Ball::stop() {
    ballVelocityY = 0;
    ballVelocityX = 0;
}

void Ball::go() {
    ballVelocityY = 0.5f;
    ballVelocityX = 0.5f;
}

Vector2f Ball::getBallPosition() {
    return ballPosition;
}

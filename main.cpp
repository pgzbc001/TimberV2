// Include important libraries here
#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Audio.hpp>
#include <memory>
#include <unistd.h>

using namespace sf;

void updateBranches(int seed);

const int NUM_BRANCHES = 6;
std::unique_ptr<sf::Sprite> branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main()
{
    chdir("/Users/cwenbo/project/cpp/Timber/TimberV2");

    // 获取屏幕分辨率
    VideoMode desktop = VideoMode::getDesktopMode();
    float screenWidth = desktop.size.x;
    float screenHeight = desktop.size.y;

    // MacBook Air 13寸 Retina 通常是 2560x1600，逻辑分辨率是 1440x900
    // 这里我们使用 1440x900 作为目标分辨率
    float targetWidth = 1440.0f;
    float targetHeight = 900.0f;

    // 计算缩放因子（相对于原设计 1920x1080）
    float scaleX = targetWidth / 1920.0f;
    float scaleY = targetHeight / 1080.0f;
    float scale = std::min(scaleX, scaleY); // 保持比例，取较小的缩放值

    // 使用固定的 1440x900 分辨率（适合 MacBook Air 13寸）
    // SFML 3.0 使用 Vector2u 作为参数
    VideoMode vm(Vector2u(1440, 900));
    RenderWindow window(vm, "Timber!!!", Style::Default);

    // 如果想要全屏，取消注释下面这行
    // RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

    // 缩放因子，用于调整所有元素的位置和大小
    float scaleFactor = 1440.0f / 1920.0f; // 0.75

    Texture textureBackground;
    if (!textureBackground.loadFromFile("graphics/background.png")) {
        return -1;
    }
    Sprite spriteBackground(textureBackground);
    spriteBackground.setPosition({0, 0});
    // 背景图片缩放以填满窗口
    spriteBackground.setScale({scaleFactor, scaleFactor});

    Texture textureTree;
    if (!textureTree.loadFromFile("graphics/tree.png")) {
        return -1;
    }
    Sprite spriteTree(textureTree);
    spriteTree.setPosition({810 * scaleFactor, 0});
    spriteTree.setScale({scaleFactor, scaleFactor});

    Texture textureTree2;
    if (!textureTree2.loadFromFile("graphics/tree2.png")) {
        return -1;
    }
    Sprite spriteTree2(textureTree2);
    spriteTree2.setPosition({20 * scaleFactor, 0});
    spriteTree2.setScale({scaleFactor, scaleFactor});

    Sprite spriteTree3(textureTree2);
    spriteTree3.setPosition({300 * scaleFactor, -400 * scaleFactor});
    spriteTree3.setScale({scaleFactor, scaleFactor});

    Sprite spriteTree4(textureTree2);
    spriteTree4.setPosition({1300 * scaleFactor, -400 * scaleFactor});
    spriteTree4.setScale({scaleFactor, scaleFactor});

    Sprite spriteTree5(textureTree2);
    spriteTree5.setPosition({1500 * scaleFactor, -500 * scaleFactor});
    spriteTree5.setScale({scaleFactor, scaleFactor});

    Sprite spriteTree6(textureTree2);
    spriteTree6.setPosition({1900 * scaleFactor, 0});
    spriteTree6.setScale({scaleFactor, scaleFactor});

    Texture textureBee;
    if (!textureBee.loadFromFile("graphics/bee.png")) {
        return -1;
    }
    Sprite spriteBee(textureBee);
    spriteBee.setPosition({0, 800 * scaleFactor});
    spriteBee.setScale({scaleFactor, scaleFactor});

    bool beeActive = false;
    float beeSpeed = 0.0f;

    Texture textureCloud;
    if (!textureCloud.loadFromFile("graphics/cloud.png")) {
        return -1;
    }

    const int NUM_CLOUDS = 6;
    std::unique_ptr<sf::Sprite> clouds[NUM_CLOUDS];
    int cloudSpeeds[NUM_CLOUDS];
    bool cloudsActive[NUM_CLOUDS];

    for (int i = 0; i < NUM_CLOUDS; i++)
    {
        clouds[i] = std::make_unique<sf::Sprite>(textureCloud);
        clouds[i]->setPosition({-300 * scaleFactor, (float)i * 150 * scaleFactor});
        clouds[i]->setScale({scaleFactor, scaleFactor});
        cloudsActive[i] = false;
        cloudSpeeds[i] = 0;
    }

    Clock clock;
    RectangleShape timeBar;
    float timeBarStartWidth = 400 * scaleFactor;
    float timeBarHeight = 80 * scaleFactor;

    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition({(1440 / 2.0f) - timeBarStartWidth / 2, 900 - 100 * scaleFactor});

    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    bool paused = true;
    int score = 0;

    Font font;
    if (!font.openFromFile("fonts/KOMIKAP_.ttf")) {
        return -1;
    }

    // 调整字体大小
    int fontSize = static_cast<int>(75 * scaleFactor);
    int scoreFontSize = static_cast<int>(100 * scaleFactor);
    int fpsFontSize = static_cast<int>(100 * scaleFactor);

    Text messageText(font, "Press Enter to start!", fontSize);
    Text scoreText(font, "Score = 0", scoreFontSize);
    Text fpsText(font, "", fpsFontSize);

    fpsText.setFillColor(Color::White);
    fpsText.setPosition({1200 * scaleFactor, 20 * scaleFactor});

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin({
        textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f
    });
    messageText.setPosition({1440 / 2.0f, 900 / 2.0f});
    scoreText.setPosition({20 * scaleFactor, 20 * scaleFactor});

    RectangleShape rect1;
    rect1.setFillColor(sf::Color(0, 0, 0, 150));
    rect1.setSize(Vector2f(600 * scaleFactor, 105 * scaleFactor));
    rect1.setPosition({0, 30 * scaleFactor});

    RectangleShape rect2;
    rect2.setFillColor(sf::Color(0, 0, 0, 150));
    rect2.setSize(Vector2f(1000 * scaleFactor, 105 * scaleFactor));
    rect2.setPosition({1150 * scaleFactor, 30 * scaleFactor});

    Texture textureBranch;
    if (!textureBranch.loadFromFile("graphics/branch.png")) {
        return -1;
    }

    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i] = std::make_unique<sf::Sprite>(textureBranch);
        branches[i]->setPosition({-2000 * scaleFactor, -2000 * scaleFactor});
        branches[i]->setOrigin({220 * scaleFactor, 20 * scaleFactor});
        branches[i]->setScale({scaleFactor, scaleFactor});
    }

    Texture texturePlayer;
    if (!texturePlayer.loadFromFile("graphics/player.png")) {
        return -1;
    }
    Sprite spritePlayer(texturePlayer);
    spritePlayer.setPosition({580 * scaleFactor, 720 * scaleFactor});
    spritePlayer.setScale({scaleFactor, scaleFactor});

    side playerSide = side::LEFT;

    Texture textureRIP;
    if (!textureRIP.loadFromFile("graphics/rip.png")) {
        return -1;
    }
    Sprite spriteRIP(textureRIP);
    spriteRIP.setPosition({600 * scaleFactor, 860 * scaleFactor});
    spriteRIP.setScale({scaleFactor, scaleFactor});

    Texture textureAxe;
    if (!textureAxe.loadFromFile("graphics/axe.png")) {
        return -1;
    }
    Sprite spriteAxe(textureAxe);
    spriteAxe.setPosition({700 * scaleFactor, 830 * scaleFactor});
    spriteAxe.setScale({scaleFactor, scaleFactor});

    const float AXE_POSITION_LEFT = 700.0f * scaleFactor;
    const float AXE_POSITION_RIGHT = 1075.0f * scaleFactor;

    Texture textureLog;
    if (!textureLog.loadFromFile("graphics/log.png")) {
        return -1;
    }
    Sprite spriteLog(textureLog);
    spriteLog.setPosition({810 * scaleFactor, 720 * scaleFactor});
    spriteLog.setScale({scaleFactor, scaleFactor});

    bool logActive = false;
    float logSpeedX = 1000 * scaleFactor;
    float logSpeedY = -1500 * scaleFactor;

    bool acceptInput = false;

    SoundBuffer chopBuffer;
    if (!chopBuffer.loadFromFile("sound/chop.wav")) {
        return -1;
    }
    Sound chop(chopBuffer);

    SoundBuffer deathBuffer;
    if (!deathBuffer.loadFromFile("sound/death.wav")) {
        return -1;
    }
    Sound death(deathBuffer);

    SoundBuffer ootBuffer;
    if (!ootBuffer.loadFromFile("sound/out_of_time.wav")) {
        return -1;
    }
    Sound outOfTime(ootBuffer);

    int lastDrawn = 0;

    // 添加一个计时器来控制树枝自动下落
    float branchTimer = 0.0f;
    const float BRANCH_INTERVAL = 0.5f; // 每0.5秒下落一个树枝

    // 输出当前分辨率信息
    printf("Screen resolution: %.0fx%.0f\n", screenWidth, screenHeight);
    printf("Window size: 1440x900\n");
    printf("Scale factor: %.2f\n", scaleFactor);

    while (window.isOpen())
    {
        // 处理事件
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // 处理窗口大小改变
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // 可以在这里处理窗口大小改变
                // 但我们保持固定大小
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
            window.close();
        }

        // 检查是否按下了 Enter 键
        if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
        {
            if (paused) {
                // 重新开始游戏
                paused = false;
                score = 0;
                timeRemaining = 6.0f;
                acceptInput = true;

                // 重置所有树枝
                for (int i = 0; i < NUM_BRANCHES; i++)
                {
                    branchPositions[i] = side::NONE;
                }

                spriteRIP.setPosition({675 * scaleFactor, 2000 * scaleFactor});
                spritePlayer.setPosition({580 * scaleFactor, 720 * scaleFactor});
                playerSide = side::LEFT;

                // 隐藏消息
                messageText.setString("");

                // 重置计时器
                branchTimer = 0.0f;
            }
        }

        // 游戏更新
        if (!paused)
        {
            Time dt = clock.restart();
            float dtSeconds = dt.asSeconds();

            // 更新时间
            timeRemaining -= dtSeconds;
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f) {
                paused = true;
                messageText.setString("Out of time!!");
                messageText.setFillColor(Color::White);
                messageText.setCharacterSize(static_cast<unsigned int>(75 * scaleFactor));

                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({
                    textRect.position.x + textRect.size.x / 2.0f,
                    textRect.position.y + textRect.size.y / 2.0f
                });
                messageText.setPosition({1440 / 2.0f, 900 / 2.0f});
                outOfTime.play();
            }

            // 自动更新树枝
            branchTimer += dtSeconds;
            if (branchTimer >= BRANCH_INTERVAL) {
                branchTimer = 0.0f;
                updateBranches(score);
            }

            // 处理玩家输入
            if (acceptInput)
            {
                // 检查左箭头
                if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                {
                    playerSide = side::LEFT;
                    score++;
                    timeRemaining += (2.0f / score) + 0.15f;

                    spriteAxe.setPosition({AXE_POSITION_LEFT,
                        spriteAxe.getPosition().y});
                    spritePlayer.setPosition({580 * scaleFactor, 720 * scaleFactor});

                    spriteLog.setPosition({810 * scaleFactor, 720 * scaleFactor});
                    logSpeedX = 5000 * scaleFactor;
                    logActive = true;

                    acceptInput = false;
                    chop.play();
                }

                // 检查右箭头
                if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                {
                    playerSide = side::RIGHT;
                    score++;
                    timeRemaining += (2.0f / score) + 0.15f;

                    spriteAxe.setPosition({AXE_POSITION_RIGHT,
                        spriteAxe.getPosition().y});
                    spritePlayer.setPosition({1200 * scaleFactor, 720 * scaleFactor});

                    spriteLog.setPosition({810 * scaleFactor, 720 * scaleFactor});
                    logSpeedX = -5000 * scaleFactor;
                    logActive = true;

                    acceptInput = false;
                    chop.play();
                }
            }

            // 允许在释放按键后重新接受输入
            if (!Keyboard::isKeyPressed(Keyboard::Key::Left) &&
                !Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                acceptInput = true;
                // 隐藏斧头
                spriteAxe.setPosition({2000 * scaleFactor, spriteAxe.getPosition().y});
            }

            // 蜜蜂逻辑
            if (!beeActive)
            {
                srand((int)time(0) * 10);
                beeSpeed = ((rand() % 200) + 200) * scaleFactor;

                srand((int)time(0) * 10);
                float height = ((rand() % 500) + 500) * scaleFactor;
                spriteBee.setPosition({2000 * scaleFactor, height});
                beeActive = true;
            }
            else
            {
                spriteBee.setPosition({
                    spriteBee.getPosition().x - (beeSpeed * dtSeconds),
                    spriteBee.getPosition().y});

                if (spriteBee.getPosition().x < -100 * scaleFactor)
                {
                    beeActive = false;
                }
            }

            // 云朵逻辑
            for (int i = 0; i < NUM_CLOUDS; i++)
            {
                if (!cloudsActive[i])
                {
                    srand((int)time(0) * i);
                    cloudSpeeds[i] = ((rand() % 200)) * scaleFactor;

                    srand((int)time(0) * i);
                    float height = (rand() % 150) * scaleFactor;
                    clouds[i]->setPosition({-200 * scaleFactor, height});
                    cloudsActive[i] = true;
                }
                else
                {
                    clouds[i]->setPosition({
                        clouds[i]->getPosition().x + (cloudSpeeds[i] * dtSeconds),
                        clouds[i]->getPosition().y
                    });

                    if (clouds[i]->getPosition().x > 1440)
                    {
                        cloudsActive[i] = false;
                    }
                }
            }

            // 更新分数和FPS
            lastDrawn++;
            if (lastDrawn == 100) {
                std::stringstream ss;
                ss << "Score = " << score;
                scoreText.setString(ss.str());

                std::stringstream ss2;
                ss2 << "FPS = " << 1.0f / dtSeconds;
                fpsText.setString(ss2.str());
                lastDrawn = 0;
            }

            // 更新树枝精灵位置
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150 * scaleFactor;
                if (branchPositions[i] == side::LEFT)
                {
                    branches[i]->setPosition({610 * scaleFactor, height});
                    branches[i]->setOrigin({220 * scaleFactor, 40 * scaleFactor});
                    branches[i]->setRotation(sf::degrees(180));
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    branches[i]->setPosition({1330 * scaleFactor, height});
                    branches[i]->setOrigin({220 * scaleFactor, 40 * scaleFactor});
                    branches[i]->setRotation(sf::degrees(0));
                }
                else
                {
                    branches[i]->setPosition({3000 * scaleFactor, height});
                }
            }

            // 飞行的木头
            if (logActive)
            {
                spriteLog.setPosition({
                    spriteLog.getPosition().x + (logSpeedX * dtSeconds),
                    spriteLog.getPosition().y + (logSpeedY * dtSeconds)});

                if (spriteLog.getPosition().x < -100 * scaleFactor ||
                    spriteLog.getPosition().x > 1440 + 100 * scaleFactor)
                {
                    logActive = false;
                    spriteLog.setPosition({810 * scaleFactor, 720 * scaleFactor});
                }
            }

            // 检查玩家是否被砸到
            if (branchPositions[5] == playerSide)
            {
                paused = true;
                acceptInput = false;

                spriteRIP.setPosition({525 * scaleFactor, 760 * scaleFactor});
                spritePlayer.setPosition({2000 * scaleFactor, 660 * scaleFactor});

                messageText.setString("SQUISHED!!");
                messageText.setFillColor(Color::Red);
                messageText.setCharacterSize(static_cast<unsigned int>(75 * scaleFactor));

                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({
                    textRect.position.x + textRect.size.x / 2.0f,
                    textRect.position.y + textRect.size.y / 2.0f
                });
                messageText.setPosition({1440 / 2.0f, 900 / 2.0f});

                death.play();
            }
        }

        // 绘制
        window.clear();
        window.draw(spriteBackground);

        for (int i = 0; i < NUM_CLOUDS; i++)
        {
            window.draw(*clouds[i]);
        }

        window.draw(spriteTree2);
        window.draw(spriteTree3);
        window.draw(spriteTree4);
        window.draw(spriteTree5);
        window.draw(spriteTree6);

        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(*branches[i]);
        }

        window.draw(spriteTree);
        window.draw(spritePlayer);
        window.draw(spriteAxe);
        window.draw(spriteLog);
        window.draw(spriteRIP);
        window.draw(rect1);
        window.draw(rect2);
        window.draw(spriteBee);
        window.draw(scoreText);
        window.draw(fpsText);
        window.draw(timeBar);

        if (paused && !messageText.getString().isEmpty())
        {
            window.draw(messageText);
        }

        window.display();
    }
    return 0;
}

void updateBranches(int seed)
{
    // 所有树枝下移一位
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    // 在顶部生成新树枝
    srand((int)time(0) + seed);
    int r = (rand() % 5);

    switch (r) {
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}
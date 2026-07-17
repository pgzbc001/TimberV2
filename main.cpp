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

    VideoMode vm({1920, 1080});
    RenderWindow window(vm, "Timber!!!", Style::Default);

    Texture textureBackground;
    if (!textureBackground.loadFromFile("graphics/background.png")) {
        return -1;
    }
    Sprite spriteBackground(textureBackground);
    spriteBackground.setPosition({0, 0});

    Texture textureTree;
    if (!textureTree.loadFromFile("graphics/tree.png")) {
        return -1;
    }
    Sprite spriteTree(textureTree);
    spriteTree.setPosition({810, 0});

    Texture textureTree2;
    if (!textureTree2.loadFromFile("graphics/tree2.png")) {
        return -1;
    }
    Sprite spriteTree2(textureTree2);
    spriteTree2.setPosition({20, 0});

    Sprite spriteTree3(textureTree2);
    spriteTree3.setPosition({300, -400});

    Sprite spriteTree4(textureTree2);
    spriteTree4.setPosition({1300, -400});

    Sprite spriteTree5(textureTree2);
    spriteTree5.setPosition({1500, -500});

    Sprite spriteTree6(textureTree2);
    spriteTree6.setPosition({1900, 0});

    Texture textureBee;
    if (!textureBee.loadFromFile("graphics/bee.png")) {
        return -1;
    }
    Sprite spriteBee(textureBee);
    spriteBee.setPosition({0, 800});

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
        clouds[i]->setPosition({-300, (float)i * 150});
        cloudsActive[i] = false;
        cloudSpeeds[i] = 0;
    }

    Clock clock;
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;

    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition({(1920 / 2.0f) - timeBarStartWidth / 2, 980});

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    bool paused = true;
    int score = 0;

    Font font;
    if (!font.openFromFile("fonts/KOMIKAP_.ttf")) {
        return -1;
    }

    Text messageText(font, "Press Enter to start!", 75);
    Text scoreText(font, "Score = 0", 100);
    Text fpsText(font, "", 100);

    fpsText.setFillColor(Color::White);
    fpsText.setCharacterSize(100);
    fpsText.setPosition({1200, 20});

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin({
        textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f
    });
    messageText.setPosition({1920 / 2.0f, 1080 / 2.0f});
    scoreText.setPosition({20, 20});

	RectangleShape rect1;
	rect1.setFillColor(sf::Color(0, 0, 0, 150));
	rect1.setSize(Vector2f(600, 105));
	rect1.setPosition({0, 30});

    RectangleShape rect2;
    rect2.setFillColor(sf::Color(0, 0, 0, 150));
    rect2.setSize(Vector2f(1000, 105));
    rect2.setPosition({1150, 30});

    Texture textureBranch;
    if (!textureBranch.loadFromFile("graphics/branch.png")) {
        return -1;
    }

    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i] = std::make_unique<sf::Sprite>(textureBranch);
        branches[i]->setPosition({-2000, -2000});
        branches[i]->setOrigin({220, 20});
    }

    Texture texturePlayer;
    if (!texturePlayer.loadFromFile("graphics/player.png")) {
        return -1;
    }
    Sprite spritePlayer(texturePlayer);
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition({580, 720});

    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare the gravestone
    Texture textureRIP;
    if (!textureRIP.loadFromFile("graphics/rip.png")) {
        return -1;
    }
    Sprite spriteRIP(textureRIP);
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition({600, 860});

    // Prepare the axe
    Texture textureAxe;
    if (!textureAxe.loadFromFile("graphics/axe.png")) {
        return -1;
    }
    Sprite spriteAxe(spritePlayer);
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition({700, 830});

    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700.0f;
    const float AXE_POSITION_RIGHT = 1075.0f;

    // Prepare the flying log
    Texture textureLog;
    if (!textureLog.loadFromFile("graphics/log.png")) {
        return -1;
    }
    Sprite spriteLog(textureLog);
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition({810, 720});

    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // Control the player input
    bool acceptInput = false;

    // Prepare the sound
    SoundBuffer chopBuffer;
    if (!chopBuffer.loadFromFile("sound/chop.wav")) {
        return -1;
    }
    Sound chop(chopBuffer);
    chop.setBuffer(chopBuffer);

    SoundBuffer deathBuffer;
    if (!deathBuffer.loadFromFile("sound/death.wav")) {
        return -1;
    }
    Sound death(deathBuffer);
    death.setBuffer(deathBuffer);

    // Out of time
    SoundBuffer ootBuffer;
    if (!ootBuffer.loadFromFile("sound/out_of_time.wav")) {
        return -1;
    }
    Sound outOfTime(ootBuffer);
    outOfTime.setBuffer(ootBuffer);

    // control the drawing of the score
    int lastDrawn = 0;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent()) {
            // if (event->is<sf::Event::Closed>()) {
            //     window.close();
            // }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (!paused) {
                    acceptInput = true;
                    spriteAxe.setPosition({2000,
                        spriteAxe.getPosition().y});
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
            {
                window.close();
            }

            // Start the game
            if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
            {
                paused = false;
                score = 0;
                timeRemaining = 6;

                // Make all the branches disappear
                for (int i = 1; i < NUM_BRANCHES; i++)
                {
                    branchPositions[i] = side::NONE;
                }

                spriteRIP.setPosition({675, 2000});
                spritePlayer.setPosition({580, 720});
                acceptInput = true;
            }

            if (acceptInput) {
                if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                {
                    playerSide = side::RIGHT;
                    score++;

                    timeRemaining += (2 / score) + .15;
                    spriteAxe.setPosition({AXE_POSITION_RIGHT,
                        spriteAxe.getPosition().y});

                    spritePlayer.setPosition({1200, 720});

                    // update the branches
                    updateBranches(score);

                    // set the log flying to the left
                    spriteLog.setPosition({810, 720});
                    logSpeedX = -5000;
                    logActive = true;

                    acceptInput = false;
                    chop.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                {
                    playerSide = side::LEFT;
                    score++;
                    timeRemaining += (2 / score) + .15;

                    spriteAxe.setPosition({AXE_POSITION_LEFT,
                        spriteAxe.getPosition().y});
                    spritePlayer.setPosition({580, 720});

                    // update the branches
                    updateBranches(score);

                    // set the log flying
                    spriteLog.setPosition({810, 720});
                    logSpeedX = 5000;
                    logActive = true;

                    acceptInput = false;
                    chop.play();
                }
            }

            if (!paused)
            {
				Time dt = clock.restart();
				timeRemaining -= dt.asSeconds();
				timeBar.setSize(Vector2f(timeBarWidthPerSecond *
					timeRemaining, timeBarHeight));

				if (timeRemaining <= 0.0f) {
					paused = true;
					messageText.setString("Out of time!!");

					FloatRect textRect = messageText.getLocalBounds();
					messageText.setOrigin({textRect.position.x +
						textRect.size.x / 2.0f,
						textRect.position.y +
						textRect.size.y / 2.0f});

					messageText.setPosition({1920 / 2.0f, 1080 / 2.0f});
					outOfTime.play();
				}

				if (!beeActive)
				{
					srand((int)time(0) * 10);
					beeSpeed = (rand() % 200) + 200;

					srand((int)time(0) * 10);
					float height = (rand() % 500) + 500;
					spriteBee.setPosition({2000, height});
					beeActive = true;
				}
				else
				{
					spriteBee.setPosition({
						spriteBee.getPosition().x -
						(beeSpeed * dt.asSeconds()),
						spriteBee.getPosition().y});

					if (spriteBee.getPosition().x < -100)
					{
						beeActive = false;
					}
				}

				for (int i = 0; i < NUM_CLOUDS; i++)
				{
					if (!cloudsActive[i])
					{
						srand((int)time(0) * i);
						cloudSpeeds[i] = (rand() % 200);

						srand((int)time(0) * i);
						float height = (rand() % 150);
						clouds[i]->setPosition({-200, height});
						cloudsActive[i] = true;
					}
					else
					{
						clouds[i]->setPosition( {
							clouds[i]->getPosition().x +
							(cloudSpeeds[i] * dt.asSeconds()),
							clouds[i]->getPosition().y
						});

						if (clouds[i]->getPosition().x > 1920)
						{
							cloudsActive[i] = false;
						}

					}
				}

				lastDrawn++;
				if (lastDrawn == 100) {
					// Update the score text
					std::stringstream ss;
					ss << "Score = " << score;
					scoreText.setString(ss.str());

					// Draw the fps
					std::stringstream ss2;
					ss2 << "FPS = " << 1/dt.asSeconds();
					fpsText.setString(ss2.str());
					lastDrawn = 0;
				}

				// update the branch sprites
				for (int i = 0; i < NUM_BRANCHES; i++)
				{
					float height = i * 150;
					if (branchPositions[i] == side::LEFT)
					{
						branches[i]->setPosition({610, height});
						branches[i]->setOrigin({220, 40});
						branches[i]->setRotation( degrees(180));
					}
					else if (branchPositions[i] == side::RIGHT)
					{
						branches[i]->setPosition({1330, height});
						branches[i]->setOrigin({220, 40});
						branches[i]->setRotation( degrees(0));

					}
					else
					{
						branches[i]->setPosition({3000, height});
					}
				}

				// Handle a flying log
				if (logActive)
				{
					spriteLog.setPosition({
						spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
						spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())});

					// Has the insect reached the right hand edge of the screen?
					if (spriteLog.getPosition().x < -100 ||
						spriteLog.getPosition().x > 2000)
					{
						// Set it up ready to be a whole new cloud next frame
						logActive = false;
						spriteLog.setPosition({810, 720});
					}
				}

				// has the player been squished by a branch?
				if (branchPositions[5] == playerSide)
				{
					// death
					paused = true;
					acceptInput = false;

					// Draw the gravestone
					spriteRIP.setPosition({525, 760});

					// hide the player
					spritePlayer.setPosition({2000, 660});

					// Change the text of the message
					messageText.setString("SQUISHED!!");

					// Center it on the screen
					FloatRect textRect = messageText.getLocalBounds();

					messageText.setOrigin({textRect.position.x +
						textRect.size.x / 2.0f,
						textRect.position.y + textRect.size.y / 2.0f});

					messageText.setPosition({1920 / 2.0f,
						1080 / 2.0f});

					// Play the death sound
					death.play();
			}
            }
        }

    	window.clear();
    	window.draw(spriteBackground);

    	for (int i = 0; i < NUM_CLOUDS; i++)
    	{
    		window.draw(*clouds[i]);
    	}

    	// Enhanced version
    	window.draw(spriteTree2);
    	window.draw(spriteTree3);
    	window.draw(spriteTree4);
    	window.draw(spriteTree5);
    	window.draw(spriteTree6);

    	// Draw the branches
    	for (int i = 0; i < NUM_BRANCHES; i++) {
    		window.draw(*branches[i]);
    	}

    	// Draw the tree
    	window.draw(spriteTree);

    	// Draw the player
    	window.draw(spritePlayer);

    	// Draw the axe
    	window.draw(spriteAxe);

    	// Draraw the flying log
    	window.draw(spriteLog);

    	// Draw the gravestone
    	window.draw(spriteRIP);

    	// Draw backgrounds for the text
    	window.draw(rect1);
    	window.draw(rect2);

    	// Drawraw the bee
    	window.draw(spriteBee);

    	// Draw the score
    	window.draw(scoreText);

    	// Draw the FPS
    	window.draw(fpsText);

    	// Draw the timebar
    	window.draw(timeBar);

    	if (paused)
    	{
    		window.draw(messageText);
    	}

    	// Show everything we just drew
    	window.display();
    }
    return 0;
}

void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
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
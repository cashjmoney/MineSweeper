
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "SFML/TextureManager.h"
#include <conio.h> 
#include <fstream>
#include <random>
#include <chrono> 
#include <cmath> 
#include <Windows.h>


		// We are used to seeing std:: prefix to indicate we use standard library
		// SFML uses sf:: prefix
			//Things I need to Fix
		//////put on the texture and and windows in seperate classes 
		///// USE A RELATIVE PATH FOR CONFIG FILES AND POSSIBLIE OTHER IMAGES 
		////// FIX THE CURSOR FUNCTIONAL
		////// Fix bomb generation in board constructor
		///// implement decrease flagged reminder
		/////the game->lose making the mouse not interact with the window which is the only way to make everything move again 
		std::map<int, sf::Sprite> parseDigits(sf::Sprite digits) {
			std::map<int, sf::Sprite> digitsMap;

			for (int i = 0; i < 10; i++) {
				sf::IntRect rect(i * 21, 0, 21, 32);
				digits.setTextureRect(rect);
				sf::Sprite sprite = digits;
				digitsMap.emplace(i, sprite);
			}
			return digitsMap;
		};
		int main() {

			struct tile_attributes
			{
				int adjacent_mines = 0;
				bool flag = false;
				bool tile = true;
				bool mine = false;
				bool num_here = false;
				int x = 0;
				int y = 0;
				bool stop_counting_flag = false;
				bool i_win = false;
				bool i_lose = false;
				vector<tile_attributes*> surround;

				sf::Texture& Tile_hiddenTexture = TextureManager::getTexture("tile_hidden");
				sf::Sprite tile_hidden;
				sf::Sprite number;
				sf::Texture num;
				string texture_name = "number_";


				void minesweep(tile_attributes& tile) {
					// Reveal the tile
					if (tile.flag != true)
					{
						tile.tile_hidden = sf::Sprite();
						tile.tile = false;
					}


					if (tile.adjacent_mines == 0) {
						for (size_t i = 0; i < tile.surround.size(); ++i) {
							tile_attributes* surrounding_tile = tile.surround[i];
							// If the surrounding tile is hidden and not a mine, recursively reveal it
							if (surrounding_tile->tile_hidden.getTexture() != nullptr && !surrounding_tile->mine)
							{
								minesweep(*surrounding_tile);
							}

						}
					}
					else if (tile.adjacent_mines != 0 && tile.flag != true)
					{

						tile.num_here = true;

					}

				}

			};






			class ConfigReader {
			private:

				std::ifstream file;

			public:
				int num_r;
				int num_c;
				int num_m;
				int num;

				ConfigReader(const std::string& filePath)
					: num(0), num_r(0), num_c(0), num_m(0), file(filePath) {
					if (!file.is_open()) {
						std::cout << "Failed to open file!" << std::endl;
					}
					else {
						std::string info;

						getline(file, info);
						num_c = stoi(info);
						getline(file, info);
						num_r = stoi(info);
						getline(file, info);
						num_m = stoi(info);
					}
				}



				int getNumR() const {
					return num_r;
				}

				int getNumC() const {
					return num_c;
				}

				int getNumM() const {
					return num_m;
				}
				int getNum() {
					return num;
				}
				void addNum()
				{
					num++;
				}
				void  subNum()
				{
					num--;
				}


			};


			class Board : public ConfigReader, public tile_attributes
			{

			public:
				std::vector<std::vector<tile_attributes>> board;
				std::mt19937 random_mt;
				std::uniform_int_distribution<int> how;
				std::uniform_int_distribution<int> dist;
				int space;
				int gen;
				int flagged = 0;
				const int row = getNumR();
				const int col = getNumC();
				const int mine = getNumM();
				int factor = round(getNumM() / getNumR());
				int same = 0;
				int revealedTiles = 0;
				int flaggedMines = 0;
				////
				int num;
				int surround_helper = 0;

				bool next_row = false;
				bool breakLoop = false;
				bool lose;


				// for recursion
				// Constructor with member initializer list



				Board() : ConfigReader("files/config.cfg"), random_mt(std::chrono::steady_clock::now().time_since_epoch().count()), how(1, 3), dist(0, 24), space(0), gen(0)
				{





					board = std::vector<std::vector<tile_attributes>>(row, std::vector<tile_attributes>(col));
					int numm = 0;
					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < board[i].size(); ++j) {
							board[i][j].x = (0 + (j * 32));
							board[i][j].y = 0 + (i * 32);
							board[i][j].tile_hidden.setTexture(board[i][j].Tile_hiddenTexture);
							board[i][j].tile_hidden.setPosition(board[i][j].x, board[i][j].y);


						}
					}
					cout << endl;

					//relook and understand
					while (num < mine) {

						for (int i = 0; i < row && num < mine; ++i) {


							gen = how(random_mt);

							if (num + gen >= mine) {
								gen = mine - num;  // Ensures the total number of mines doesn't exceed 50
							}

							num += gen;
							cout << num << endl;

							for (int j = 0; j < gen; ++j) {

								space = dist(random_mt);

								// Check if the space already has a mine
								if (!board[i][space].mine) {
									board[i][space].mine = true;
								}
								else {
									// If a mine is already present, reduce the count of generated mines
									j--;
								}
							}
						}
					}
					num = 0;
					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < col; ++j) {
							cout << board[i][j].mine;
							if (board[i][j].mine == true)
							{

								num++;
							}

							//cout << board[i][space].mine ;
						}
						cout << endl;
					}
					cout << "how many mines: " << num << endl;

				}

				void set_neighbor()
				{
					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < col; ++j) {
							int sX = std::max(0, i - 1);

							int ex = std::min(row - 1, i + 1);

							int sY = std::max(0, j - 1);

							int eY = std::min(col - 1, j + 1);

							for (int x = sX; x <= ex; ++x) {
								for (int y = sY; y <= eY; ++y) {
									if (x != i || y != j) {
										board[i][j].surround.push_back(&board[x][y]);
									}
								}
							}
						}
					}

				}
				void find() {
					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < col; ++j) {
							board[i][j].adjacent_mines = 0;
							for (tile_attributes* tile : board[i][j].surround) {
								if (tile->mine) {
									board[i][j].adjacent_mines++;
								}
							}
						}
					}

					// Display adjacent mines count
					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < col; ++j) {
							cout << board[i][j].adjacent_mines << " ";
						}
						cout << endl;

					}
					num = 0;
					breakLoop = false;
				}

				void Defeat()
				{
					for (int i = 0; i < row; ++i)
					{
						for (int j = 0; j < col; ++j)
						{
							if (board[i][j].mine == true && board[i][j].tile == false)
							{
								board[i][j].i_lose == true;
								lose = true;
							}
						}

					}
				}

				bool checkVictory() {
					

					for (int i = 0; i < row; ++i) {
						for (int j = 0; j < col; ++j) {
							if (board[i][j].tile == false && board[i][j].mine == false) {
								revealedTiles++;
							}
							if (board[i][j].flag == true && board[i][j].mine == true) {
								flaggedMines++;
							}
						}
					}

					int totalTiles = row * col;
					int totalMines = getNumM();

					return (totalTiles - totalMines == revealedTiles) && (totalMines == flaggedMines);
				}
			};
			struct window : public ConfigReader
			{
				sf::RenderWindow welcomeWindow;

				window() : ConfigReader("files/config.cfg"), welcomeWindow(sf::VideoMode(getNumC() * 32, (getNumR() * 32) + 100), "Welcome")
				{
				}
			};	////////////////////////

			struct window_2 : public ConfigReader
			{
				sf::RenderWindow Game_window;

				window_2() : ConfigReader("files/config.cfg"), Game_window(sf::VideoMode(getNumC() * 32, (getNumR() * 32) + 100), "Game Window")
				{
				}
			};

			//	bject with width 450 pixels, height 200 pixels
			//sf::RenderWindow welcomeWindow(sf::VideoMode(800, 600), "Welcome");








							// Update cursor position
			class TextEntry : public ConfigReader
			{
			private:
				sf::Font font;

			public:
				std::string userinput;

				sf::FloatRect welcomeTextRect;
				sf::RectangleShape cursor;

				sf::Text text;
				/*sf::RectangleShape cursor(sf::Vector2f(2.0f, static_cast<float>(text.getCharacterSize()));*/



				TextEntry(int characterSize) : ConfigReader("files/config.cfg")
				{

					if (!font.loadFromFile("files/font.ttf"))
					{
						std::cout << "can't load :(" << std::endl;
					}
					text.setFont(font);
					text.setCharacterSize(characterSize);
					//text.setStyle(sf::Text::Bold | sf::Text::Underlined);
					//text.setFillColor(sf::Color::White);




				}


				void setStyle(std::string bold, std::string underlined)
				{
					text.setStyle(sf::Text::Bold | sf::Text::Underlined);


				}
				void setStyle(std::string bold)
				{
					text.setStyle(sf::Text::Bold);
				}

				void setString(const std::string& str) {
					text.setString(str);
				}
				void setFillColor(char color)
				{
					if (color == 'w')
					{
						text.setFillColor(sf::Color::White);
					}
					else if (color == 'w')
					{
						text.setFillColor(sf::Color::White);
					}
					else if (color == 'y')
					{
						text.setFillColor(sf::Color::Yellow);
					}
					sf::FloatRect welcomeTextRect = text.getLocalBounds();
					text.setOrigin(welcomeTextRect.left + welcomeTextRect.width / 2.0f, welcomeTextRect.top + welcomeTextRect.height / 2.0f);
					text.setPosition(sf::Vector2f((getNumC() * 32) / 2.0f, ((getNumR() * 32) + 100 / 2.0f) - 150.0f));
				}

				void setOrigin()
				{
					sf::FloatRect welcomeTextRect = text.getLocalBounds();
					text.setOrigin(welcomeTextRect.left + welcomeTextRect.width / 2.0f, welcomeTextRect.top + welcomeTextRect.height / 2.0f);
				}

				void setPosition(float x, float y)
				{
					text.setPosition(sf::Vector2f(((getNumC() * 32) / 2.0f) - x, (((getNumR() * 32) + 100) / 2.0f) - y));
					//cursor.setPosition(sf::Vector2f(800 / 2.0f, (600 / 2.0f) - 45.0f));

				}
				void setCursor()
				{
					sf::RectangleShape cursor(sf::Vector2f(2.0f, static_cast<float>(text.getCharacterSize())));
					cursor.setPosition(sf::Vector2f(((getNumC() * 32) / 2.0f), (((getNumR() * 32) + 100) / 2.0f) - 45.0f));
				}





			};

			// Creating font object. SFML font objects will use .loadFromFile() function
			//sf::Font font;
			//if (!font.loadFromFile("C:\\Users\\LookO\\OneDrive\\.idea\\Projects\\THE BIG OL PROJECT\\font.ttf"))
			//{
			//	std::cout << "can't load :(" << std::endl;
			//	return 0;
			//}
			// Creating text object called welcomeText
			//sf::Text welcomeText("Press enter to begin", font, 16); // Only takes 3 rguments
			TextEntry welcomeText(24);
			//sf::Text welcomeText;
			welcomeText.setString("WELCOME TO MINESWEEPER!");
			//welcomeText.setPosition(sf::Vector2f(800 / 2.0f, (600 / 2.0f) - 150.0f));


			//welcomeText.setFont(font);
			//welcomeText.setCharacterSize(24);
			//welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
			welcomeText.setStyle("w", "w");
			welcomeText.setFillColor('w');
			welcomeText.setOrigin();
			welcomeText.setPosition(0.0f, 150.0f);
			//sf::FloatRect welcomeTextRect = welcomeText.getLocalBounds();
			// Origin is default the top left corner, so we change this to be the middle of the x and y axis
				//welcomeText.setOrigin(welcomeTextRect.left + welcomeTextRect.width / 2.0f,welcomeTextRect.top + welcomeTextRect.height / 2.0f);
			//Creating text object called Enter name 
			TextEntry NameText(20);
			NameText.setStyle("Bold");

			//sf::Text NameText;
			NameText.setString("Enter your name:");
			//NameText.setFont(font);
			//NameText.setCharacterSize(20);
			NameText.setFillColor('w');
			NameText.setOrigin();
			NameText.setPosition(0, 75.0f);


			//sf::FloatRect NameTextRect = NameText.getLocalBounds();
			// Origin is default the top left corner, so we change this to be the middle of the x and y axis
			//NameText.setOrigin(NameTextRect.left + NameTextRect.width / 2.0f, NameTextRect.top + NameTextRect.height / 2.0f);
			//NameText.setPosition(sf::Vector2f(800 / 2.0f, (600 / 2.0f) - 75.0f));

		//Creating text object called Enter name 
			TextEntry endinput(18);
			//sf::Text endinput;
			//endinput.setFont(font);
			//endinput.setCharacterSize(18);
			endinput.setStyle("Bold");
			endinput.setFillColor('y');
			endinput.setOrigin();
			// Origin is default the top left corner, so we change this to be the middle of the x and y axis
			endinput.setPosition(0.0f, 45.0f);
			//////////////////////next window 
			cout << "3";

			std::string userinput;
			/////////fix this before submitting milestone 333333333333333333333333333333333333333333333333333333333333333333333333333333
			window win;
			win.welcomeWindow;
			endinput.cursor.setSize(sf::Vector2f(2.0f, static_cast<float>(endinput.text.getCharacterSize())));


			Board* game = new Board();
			game->set_neighbor();
			game->find();


			endinput.cursor.setPosition(sf::Vector2f((game->getNumC() * 32) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f));//trying to fix the cursor and the stringinput to correlate to the TextEntry class 



			sf::RectangleShape cursor(sf::Vector2f(2.0f, static_cast<float>(endinput.text.getCharacterSize())));
			cursor.setPosition(sf::Vector2f(((game->getNumC() * 32) / 2.0f), (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f));
			//system("cls");
			// Everything that is updated has to occur in the while loop
			while (win.welcomeWindow.isOpen()) { // while the welcome window is active, we do everything below here

				sf::Mouse;
				sf::Event event;
				while (win.welcomeWindow.pollEvent(event)) {
					if (event.type == sf::Event::Closed) // Click X on the window
					{
						win.welcomeWindow.close();
						return 0;
					}


					else if (event.type == sf::Event::TextEntered)
					{
						if (game->getNum() == 0)
						{
							if ((event.text.unicode > 96 && event.text.unicode < 123))
							{
								game->addNum();
								userinput += static_cast<char>(std::toupper((event.text.unicode)));
								endinput.text.setString(userinput);
								sf::FloatRect textRect = endinput.text.getLocalBounds();
								float xOffset = (800 - textRect.width) / 2.0f;
								endinput.text.setPosition(((game->getNumC() * 32) - textRect.width) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f);

								endinput.cursor.setPosition(endinput.text.getPosition().x + textRect.width, endinput.text.getPosition().y);


							}

						}
						else if (game->getNum() < 10)
						{//look at the difference between this and your old code 
							if ((event.text.unicode > 96 && event.text.unicode < 123)) {
								game->addNum();
								userinput += static_cast<char>(event.text.unicode);
								endinput.text.setString(userinput);
								// Update text position
								sf::FloatRect textRect = endinput.text.getLocalBounds();
								float xOffset = (800 - textRect.width) / 2.0f;
								endinput.text.setPosition(((game->getNumC() * 32) - textRect.width) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f);

								// Update cursor position
								endinput.cursor.setPosition(((game->getNumC() * 32) + textRect.width) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f);
							}

						}

						if (game->getNum() > 0)
						{
							if (event.text.unicode == 8)
							{
								game->subNum();
								userinput.pop_back();
								endinput.text.setString(userinput);
								sf::FloatRect textRect = endinput.text.getLocalBounds();
								float xOffset = (800 - textRect.width) / 2.0f;
								endinput.text.setPosition(((game->getNumC() * 32) - textRect.width) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f);

								// Update cursor position
								endinput.cursor.setPosition(((game->getNumC() * 32) + textRect.width) / 2.0f, (((game->getNumR() * 32) + 100) / 2.0f) - 45.0f);




							}
						}


					}
					if (event.type == sf::Event::KeyPressed)
					{
						if (event.key.code == sf::Keyboard::Enter)
						{
							if (game->getNum() > 0) {
								win.welcomeWindow.close();




							}
						}
					}
				}
				// Most code will go above this line for this window
				win.welcomeWindow.clear(sf::Color::Blue); // Set background color of the window
				win.welcomeWindow.draw(welcomeText.text); // have to draw each object for this specific window
				win.welcomeWindow.draw(NameText.text);
				win.welcomeWindow.draw(endinput.cursor);
				win.welcomeWindow.draw(endinput.text);
				win.welcomeWindow.display(); // Final
			}









			class GameSprites : public Board
			{

				///private:
				//	sf::Texture& getTexture(const std::string& textureName) {
						// Implement your TextureManager::getTexture logic here
						// Return the appropriate texture based on the textureName
						// Example: return TextureManager::getTexture(textureName);
			//		}

			public:
				sf::Sprite tile_revealedSprite;
				sf::Sprite face_happy;
				sf::Sprite face_lose;
				sf::Sprite face_win;

				sf::Sprite debug;
				sf::Sprite pause;
				sf::Sprite play;
				sf::Sprite leaderboard;
				sf::Sprite digits;
				sf::Sprite flag;
				sf::Sprite mine;

				GameSprites() : Board()

				{
					sf::Texture& Tile_revealedTexture = TextureManager::getTexture("tile_revealed");
					tile_revealedSprite.setTexture(Tile_revealedTexture);
					tile_revealedSprite.setPosition(0, 0);


					sf::Texture& face_happyTexture = TextureManager::getTexture("face_happy");
					face_happy.setTexture(face_happyTexture);
					face_happy.setPosition(((getNumC() / 2.0f) * 32) - 32, 32 * (getNumR() + 0.5f));

					sf::Texture& face_winTexture = TextureManager::getTexture("face_win");
					face_win.setTexture(face_winTexture);
					face_win.setPosition(((getNumC() / 2.0f) * 32) - 32, 32 * (getNumR() + 0.5f));

					sf::Texture& face_loseTexture = TextureManager::getTexture("face_lose");
					face_lose.setTexture(face_loseTexture);
					face_lose.setPosition(((getNumC() / 2.0f) * 32) - 32, 32 * (getNumR() + 0.5f));

					sf::Texture& playTexture = TextureManager::getTexture("play");
					play.setTexture(playTexture);
					play.setPosition((getNumC() * 32) - 240, 32 * (getNumR() + 0.5));

					sf::Texture& pauseTexture = TextureManager::getTexture("pause");
					pause.setTexture(pauseTexture);
					pause.setPosition((getNumC() * 32) - 240, 32 * (getNumR() + 0.5));

					sf::Texture& leaderboardTexture = TextureManager::getTexture("leaderboard");
					leaderboard.setTexture(leaderboardTexture);
					leaderboard.setPosition((getNumC() * 32) - 176, 32 * (getNumR() + 0.5));

					sf::Texture& digitsTexture = TextureManager::getTexture("digits");
					digits.setTexture(digitsTexture);
					digits.setPosition((getNumC() * 32) - 97, 32 * (getNumR() + 0.5) + 16);

					sf::Texture& flagTexture = TextureManager::getTexture("flag");
					flag.setTexture(flagTexture);
					flag.setOrigin(0, 0);

					sf::Texture& debugTexture = TextureManager::getTexture("debug");
					debug.setTexture(debugTexture);
					debug.setPosition((getNumC() * 32) - 304, 32 * (getNumR() + 0.5));

					sf::Texture& mineTexture = TextureManager::getTexture("mine");
					mine.setTexture(mineTexture);



				}


			};
			/////////////////
			GameSprites visual;
			window_2 next;
			next.Game_window;
			//////////////////////// Board class is made





			//////////////////////////////////



			int minutes0 = 0;
			int minutes1 = 0;
			int seconds0 = 0;
			int seconds1 = 0;
			auto start_time = chrono::high_resolution_clock::now();//starting the timer when the program begins to run!
			//In project 3, this(^^^) would go above the Game Window while loops, but below the Welcome Window while loops

			//Below is just for initialization, in the end, this will be overwritten, just needed to equal something. TLDR: Ignore what it equals here (below).
			auto pauseTime = chrono::high_resolution_clock::now();
			auto elapsed_paused_time = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - pauseTime).count();

			bool paused = false; //false when game in not paused, true when the game is paused


			while (next.Game_window.isOpen())
			{
				sf::Vector2i mouse;

				sf::Event event;
				auto currentTime = chrono::high_resolution_clock::now();
				auto gameTimeElapsed = chrono::duration_cast<chrono::seconds>(currentTime - start_time).count();

				int minutes = gameTimeElapsed / 60;
				int seconds = gameTimeElapsed % 60;
				while (next.Game_window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed) // Click X on the window
					{
						next.Game_window.close();
					}

					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && (game->lose == false)) {
						mouse = sf::Mouse::getPosition(next.Game_window);
						for (int i = 0; i < game->getNumR(); ++i) {
							for (int j = 0; j < game->getNumC(); ++j) {
								if ((mouse.x - game->board[i][j].x < 32 && mouse.x - game->board[i][j].x > 0) && (mouse.y - game->board[i][j].y < 32 && mouse.y - game->board[i][j].y > 0) && (game->board[i][j].flag == false)) {
									game->board[i][j].tile = false;
									game->board[i][j].tile_hidden = sf::Sprite();

									if (game->board[i][j].mine != true)
									{
										game->board[i][j].minesweep(game->board[i][j]);

									}
									cout << game->board[i][j].num_here;

								}
							}
						}
					}

					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && (game->lose == false)) {
						mouse = sf::Mouse::getPosition(next.Game_window);
						for (int i = 0; i < game->getNumR(); ++i) {
							for (int j = 0; j < game->getNumC(); ++j) {
								if ((mouse.x - game->board[i][j].x < 32 && mouse.x - game->board[i][j].x > 0) && (mouse.y - game->board[i][j].y < 32 && mouse.y - game->board[i][j].y > 0) && game->board[i][j].flag == false) {
									game->board[i][j].flag = true;

								}
								else if ((mouse.x - game->board[i][j].x < 32 && mouse.x - game->board[i][j].x > 0) && (mouse.y - game->board[i][j].y < 32 && mouse.y - game->board[i][j].y > 0) && game->board[i][j].flag == true) {
									game->board[i][j].flag = false;

								}
							}
						}
					}

					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						if (visual.pause.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
							paused = !paused; // Toggle the paused state

							if (paused) {
								cout << "Paused" << endl;
								pauseTime = chrono::high_resolution_clock::now();
							}
							else {
								auto unPausedTime = chrono::high_resolution_clock::now();
								elapsed_paused_time += chrono::duration_cast<chrono::seconds>(unPausedTime - pauseTime).count();
							}
						}
					}
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						if (visual.debug.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {

							for (int r = 0; r < game->getNumR(); ++r) {

								for (int c = 0; c < game->getNumC(); ++c)
								{
									sf::Sprite ne = game->board[r][c].tile_hidden;
									if (game->board[r][c].mine == true)
									{
										game->board[r][c].tile_hidden = visual.tile_revealedSprite;
									}
									
									else
									{
										game->board[r][c].tile_hidden = ne;

									}


								}
							}
						}
					}
				
					


					
				}
				////////////////
				
					for (int r = 0; r < game->getNumR(); ++r)
					{

						for (int c = 0; c < game->getNumC(); ++c) {
							if (game->board[r][c].number.getTexture() != nullptr)
							{
								game->board[r][c].number.setPosition(game->board[r][c].x, game->board[r][c].y);
							}
						}
					}



					//Game_window.clear(sf::Color(192, 192, 192));
					next.Game_window.clear(sf::Color::White);


					for (int r = 0; r < game->getNumR(); ++r) {

						for (int c = 0; c < game->getNumC(); ++c) {

							next.Game_window.draw(visual.tile_revealedSprite);
							next.Game_window.draw(game->board[r][c].number);
							visual.tile_revealedSprite.setPosition(0 + (c * 32), 0 + (r * 32));

						}
					}
					for (int r = 0; r < game->getNumR(); ++r) {

						for (int c = 0; c < game->getNumC(); ++c) {
							if (game->board[r][c].mine == true)
							{
								visual.mine.setPosition(game->board[r][c].x, game->board[r][c].y);
								next.Game_window.draw(visual.mine);


							}

						}
					}
					game->addNum();
					if (game->getNum() > 0)//draws it once
					{
						for (int r = 0; r < game->getNumR(); ++r) {

							for (int c = 0; c < game->getNumC(); ++c) {
								next.Game_window.draw(game->board[r][c].tile_hidden);


							}
						}
						game->addNum();
					}

					for (int r = 0; r < game->getNumR(); ++r) {

						for (int c = 0; c < game->getNumC(); ++c) {


							if (game->board[r][c].num_here == true)
							{
								game->board[r][c].num = TextureManager::getTexture(game->board[r][c].texture_name + to_string(game->board[r][c].adjacent_mines));
								game->board[r][c].number.setTexture(game->board[r][c].num);
								game->board[r][c].number.setPosition(game->board[r][c].x, game->board[r][c].y);
								next.Game_window.draw(game->board[r][c].number);

							}

						}
					}
					for (int r = 0; r < game->getNumR(); ++r) {//checks to see if it should draw a flag

						for (int c = 0; c < game->getNumC(); ++c) {
							if (game->board[r][c].tile == true && game->board[r][c].flag == true)
							{
								visual.flag.setPosition(game->board[r][c].x, game->board[r][c].y);

								next.Game_window.draw(visual.flag);

							}

						}
					}
					//counter code starts here 
					for (int r = 0; r < game->getNumR(); ++r) {

						for (int c = 0; c < game->getNumC(); ++c) {
							if (game->board[r][c].tile == true && game->board[r][c].flag == true && game->board[r][c].mine == true)
							{

								if (game->board[r][c].stop_counting_flag == false)
								{
									game->flagged++;
									game->board[r][c].stop_counting_flag = true;




								}

							}

						}
					}

						// Update the previous mouse position
					map<int, sf::Sprite> digitsMap = parseDigits(visual.digits);

					//this finds the time elapsed, so the current time - the time the window opened.
					auto game_duration = std::chrono::duration_cast<std::chrono::seconds>(chrono::high_resolution_clock::now() - start_time);
					int total_time = game_duration.count(); // necessary to subtract elapsed time later because "game_duration.count()" is const

					

					if (!paused) {
						//enters if the game is NOT paused. This is the condition that keeps the timer from incrementing when paused.
						//cout << "not paused\n";
						total_time = total_time - elapsed_paused_time; //
						minutes = total_time / 60;
						seconds = total_time % 60;
					}
					

					if (paused != true )
					{					
					 minutes0 = minutes / 10 % 10; //minutes index 0
					 minutes1 = minutes % 10; // minutes index 1
					 seconds0 = seconds / 10 % 10; // seconds index 0
					 seconds1 = seconds % 10; // seconds index 1
					}
				
					int counter1;
					int counter10;
					int counter100;

					 
					

						 digitsMap[minutes0].setPosition((game->getNumC() * 32) - 97, 32 * (game->getNumR() + 0.5) + 16);
						 next.Game_window.draw(digitsMap[minutes0]);

						 digitsMap[minutes1].setPosition((game->getNumC() * 32) - 76, 32 * (game->getNumR() + 0.5) + 16);
						 next.Game_window.draw(digitsMap[minutes1]);

						 digitsMap[seconds0].setPosition((game->getNumC() * 32) - 55, 32 * (game->getNumR() + 0.5) + 16);
						 next.Game_window.draw(digitsMap[seconds0]);

						 digitsMap[seconds1].setPosition((game->getNumC() * 32) - 34, 32 * (game->getNumR() + 0.5) + 16);
						 next.Game_window.draw(digitsMap[seconds1]);
					 
				
					if ((game->flagged / 10.0f) < 1.0f)
					{
						counter1 = game->flagged;

					}
					else if ((game->flagged / 10.f) >= 1.0f)
					{
						counter10 = game->flagged / 10;
						counter1 = 0;

					}
					else if ((game->flagged / 10.0f) >= 10.0f)
					{

						counter100 = game->flagged / 100;
						counter10 = 0;
						counter1 = 0;

					}


					if (paused != true)
					{
						
					}
						
					
					


					int displayCounter1 = counter1 % 10;
					int displayCounter10 = counter10 % 10;
					int displayCounter100 = counter100 % 10;

					// Draw individual digits at appropriate positions
					// (Assuming digitsMap[] holds the graphical representation of numbers)
					digitsMap[displayCounter100].setPosition(33, 32 * (game->getNumR() + 0.5) + 16);
					next.Game_window.draw(digitsMap[displayCounter100]);

					digitsMap[displayCounter10].setPosition(54, 32 * (game->getNumR() + 0.5) + 16);
					next.Game_window.draw(digitsMap[displayCounter10]);

					digitsMap[displayCounter1].setPosition(75, 32 * (game->getNumR() + 0.5) + 16);
					next.Game_window.draw(digitsMap[displayCounter1]);




					





					game->Defeat();
					game->checkVictory();

					for (int r = 0; r < game->getNumR(); ++r) {

						for (int c = 0; c < game->getNumC(); ++c)
						{
							if (game->lose == true)
							{
								paused = true;
								next.Game_window.draw(visual.face_lose);
								if (game->board[r][c].mine == true)
								{
									game->board[r][c].tile = false;
									game->board[r][c].tile_hidden = sf::Sprite();
								}


							}



							else if (game->lose == false)
							{
								next.Game_window.draw(visual.face_happy);

							}

						}
					}

					if (game->checkVictory() == true)
					{
						next.Game_window.draw(visual.face_win);


					}
					

					next.Game_window.draw(visual.debug);
					if (paused) {

						next.Game_window.draw(visual.play);
					}
					else
					{
						next.Game_window.draw(visual.pause);
					}


					next.Game_window.draw(visual.leaderboard);

					next.Game_window.display();





			}



			/*

	*/
			return 0;
		}




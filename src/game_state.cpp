#include "game_state.h"
#include "game_over_state.h"

GameState::GameState(GameDataRef data) : _data(data) {

}

void GameState::Init() {
    gameState = STATE_PLAYING;
    turn = PLAYER_PIECE;
    this->computerLogic = new ComputerLogic(turn, this->_data);
    this->_data->assets.LoadTexture("Pause Button", PAUSE_BUTTON_IMAGEPATH);
    this->_data->assets.LoadTexture("Game Background", GAME_BACKGROUND_IMAGEPATH);
    this->_data->assets.LoadTexture("Grid Sprite", GRID_SPRITE_IMAGEPATH);
    this->_data->assets.LoadTexture("X Piece", X_PIECE_IMAGEPATH);
    this->_data->assets.LoadTexture("O Piece", O_PIECE_IMAGEPATH);
    this->_data->assets.LoadTexture("X Piece Winning", X_WINNING_PIECE_IMAGEPATH);
    this->_data->assets.LoadTexture("O Piece Winning", O_WINNING_PIECE_IMAGEPATH);

    _background.setTexture(this->_data->assets.GetTexture("Game Background"));
    _pauseButton.setTexture(this->_data->assets.GetTexture("Pause Button"));
    _gridSprite.setTexture(this->_data->assets.GetTexture("Grid Sprite"));
    _pauseButton.setPosition(this->_data->window.getSize().x - _pauseButton.getLocalBounds().width, _pauseButton.getPosition().y);
    _gridSprite.setPosition((SCREEN_WIDTH / 2) - (_gridSprite.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 2) - (_gridSprite.getGlobalBounds().height / 2));
    InitGridPieces();

    for (int x =0; x< 3; x++) {
        for (int y = 0; y < 3; y++)
        {
            gridArray[x][y] = EMPTY_PIECE;
        }
    }

}

void GameState::HandleInput() {
    sf::Event event;
    while (this->_data->window.pollEvent((event))) {
        if (sf::Event::Closed == event.type) {
            this->_data->window.close();
        }
        if (this->_data->input.IsSpriteSelected(this->_pauseButton, sf::Mouse::Left, this ->_data->window)) {
            this->_data->machine.AddState(StateRef(new PauseState(_data)), false);
        }
        if (this->_data->input.IsSpriteSelected(this->_gridSprite, sf::Mouse::Left, this ->_data->window)) {
            if (STATE_PLAYING == gameState) {
                this->PlacePiece();
            }
        }
    }
}

void GameState::Update(float deltaTime) {
    if (gameState == STATE_WON || gameState == STATE_LOST || gameState == STATE_WON ) {
        if (this->_clock.getElapsedTime().asSeconds() > SECONDS_BEFORE_SHOWING_GAME_OVER) {
            this->_data->machine.AddState(StateRef(new GameOverState(_data)), true);
        }
    }
}

void GameState::Draw(float deltaTime) {
    this->_data->window.clear();
    this->_data->window.draw(this->_background);
    this->_data->window.draw(this->_pauseButton);
    this->_data->window.draw(this->_gridSprite);
    for (int x = 0; x <3; x++) {
        for (int y = 0; y <3 ; y++) {
            this->_data->window.draw(this->_gridPieces[x][y]);
        }
    }
    this->_data->window.display();
}

void GameState::InitGridPieces() {
    sf::Vector2u tempSpriteSize = this->_data->assets.GetTexture("X Piece").getSize();

    for (int x = 0; x <3; x++) {
        for (int y = 0; y <3 ; y++) {
            _gridPieces[x][y].setTexture(this->_data->assets.GetTexture("X Piece"));
            _gridPieces[x][y].setPosition(_gridSprite.getPosition().x + (tempSpriteSize.x * x) - 7,
                                          _gridSprite.getPosition().y + (tempSpriteSize.y * y) -7);
            _gridPieces[x][y].setColor(sf::Color(255, 255, 255, 0));
        }
    }
}

void GameState::PlacePiece() {
    sf::Vector2i touchPoint = this->_data->input.GetMousePosition(this->_data->window);
    sf::FloatRect gridSize = _gridSprite.getGlobalBounds();
    sf::Vector2f outsideGridGap = sf::Vector2f((SCREEN_WIDTH - gridSize.width) / 2, (SCREEN_HEIGHT - gridSize.height) / 2);
    sf::Vector2f gridLocalTouchPos = sf::Vector2f(touchPoint.x - outsideGridGap.x, touchPoint.y - outsideGridGap.y);
    sf::Vector2f gridSectionSize = sf::Vector2f(gridSize.width / 3, gridSize.height / 3);
    int column, row;
    if (gridLocalTouchPos.x < gridSectionSize.x ) {
        column = 1;
    }
    else if (gridLocalTouchPos.x < gridSectionSize.x * 2) {
        column = 2;
    }
    else if (gridLocalTouchPos.x < gridSize.width) {
        column = 3;
    }
    if (gridLocalTouchPos.y < gridSectionSize.y ) {
        row = 1;
    }
    else if (gridLocalTouchPos.y < gridSectionSize.y * 2) {
        row = 2;
    }
    else if (gridLocalTouchPos.y < gridSize.height) {
        row = 3;
    }
    if (gridArray[column - 1][row - 1] == EMPTY_PIECE) {
        gridArray[column - 1][row - 1] = turn;
        if (PLAYER_PIECE == turn)
        {
            _gridPieces[column -1][row - 1].setTexture(this->_data->assets.GetTexture("X Piece"));
            this->CheckPlayerHasWon(turn);
        }

        _gridPieces[column - 1][row - 1].setColor(sf::Color(255, 255, 255, 255));
    }
}

void GameState::CheckPlayerHasWon(int turn) {
    CheckForMatch(0, 0, 1, 0, 2, 0, turn);
    CheckForMatch(0, 1, 1, 1, 2, 1, turn);
    CheckForMatch(0, 2, 1, 2, 2, 2, turn);
    CheckForMatch(0, 0, 0, 1, 0, 2, turn);
    CheckForMatch(1, 0, 1, 1, 1, 2, turn);
    CheckForMatch(2, 0, 2, 1, 2, 2, turn);
    CheckForMatch(0, 0, 1, 1, 2, 2, turn);
    CheckForMatch(0, 2, 1, 1, 2, 0, turn);

    if (gameState != STATE_WON) {
        gameState = STATE_COMPUTER_PLAYING;
        computerLogic->PlacePiece(&gridArray, _gridPieces, &gameState);
        CheckForMatch(0, 0, 1, 0, 2, 0, COMPUTER_PIECE);
        CheckForMatch(0, 1, 1, 1, 2, 1, COMPUTER_PIECE);
        CheckForMatch(0, 2, 1, 2, 2, 2, COMPUTER_PIECE);
        CheckForMatch(0, 0, 0, 1, 0, 2, COMPUTER_PIECE);
        CheckForMatch(1, 0, 1, 1, 1, 2, COMPUTER_PIECE);
        CheckForMatch(2, 0, 2, 1, 2, 2, COMPUTER_PIECE);
        CheckForMatch(0, 0, 1, 1, 2, 2, COMPUTER_PIECE);
        CheckForMatch(0, 2, 1, 1, 2, 0, COMPUTER_PIECE);
    }
    int emptyNum = 9;

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (EMPTY_PIECE != gridArray[x][y]) {
                emptyNum--;
            }
        }
    }

    if (emptyNum == 0 && (gameState != STATE_WON) && (gameState != STATE_LOST)) {
        gameState = STATE_DRAW;
    }

    if (gameState == STATE_DRAW || gameState == STATE_LOST || gameState == STATE_WON) {
        this->_clock.restart();
    }

    std::cout << gameState << std::endl;
}

void GameState::CheckForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck) {
    if (pieceToCheck == gridArray[x1][y1] && pieceToCheck == gridArray[x2][y2] && pieceToCheck == gridArray[x3][y3]) {
        std::string winningPieceStr;

        if (pieceToCheck == O_PIECE) {
            winningPieceStr = "O Piece Winning";
        }
        if (pieceToCheck == X_PIECE) {
            winningPieceStr = "X Piece Winning";
        }
        _gridPieces[x1][y1].setTexture(this->_data->assets.GetTexture(winningPieceStr));
        _gridPieces[x2][y2].setTexture(this->_data->assets.GetTexture(winningPieceStr));
        _gridPieces[x3][y3].setTexture(this->_data->assets.GetTexture(winningPieceStr));

        if (pieceToCheck == PLAYER_PIECE) {
            gameState = STATE_WON;
        }
        else {
            gameState = STATE_LOST;
        }

    }
}


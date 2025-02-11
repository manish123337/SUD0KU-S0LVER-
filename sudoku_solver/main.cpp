#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

bool isSafe(const vector<vector<int>>& grid, int N, int row, int col, int num) {
    //Check in Row
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num)
            return false;

    //Check in Coloumn
    for (int x = 0; x < N; x++)
        if (grid[x][col] == num)
            return false;

    //Block check (generalized for square blocks if N is a perfect square)
    if (sqrt(N) == floor(sqrt(N))) {
        int blockSize = sqrt(N);
        int startRow = row - row % blockSize;
        int startCol = col - col % blockSize;
        for (int i = 0; i < blockSize; i++)
            for (int j = 0; j < blockSize; j++)
                if (grid[i + startRow][j + startCol] == num)
                    return false;
    } else if (N == 9) { //Special case for 9x9 to keep 3x3 blocks
        int startRow = row - row % 3;
        int startCol = col - col % 3;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (grid[i + startRow][j + startCol] == num)
                    return false;
    }

    return true;
}

bool solveSudoku(vector<vector<int>>& grid, int N, int row, int col) {
    if (row == N - 1 && col == N)
        return true;

    if (col == N) {
        row++;
        col = 0;
    }

    if (grid[row][col] > 0)
        return solveSudoku(grid, N, row, col + 1);

    for (int num = 1; num <= N; num++) {
        if (isSafe(grid, N, row, col, num)) {
            grid[row][col] = num;
            if (solveSudoku(grid, N, row, col + 1))
                return true;
        }
        grid[row][col] = 0;
    }
    return false;
}

bool isValidSudoku(const vector<vector<int>>& grid, int N) {
    for (int row = 0; row < N; row++) {
        vector<int> row_values;
        for (int col = 0; col < N; col++) {
            if (grid[row][col] != 0) {
                for (int val : row_values) {
                    if (val == grid[row][col]) return false; // Duplicate in row
                }
                row_values.push_back(grid[row][col]);
            }
        }
    }

    for (int col = 0; col < N; col++) {
        vector<int> col_values;
        for (int row = 0; row < N; row++) {
            if (grid[row][col] != 0) {
                for (int val : col_values) {
                    if (val == grid[row][col]) return false; // Duplicate in column
                }
                col_values.push_back(grid[row][col]);
            }
        }
    }

    if (sqrt(N) == floor(sqrt(N))) {
        int blockSize = sqrt(N);
        for (int block_start_row = 0; block_start_row < N; block_start_row += blockSize) {
            for (int block_start_col = 0; block_start_col < N; block_start_col += blockSize) {
                vector<int> block_values;
                for (int row = 0; row < blockSize; row++) {
                    for (int col = 0; col < blockSize; col++) {
                        int current_val = grid[block_start_row + row][block_start_col + col];
                        if (current_val != 0) {
                            for (int val : block_values) {
                                if (val == current_val) return false; // Duplicate in block
                            }
                            block_values.push_back(current_val);
                        }
                    }
                }
            }
        }
    } else if (N == 9) { // 3x3 block check for 9x9
        for (int block_start_row = 0; block_start_row < N; block_start_row += 3) {
            for (int block_start_col = 0; block_start_col < N; block_start_col += 3) {
                vector<int> block_values;
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 3; col++) {
                        int current_val = grid[block_start_row + row][block_start_col + col];
                        if (current_val != 0) {
                            for (int val : block_values) {
                                if (val == current_val) return false; // Duplicate in 3x3 block
                            }
                            block_values.push_back(current_val);
                        }
                    }
                }
            }
        }
    }
    return true; // No immediate violations found
}


int main() {
    SDL_SetMainReady(); //Needed for SDL_MAIN_HANDLED

    int N = 0;
    while (N <= 0) {
        cout << "Enter Sudoku grid size (e.g., 4, 9, 16, ...): ";
        cin >> N;
        if (N <= 0) {
            cout << "Grid size must be a positive integer. Please try again." << endl;
        }
    }


    //For Screen
    int screenWidth = 1000;
    int screenHeight = 800; // Increased height to accommodate messages better

    //To store suduko data - using vector of vectors now
    vector<vector<int>> arr(N, vector<int>(N, 0)); // Initialize N x N vector with 0s

    //mouse coordinates
    int mx;
    int my;

    //mouse left pressed flag
    int mlp = 0;


    //To check if the number is entered or not
    int gotnum = 0;
    int numk = 0;   //Number entered from keyboard

    //coordinates of the chosen box
    int stpx = 0;
    int stpy = 0;

    //Coordinate of the text display box (not really used in SDL2 as in RayGui)
    int codrx = 0;
    int codry = 0;

    //Check the status of button
    int generated = 0;
    int generateAnswer = 0;
    int enterQuestion = 0;
    int done = 0;
    int clear = 0;


    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Sudoku Solver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_TTF
    if (TTF_Init() == -1) {
        SDL_Log("SDL_TTF Error: %s", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("fonts/arial.ttf", 20); // Increased font size
    if (font == nullptr) {
        SDL_Log("SDL_TTF Error: %s", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    SDL_Event event;
    bool quit = false;

    //Starting of suduko table
    int recx = (screenWidth - N * 50) / 2; // Center the grid horizontally
    int recy = 150;
    int rech = 50; // Increased cell height
    int recw = 50; // Increased cell width

    //coordinates of selected box
    int codx = 0;
    int cody = 0;

    int dxy = 0;

    // --- Color Palette ---
    SDL_Color backgroundColor = {245, 245, 245, 255};       // Light gray background
    SDL_Color gridLineColor = {100, 150, 255, 255};         // Light Blue grid lines
    SDL_Color blockLineColor = {100, 50, 150, 255};      // Dark Purple for block lines
    SDL_Color selectedCellColor = {255, 255, 200, 150};    // Light yellow selected cell
    SDL_Color textColor = {50, 50, 50, 255};             // Dark gray text color
    SDL_Color buttonColor = {220, 220, 220, 255};          // Light gray button
    SDL_Color buttonHoverColor = {200, 200, 200, 255};      // Slightly darker button hover (not used in this code for hover effect)
    SDL_Color buttonTextColor = {50, 50, 50, 255};         // Dark gray button text
    SDL_Color titleColor = {0, 0, 128, 255};              // Navy blue title
    SDL_Color messageColor = {80, 80, 80, 255};            // Darker gray message text
    SDL_Color errorColor = {255, 0, 0, 255};              // Red error message

    int buttonWidth = 140; // Adjusted button width
    int buttonHeight = 45; // Adjusted button height
    int buttonSpacing = 20; // Spacing between buttons (increased for better separation)

    int buttonsYPosition = 680; // Adjusted Y position for buttons

    // Calculate starting X position to center the group of 3 buttons
    float totalButtonsWidth = 3 * buttonWidth + 2 * buttonSpacing;
    int startButtonsX = (screenWidth - totalButtonsWidth) / 2;


    SDL_Rect enterQuestionButtonRect = { startButtonsX, buttonsYPosition, buttonWidth, buttonHeight };
    SDL_Rect doneButtonRect =         { startButtonsX + buttonWidth + buttonSpacing, buttonsYPosition, buttonWidth, buttonHeight };
    SDL_Rect clearButtonRect =        { startButtonsX + 2 * buttonWidth + 2 * buttonSpacing, buttonsYPosition, buttonWidth, buttonHeight };
    SDL_Rect generateAnswerButtonRect = { (screenWidth - 300) / 2, buttonsYPosition + buttonHeight + buttonSpacing, 300, buttonHeight }; // Wider Generate Answer button, below others


    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mx = event.button.x;
                    my = event.button.y;
                    mlp = 1;

                    // Button clicks detection:
                    if (mx > enterQuestionButtonRect.x && mx < (enterQuestionButtonRect.x + enterQuestionButtonRect.w) &&
                        my > enterQuestionButtonRect.y && my < (enterQuestionButtonRect.y + enterQuestionButtonRect.h)) {
                        enterQuestion = 1;
                        generateAnswer = 0;
                        generated = 0;
                    }
                    if (mx > generateAnswerButtonRect.x && mx < (generateAnswerButtonRect.x + generateAnswerButtonRect.w) &&
                        my > generateAnswerButtonRect.y && my < (generateAnswerButtonRect.y + generateAnswerButtonRect.h)) {
                        generateAnswer = 1;
                        done = 1;
                        enterQuestion = 0;
                    }
                    if (mx > doneButtonRect.x && mx < (doneButtonRect.x + doneButtonRect.w) &&
                        my > doneButtonRect.y && my < (doneButtonRect.y + doneButtonRect.h)) {
                        done = 1;
                        enterQuestion = 0;
                    }
                    if (mx > clearButtonRect.x && mx < (clearButtonRect.x + clearButtonRect.w) &&
                        my > clearButtonRect.y && my < (clearButtonRect.y + clearButtonRect.h)) {
                        clear = 1;
                        generateAnswer = 0;
                        for (int i = 0; i < N; ++i) { // Use N here
                            for (int j = 0; j < N; ++j) { // Use N here
                                arr[i][j] = 0;
                            }
                        }
                        enterQuestion = 0;
                        generated = 0;
                    }

                    // Grid cell click detection
                    if (enterQuestion == 1) {
                        if (mx > recx && mx < (recx + N * recw) && my > recy && my < (recy + N * rech)) { // Use N here
                            codx = ((mx - recx) / recw) + 1;
                            cody = ((my - recy) / rech) + 1;
                            stpx = recx + (codx - 1) * recw;
                            stpy = recy + (cody - 1) * rech;
                            codrx = codx;
                            codry = cody;
                            dxy = 1;
                        } else {
                            dxy = 0;
                            codx = 0;
                            cody = 0;
                            stpx = 0; stpy = 0;
                        }
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mlp = 0;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (codx > 0 && cody > 0 && enterQuestion == 1) {
                    switch (event.key.keysym.sym) {
                    case SDLK_1: numk = 1; gotnum = 1; break;
                    case SDLK_2: numk = 2; gotnum = 1; break;
                    case SDLK_3: numk = 3; gotnum = 1; break;
                    case SDLK_4: numk = 4; gotnum = 1; break;
                    case SDLK_5: numk = 5; gotnum = 1; break;
                    case SDLK_6: numk = 6; gotnum = 1; break;
                    case SDLK_7: numk = 7; gotnum = 1; break;
                    case SDLK_8: numk = 8; gotnum = 1; break;
                    case SDLK_9: numk = 9; gotnum = 1; break;
                    case SDLK_BACKSPACE: numk = 0; gotnum = 1; break; // Backspace clears cell
                    }
                    if (gotnum == 1 && numk <= N) { // Ensure entered number is within 1 to N range
                        arr[codx - 1][cody - 1] = numk;
                        gotnum = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        // SUDOKU SOLVER Title
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "SUDOKU SOLVER BY MANISH SHRESTHA", titleColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect;
        titleRect.x = (screenWidth - titleSurface->w) / 2; // Center title
        titleRect.y = 40; // Adjusted title position
        SDL_QueryTexture(titleTexture, nullptr, nullptr, &titleRect.w, &titleRect.h);
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(titleSurface);


        // Draw Sudoku Grid
        SDL_SetRenderDrawColor(renderer, gridLineColor.r, gridLineColor.g, gridLineColor.b, gridLineColor.a);
        for (int i = 0; i <= N; ++i) { // Use N here
            SDL_RenderDrawLine(renderer, recx + i * recw, recy, recx + i * recw, recy + N * rech); // Vertical lines
            SDL_RenderDrawLine(renderer, recx, recy + i * rech, recx + N * recw, recy + i * rech); // Horizontal lines
        }

        // Draw thicker block lines
        SDL_SetRenderDrawColor(renderer, blockLineColor.r, blockLineColor.g, blockLineColor.b, blockLineColor.a);
        if (sqrt(N) == floor(sqrt(N))) {
            int blockSize = sqrt(N);
            for (int i = 1; i < blockSize; ++i) {
                SDL_RenderDrawLine(renderer, recx + i * blockSize * recw, recy, recx + i * blockSize * recw, recy + N * rech);
                SDL_RenderDrawLine(renderer, recx, recy + i * blockSize * rech, recx + N * recw, recy + i * blockSize * rech);
            }
        } else if (N == 9) { //Special case for 9x9
            SDL_RenderDrawLine(renderer, recx + 3 * recw, recy, recx + 3 * recw, recy + N * rech);
            SDL_RenderDrawLine(renderer, recx + 6 * recw, recy, recx + 6 * recw, recy + N * rech);
            SDL_RenderDrawLine(renderer, recx, recy + 3 * rech, recx + N * recw, recy + 3 * rech);
            SDL_RenderDrawLine(renderer, recx, recy + 6 * rech, recx + N * recw, recy + 6 * rech);
        }


        // Draw Numbers in Grid
        for (int i = 0; i < N; ++i) { // Use N here
            for (int j = 0; j < N; ++j) { // Use N here
                if (arr[j][i] != 0) {
                    string numStr = to_string(arr[j][i]);
                    SDL_Color textColor = { textColor.r, textColor.g, textColor.b, 255 }; // Ensure textColor is used
                    SDL_Surface* textSurface = TTF_RenderText_Solid(font, numStr.c_str(), textColor);
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_Rect textRect;
                    textRect.x = recx + (j * recw) + (recw - textSurface->w) / 2; // Center number in cell
                    textRect.y = recy + (i * rech) + (rech - textSurface->h) / 2;
                    SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
                    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                    SDL_DestroyTexture(textTexture);
                    SDL_FreeSurface(textSurface);
                }
            }
        }

        // Draw selected cell highlight
        if (dxy == 1 && enterQuestion == 1) {
            SDL_SetRenderDrawColor(renderer, selectedCellColor.r, selectedCellColor.g, selectedCellColor.b, selectedCellColor.a);
            SDL_Rect selectedCellRect = { stpx, stpy, recw, rech };
            SDL_RenderFillRect(renderer, &selectedCellRect);
        }


        // Buttons drawing
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &enterQuestionButtonRect);
        SDL_RenderFillRect(renderer, &doneButtonRect);
        SDL_RenderFillRect(renderer, &clearButtonRect);
        SDL_RenderFillRect(renderer, &generateAnswerButtonRect);

        SDL_SetRenderDrawColor(renderer, gridLineColor.r, gridLineColor.g, gridLineColor.b, gridLineColor.a); // Button borders same as grid lines
        SDL_RenderDrawRect(renderer, &enterQuestionButtonRect);
        SDL_RenderDrawRect(renderer, &doneButtonRect);
        SDL_RenderDrawRect(renderer, &clearButtonRect);
        SDL_RenderDrawRect(renderer, &generateAnswerButtonRect);

        // Button texts rendering
        SDL_Surface* enterQuestionTextSurface = TTF_RenderText_Solid(font, "Enter Question", buttonTextColor);
        SDL_Texture* enterQuestionTextTexture = SDL_CreateTextureFromSurface(renderer, enterQuestionTextSurface);
        SDL_Rect enterQuestionTextRect;
        enterQuestionTextRect.x = enterQuestionButtonRect.x + (enterQuestionButtonRect.w - enterQuestionTextSurface->w) / 2; // Center text
        enterQuestionTextRect.y = enterQuestionButtonRect.y + (enterQuestionButtonRect.h - enterQuestionTextSurface->h) / 2;
        SDL_QueryTexture(enterQuestionTextTexture, nullptr, nullptr, &enterQuestionTextRect.w, &enterQuestionTextRect.h);
        SDL_RenderCopy(renderer, enterQuestionTextTexture, nullptr, &enterQuestionTextRect);
        SDL_DestroyTexture(enterQuestionTextTexture);
        SDL_FreeSurface(enterQuestionTextSurface);


        SDL_Surface* doneTextSurface = TTF_RenderText_Solid(font, "Done", buttonTextColor);
        SDL_Texture* doneTextTexture = SDL_CreateTextureFromSurface(renderer, doneTextSurface);
        SDL_Rect doneTextRect;
        doneTextRect.x = doneButtonRect.x + (doneButtonRect.w - doneTextSurface->w) / 2;
        doneTextRect.y = doneButtonRect.y + (doneButtonRect.h - doneTextSurface->h) / 2;
        SDL_QueryTexture(doneTextTexture, nullptr, nullptr, &doneTextRect.w, &doneTextRect.h);
        SDL_RenderCopy(renderer, doneTextTexture, nullptr, &doneTextRect);
        SDL_DestroyTexture(doneTextTexture);
        SDL_FreeSurface(doneTextSurface);


        SDL_Surface* clearTextSurface = TTF_RenderText_Solid(font, "Clear", buttonTextColor);
        SDL_Texture* clearTextTexture = SDL_CreateTextureFromSurface(renderer, clearTextSurface);
        SDL_Rect clearTextRect;
        clearTextRect.x = clearButtonRect.x + (clearButtonRect.w - clearTextSurface->w) / 2;
        clearTextRect.y = clearButtonRect.y + (clearButtonRect.h - clearTextSurface->h) / 2;
        SDL_QueryTexture(clearTextTexture, nullptr, nullptr, &clearTextRect.w, &clearTextRect.h);
        SDL_RenderCopy(renderer, clearTextTexture, nullptr, &clearTextRect);
        SDL_DestroyTexture(clearTextTexture);
        SDL_FreeSurface(clearTextSurface);


        SDL_Surface* generateAnswerTextSurface = TTF_RenderText_Solid(font, "Generate Answer", buttonTextColor);
        SDL_Texture* generateAnswerTextTexture = SDL_CreateTextureFromSurface(renderer, generateAnswerTextSurface);
        SDL_Rect generateAnswerTextRect;
        generateAnswerTextRect.x = generateAnswerButtonRect.x + (generateAnswerButtonRect.w - generateAnswerTextSurface->w) / 2;
        generateAnswerTextRect.y = generateAnswerButtonRect.y + (generateAnswerButtonRect.h - generateAnswerTextSurface->h) / 2;
        SDL_QueryTexture(generateAnswerTextTexture, nullptr, nullptr, &generateAnswerTextRect.w, &generateAnswerTextRect.h);
        SDL_RenderCopy(renderer, generateAnswerTextTexture, nullptr, &generateAnswerTextRect);
        SDL_DestroyTexture(generateAnswerTextTexture);
        SDL_FreeSurface(generateAnswerTextSurface);


        // Messages
        SDL_Rect messageRect = { (screenWidth - 600) / 2, 100, 600, 30 }; // Centered message area below title, adjusted position
        if (enterQuestion == 1) {
            SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "Enter your Sudoku question in the table.", messageColor);
            SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
            messageRect.w = messageSurface->w; messageRect.h = messageSurface->h; messageRect.x = (screenWidth - messageRect.w) / 2; // Adjust message rect to text size and center
            SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
            SDL_DestroyTexture(messageTexture);
            SDL_FreeSurface(messageSurface);
        } else if (generateAnswer == 1 && generated != 1) {
            if (!isValidSudoku(arr, N)) {
                SDL_Surface* invalidInputSurface = TTF_RenderText_Solid(font, "Invalid Sudoku Input!", errorColor);
                SDL_Texture* invalidInputTexture = SDL_CreateTextureFromSurface(renderer, invalidInputSurface);
                messageRect.w = invalidInputSurface->w; messageRect.h = invalidInputSurface->h; messageRect.x = (screenWidth - messageRect.w) / 2; // Adjust message rect
                SDL_RenderCopy(renderer, invalidInputTexture, nullptr, &messageRect);
                SDL_DestroyTexture(invalidInputTexture);
                SDL_FreeSurface(invalidInputSurface);
            } else {
                SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "Generating Answer...", messageColor);
                SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
                messageRect.w = messageSurface->w; messageRect.h = messageSurface->h; messageRect.x = (screenWidth - messageRect.w) / 2; // Adjust message rect
                SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
                SDL_DestroyTexture(messageTexture);
                SDL_FreeSurface(messageSurface);
                if (solveSudoku(arr, N, 0, 0)) {
                    generated = 1;
                } else {
                    generated = 0;
                    generateAnswer = 0;
                    SDL_Surface* noSolutionSurface = TTF_RenderText_Solid(font, "Sudoku Unsolvable", messageColor);
                    SDL_Texture* noSolutionTexture = SDL_CreateTextureFromSurface(renderer, noSolutionSurface);
                    messageRect.w = noSolutionSurface->w; messageRect.h = noSolutionSurface->h; messageRect.x = (screenWidth - messageRect.w) / 2; // Adjust message rect
                    SDL_RenderCopy(renderer, noSolutionTexture, nullptr, &messageRect);
                    SDL_DestroyTexture(noSolutionTexture);
                    SDL_FreeSurface(noSolutionSurface);
                }
            }
        } else if (generateAnswer == 1 && generated == 1) {
            SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "Answer Generated!", messageColor);
            SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
            messageRect.w = messageSurface->w; messageRect.h = messageSurface->h; messageRect.x = (screenWidth - messageRect.w) / 2; // Adjust message rect
            SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
            SDL_DestroyTexture(messageTexture);
            SDL_FreeSurface(messageSurface);
        }


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

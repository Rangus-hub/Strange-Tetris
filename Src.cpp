#include<iostream>
#include<Windows.h>
#include<thread>//for this_thread command
#include<vector>
#include<random>


using namespace std;

wstring tetromino[7];

int fw = 12;
int fh = 18;

int scw = 120;
int sch = 30;

unsigned char* pfield = nullptr;


int Rotate(int x, int y, int r)
{
	switch (r % 4)
	{
	case 0:return (y * 4) + x;//0 deg
	case 1:return 12 + y - (x * 4);//90 deg
	case 2:return 15 - (y * 4) - x;//180 deg
	case 3:return 3 - y + (x * 4);//270 deg
	}
	return 0;
}


bool DoesPieceFit(int tm, int rot, int Px, int Py)//Px and Py indicate the position of Tetrominoblck(tm) is the top left cell of the tm
{
	for (int px = 0; px < 4; ++px)// To Iterate across tetromino block
		for (int py = 0; py < 4; ++py)
		{
			//Get index into piece
			int pi = Rotate(px, py, rot);

			//Get index into the field
			int fi = (Py + py) * fw + (Px + px);

			//Checking if we are going out of bounds(boundary conditions)
			if (Px + px >= 0 && Px + px < fw)
			{
				if (Py + py >= 0 && Py + py < fh)
				{
					if (tetromino[tm][pi] == L'X' && pfield[fi] != 0)
						return false;//fail on first hit
				}
			}

		}

	return true;
}

int main()
{
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");


	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");


	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	pfield = new unsigned char[fw * fh];

	for (int x = 0; x < fw; ++x)
	{
		for (int y = 0; y < fh; ++y)
		{
			pfield[y * fw + x] = (x == 0 || x == fw - 1 || y == fh - 1) ? 9 : 0;//creating border of he field
			//9 is used to represent the hash border otherwise set everything to empty
		}
	}

	//Generating Screen Buffer
	wchar_t* screen = new wchar_t[scw * sch];
	for (int i = 0; i < scw * sch; ++i)
		screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;


	//Game Logic Stuff
	//Random Generator
	mt19937 rng(std::random_device{}());
	uniform_int_distribution<int> dist(0, 6);

	bool GameOver = false;

	//int currentpiece = 1;
	int currentpiece = dist(rng);
	int currentrot = 0;
	int currentX = fw / 2;//Since it is the first block has to be in the middle
	int currentY = 0;//at the top

	bool Key[4];//for user input keys
	bool rotateHoldKey = false;// to resolve the continuous rotation within 50 ms

	int speed = 20;// to inc the difficulty by progressively we have set a certain speed, which is also the current level
	int speedcounter = 0;//and a counter to decrease the tick time
	bool ForcePieceDown = false;
	int piececounter = 0;//Lets say increase the tick by 10 ms if 10 pieces are placed
	int score = 0;

	vector<int> lines;
	
	//Colors Each Block
	WORD tetrominoColors[7] = {
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,  // Color for piece 0 (White)
	FOREGROUND_INTENSITY | FOREGROUND_GREEN,      // Color for piece 1 (Green)
	FOREGROUND_INTENSITY | FOREGROUND_BLUE,       // Color for piece 2 (Blue)
	FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,  // Color for piece 3 (Yellow)
	FOREGROUND_RED | FOREGROUND_INTENSITY | FOREGROUND_BLUE,  // Color for piece 4 (Magenta)
	FOREGROUND_RED,  // Color for piece 5 (Purple)
	FOREGROUND_GREEN | FOREGROUND_BLUE   // Color for piece 6 (Cyan)
	};

	while (!GameOver)
	{
		//Game Timing===========================
		//In tetris the delay is a simple tick, this time is given for user input
		this_thread::sleep_for(50ms);
		speedcounter++;
		ForcePieceDown = (speedcounter == speed);

		//Game Input============================
		for (int k = 0; k < 4; ++k)
		{
			//AsyncKeystate tells us whether key is pressed or not
			// \x27\x25\x28Z are hexd values for R, L, D and Z key
			Key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		}


		//Game Logic============================
		if (Key[1])//if user presses L key
		{
			if(DoesPieceFit(currentpiece, currentrot, currentX - 1, currentY))//-1 because we want to check to the left(L key)
			{
				currentX = currentX - 1;
			}
		}

		if (Key[0])//if user presses R key
		{
			if (DoesPieceFit(currentpiece, currentrot, currentX + 1, currentY))//similarly +1 for Right
			{
				currentX = currentX + 1;
			}
		}

		if (Key[2])//if user presses R key
		{
			if (DoesPieceFit(currentpiece, currentrot, currentX , currentY + 1))//similarly +1 for Right
			{
				currentY = currentY + 1;
			}
		}

		if (Key[3])//if user presses R key
		{
			if (!rotateHoldKey && DoesPieceFit(currentpiece, currentrot + 1, currentX, currentY))//similarly +1 for Right
			{
				currentrot = currentrot + 1;
				rotateHoldKey = true;// to avoid continuous rot of the block within the 50ms
			}
		}
		else
			rotateHoldKey = false;

		if (ForcePieceDown)
		{
			if (DoesPieceFit(currentpiece, currentrot, currentX, currentY + 1))
				currentY = currentY + 1;//It can so do it
			else
			{
				//lock the current piece in the field
				for (int px = 0; px < 4; ++px)
					for (int py = 0; py < 4; ++py)
						if (tetromino[currentpiece][Rotate(px, py, currentrot)] == L'X')
							pfield[(currentY + py) * fw + (currentX + px)] = currentpiece + 1;


				++piececounter;
				if (piececounter % 10 == 0)
					if (speed >= 10)
						--speed;



				//check if any lines exist
				for (int py = 0; py < 4; ++py)
				{
					if (currentY + py < fh - 1)
					{
						bool ifline = true;//assuming line exists
						for (int px = 1; px < fw - 1; ++px)
							ifline &= (pfield[(currentY + py) * fw + px]) != 0;

						if (ifline == true)
						{
							//remove line and set to:
							for (int px = 1; px < fw - 1; ++px)
							{
								pfield[(currentY + py) * fw + px] = 8;
							}

							lines.push_back(currentY + py);
						}
					}
				}

				score += 25;
				if (!lines.empty())
					score += 50 * (1 << lines.size());//exponential score increase

				//choose next piece
				currentX = fw / 2;
				currentY = 0;
				currentrot = 0;
				currentpiece = dist(rng);//Better distribution

				//if piece does not fit
				GameOver = !DoesPieceFit(currentpiece, currentrot, currentX, currentY);
			}
			speedcounter = 0;
		}


		//Render Output


		//Draw Field
		for (int x = 0; x < fw; ++x)
			for (int y = 0; y < fh; ++y)
				screen[(y + 2) * scw + (x + 2)] = L" ABCDEFG=#"[pfield[y * fw + x]];


		//Draw Current Piece
		for (int px = 0; px < 4; ++px)
			for (int py = 0; py < 4; ++py)
				if (tetromino[currentpiece][Rotate(px, py, currentrot)] == L'X')
				{
					WORD colc = tetrominoColors[currentpiece];
					SetConsoleTextAttribute(hConsole, colc);  // Set console text attribute

					screen[(currentY + py + 2) * scw + (currentX + px + 2)] = currentpiece + 65;//Capital
					FillConsoleOutputAttribute(hConsole, colc, 1, { static_cast<SHORT>(currentX + px + 2), static_cast<SHORT>(currentY + py + 2) }, & dwBytesWritten); // Set the color attribute for the cell
				}
					


		//Draw score
		swprintf_s(&screen[2 * scw + fw + 6], 16, L"Score: %8d", score);


		if (!lines.empty())//If lines exist
		{
			WriteConsoleOutputCharacter(hConsole, screen, scw*sch, { 0,0 }, & dwBytesWritten);//Display frame to draw lines
			this_thread::sleep_for(400ms);//Delay a little

			for(auto &v:lines)//move blocks above lines downwards
				for (int px = 1; px < fw - 1; ++px)
				{
					for (int py = v; py > 0; --py)
						pfield[py * fw + px] = pfield[(py - 1) * fw + px];

					pfield[px] = 0;
				}

			lines.clear();
		}

		//Display Frame- Display the console screen
		WriteConsoleOutputCharacter(hConsole, screen, scw * sch, { 0,0 }, &dwBytesWritten);
	}

	//After GameOver:
	CloseHandle(hConsole);
	cout << "GAME OVER\nSCORE: " << score;
	system("pause");
}


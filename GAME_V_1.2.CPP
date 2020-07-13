#include <GLFW/glfw3.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

class Color {
public:
	int r, g, b, iter = 10;
	Color(int r, int g, int b) {
		this->r = this->checkColor(r);
		this->g = this->checkColor(g);
		this->b = this->checkColor(b);
	}
	Color() : Color(0,0,0){}
	void setObjectColor() {
		float r_ = ((float)this->r) / 255;
		float g_ = ((float)this->g) / 255;
		float b_ = ((float)this->b) / 255;
		glColor3f(r_, g_, b_);
	}
	int checkColor(int k) {
		if (k > 255) k = 255;
		else if (k < 0) k = 0;
		return k;
	}
	void setColor(Color c_) {
		this->r = this->checkColor(c_.r);
		this->g = this->checkColor(c_.g);
		this->b = this->checkColor(c_.b);
	}
	void setColor(int r, int g, int b) {
		this->r = this->checkColor(r);
		this->g = this->checkColor(g);
		this->b = this->checkColor(b);
	}
	Color operator++ (int) {
		this->r = this->checkColor(r + this->iter);
		this->g = this->checkColor(g + this->iter);
		this->b = this->checkColor(b + this->iter);
		return *this;
	}
	Color operator-- (int) {
		this->r = this->checkColor(r - this->iter);
		this->g = this->checkColor(g - this->iter);
		this->b = this->checkColor(b - this->iter);
		return *this;
	}
};

class Board{
private:
	Board(int sizeWindow, int mashtab) {
		this->s_win = sizeWindow;
		this->mashtab = mashtab;
	}
protected:
	int s_win, mashtab;
public:
	Board() {}
	int get_s_win() {
		return this->s_win;
	}
	int get_mashtab() {
		return this->mashtab;
	}
	static Board* instance;
	static Board* getInstance(int sizeWindow, int mashtab_) {
		if (Board::instance == 0) {
			Board::instance = new Board(sizeWindow, mashtab_);
		}
		return Board::instance;
	}
};
Board* Board::instance = 0;

class Tank {
public:
	float posX, posY, wheelsN, sizeCube;
	Color color;
	unsigned int direction = 1;
	float speed = 0.6;
	bool k = 0;
	Tank(int x_, int y_) {
		this->posX = x_;
		this->posY = y_;
		this->wheelsN = 20;
		this->sizeCube = 10;
	}
	Tank setColor(Color c_) {
		this->color = c_;
		return *this;
	}
	Tank* operator*() {
		return this;
	}
};

class Draw {
public:
	Draw setColor(int r, int g, int b) {
		this->color.setColor(r, g, b);
		return *this;
	}
	Draw drawRect(float x, float y, float w, float h, bool border = false) {
		glBegin(GL_POLYGON);
			color.setObjectColor();
			this->Dot(x, y);
			this->Dot(x + w, y);
			this->Dot(x + w, y + h);
			this->Dot(x, y + h);
		glEnd();
		return *this;
	}
	Draw drawRectCentered(int cx, int cy, int rx, int ry) {
		return this->drawRect(cx - rx, cy - ry, 2 * rx, 2 * ry);
	}
	Draw drawLine(int x1, int y1, int x2, int y2) {
		glBegin(GL_LINES);
			color.setObjectColor();
			this->Dot(x1, y1);
			this->Dot(x2, y2);
		glEnd();
		return *this;
	}
	Draw drawPolygon(int pos = -1) {
		if (pos == 0) {
			glBegin(GL_POLYGON);
			this->color.setObjectColor();
		}
		else if (pos == 1) {
			glEnd();
		}
		return *this;
	}

	Draw drawTankBody(Tank* t) {
		this->drawRect(t->posX - t->sizeCube * 0.5, t->posY - t->sizeCube * 0.125, t->sizeCube, t->sizeCube * 0.6);
		return *this;
	}
	Draw drawTankWheels(Tank* t) {
		color.setColor(color++);
		this->drawRect(t->posX - t->sizeCube * 0.5, t->posY - t->sizeCube * 0.25, t->sizeCube * 0.25, t->sizeCube * 0.75).
			drawRect(t->posX + t->sizeCube * 0.25, t->posY - t->sizeCube * 0.25, t->sizeCube * 0.25, t->sizeCube * 0.75);
		float n = (t->sizeCube * 0.75 ) / t->wheelsN;
		for (int i = 1; i <= t->wheelsN; i++) {
			if (i % 2 == t->k) {
				continue;
			}
			color.setColor(color++);
			this->drawRect(t->posX - t->sizeCube * 0.5, t->posY - t->sizeCube * 0.25 + n * i, t->sizeCube * 0.25, (t->sizeCube * 0.75) / t->wheelsN).
				drawRect(t->posX + t->sizeCube * 0.25, t->posY - t->sizeCube * 0.25 + n * i, t->sizeCube * 0.25, (t->sizeCube * 0.75) / t->wheelsN);
		}
		return *this;
	}
	Draw drawTankGun(Tank* t) {
		color.setColor(color++);
		this->drawPolygon(0).
			Dot(t->posX, t->posY).
			Dot(t->posX - t->sizeCube * 0.0625, t->posY).
			Dot(t->posX - t->sizeCube * 0.125, t->posY - t->sizeCube * 0.5).
			Dot(t->posX + t->sizeCube * 0.125, t->posY - t->sizeCube * 0.5).
			Dot(t->posX + t->sizeCube * 0.0625, t->posY).
			Dot(t->posX, t->posY).
			drawPolygon(1);
		return *this;
	}
	Draw drawTank(Tank *t) {
		this->color.setColor(t->color);
		this->drawTankBody(t).drawTankWheels(t).drawTankGun(t);
		return *this;
	}
	Draw Dot(float x, float y) {
		float s_win = board->get_s_win();
		float mashtab = board->get_mashtab();
		float x_ = (x * mashtab / (s_win / 2)) - 1;
		float y_ = (y * mashtab / (s_win / 2)) - 1;
		glVertex2d(x_, -y_);
		return *this;
	}
	void setBoard(Board* board) {
		this->board = board;
	}
	Color color;
	Board* board;
};

void moveAllTanks(vector<Tank*> tanks_, Board* b) {
	for (auto tank : tanks_) {
		float x_ = tank->posX;
		float y_ = tank->posY;
		if (tank->direction == 1) {
			y_ -= tank->speed;
		}
		else if (tank->direction == 2) {
			x_ += tank->speed;
		}
		else if (tank->direction == 3) {
			y_ += tank->speed;
		}
		else if (tank->direction == 4) {
			x_ -= tank->speed;
		}
		int n = b->get_s_win() / b->get_mashtab();
		if (y_ + tank->sizeCube / 2 > n) y_ = n - tank->sizeCube / 2;
		if (x_ + tank->sizeCube / 2 > n) x_ = n - tank->sizeCube / 2;
		if (y_ - tank->sizeCube / 2 < 0) y_ = tank->sizeCube / 2;
		if (x_ - tank->sizeCube / 2 < 0) x_ = tank->sizeCube / 2;
		tank->posX = x_;
		tank->posY = y_;
		if(tank->direction != 5) tank->k = !tank->k;
	}
}

void drawAllTanks(Draw d_, vector<Tank*> tanks_) {
	for (auto tank : tanks_) {
		d_.drawTank(tank);
	}
}

void checkJoistic(Tank* t_) {
	if (GetKeyState(VK_UP) & 0x8000)
	{
		t_->direction = 1;
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		t_->direction = 2;
	}
	else if (GetKeyState(VK_DOWN) & 0x8000)
	{
		t_->direction = 3;
	}
	else if (GetKeyState(VK_LEFT) & 0x8000)
	{
		t_->direction = 4;
	}
	else t_->direction = 5;
}

int main(void)
{
	GLFWwindow* window;
	glfwInit();
	window = glfwCreateWindow(500, 500, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	vector<Tank*> tanks;

	Board* board = Board::getInstance(500,10);
	Tank* myTank = new Tank(25, 25);
	tanks.push_back(myTank);
	Draw d;
	d.setBoard(board);
	myTank->setColor(Color(255, 40, 130));

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Sleep(10);
		checkJoistic(myTank);
		moveAllTanks(tanks, board);
		drawAllTanks(d, tanks);
		glLoadIdentity();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}
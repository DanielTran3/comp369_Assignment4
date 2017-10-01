#ifndef _SPRITE_H
#define _SPRITE_H 1

#include "allegro.h"

class Sprite {
	private:
		int _alive;
		int _leftOrRightSide;
		int _direction;
		int _animDir;
		int _animColumns;
		int _width, _height;
		int _xDelay, _yDelay;
		int _xCount, _yCount;
		int _curFrame, _totalFrames;
		int _frameCount, _frameDelay;
		int _animStartX, _animStartY;
		int _faceAngle, _moveAngle;

		double _x, _y;
		double _speed;
		double _velX, _velY;

		bool _passLeftSideToLose;

		BITMAP *_image;
	public:
		Sprite();
		~Sprite();
		
		int Load(BITMAP *image);
		void Draw(BITMAP *dest);
		void DrawFrame(BITMAP *dest);
		void UpdatePosition();
		void UpdateAnimation();
		int Inside(int x,int y,int left,int top,int right,int bottom);
		int PointInside(int px,int py);
		int Collided(Sprite *other = NULL, int shrink = 5);
		double CenterX();
		double CenterY();
		void ChangeDirection();
		void IncreaseSpeed();
		
		int getAlive();
		void setAlive(int alive);
		int getLeftOrRightSide();
		void setLeftOrRightSide(int leftOrRightSide);
		int getDirection();
		void setDirection(int direction);
		int getAnimDir();
		void setAnimDir(int animDir);
		int getAnimColumns();
		void setAnimColumns(int animColumns);
		int getWidth();
		void setWidth(int width);
		int getHeight();
		void setHeight(int height);
		int getXDelay();
		void setXDelay(int xDelay);
		int getYDelay();
		void setYDelay(int yDelay);
		int getXCount();
		void setXCount(int xCount);
		int getYCount();
		void setYCount(int yCount);
		int getCurFrame();
		void setCurFrame(int curFrame);
		int getTotalFrames();
		void setTotalFrames(int totalFrames);
		int getFrameCount();
		void setFrameCount(int frameCount);
		int getFrameDelay();
		void setFrameDelay(int frameDelay);
		int getAnimStartX();
		void setAnimStartX(int animStartX);
		int getAnimStartY();
		void setAnimStartY(int animStartY);
		int getFaceAngle();
		void setFaceAngle(int faceAngle);
		int getMoveAngle();
		void setMoveAngle(int moveAngle);

		double getX();
		void setX(double x);
		double getY();
		void setY(double y);
		double getSpeed();
		void setSpeed(double speed);
		double getVelX();
		void setVelX(double velX);
		double getVelY();
		void setVelY(double velY);
		
		bool getPassLeftSideToLose();
		void setPassLeftSideToLose(bool passLeftSideToLose);

		BITMAP *getImage();
		void setImage(BITMAP *image);
};

#endif

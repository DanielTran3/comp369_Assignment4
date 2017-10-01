#ifndef _SPRITE_H
#define _SPRITE_H 1

#include "toTheTopDefs.h"

class Sprite {
	private:
		int _alive;
		int _direction;
		int _animDir;
		int _animColumns;
		int _width, _height;
		int _xDelay, _yDelay;
		int _xCount, _yCount;
		int _curFrame, _totalFrames;
		int _frameCount, _frameDelay;
		int _animStartX, _animStartY;
		int _framesStart, _framesEnd;
		int _jump;
		int _levelReached;
				
		double _x, _y;
		double _velX, _velY;

		BITMAP *_image;
		
	public:
		Sprite();
		~Sprite();
		
		int Load(BITMAP *image);
		void Draw(BITMAP *dest, int xOffset, int yOffset);
		void DrawFrame(BITMAP *dest, int xOffset, int yOffset);
		void UpdatePosition();
		void UpdateAnimation();
		int Inside(int x,int y,int left,int top,int right,int bottom);
		int PointInside(int px,int py);
		int Collided(Sprite *other = NULL, int shrink = 5);
		int CollideWithBlock(int x, int y);
		int GetBlockData1(int x, int y);
		double CenterX();
		double CenterY();
		void PlayerControls(Sound *sounds);
		void UpdateLevelReached();
		void Walk();
		void Move();
		
		int getAlive();
		void setAlive(int alive);
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
		int getFramesStart();
		void setFramesStart(int framesStart);
		int getFramesEnd();
		void setFramesEnd(int framesEnd);
		int getJump();
		void setJump(int jump);
		int getLevelReached();
		
		double getX();
		void setX(double x);
		double getY();
		void setY(double y);
		double getVelX();
		void setVelX(double velX);
		double getVelY();
		void setVelY(double velY);

		BITMAP *getImage();
		void setImage(BITMAP *image);
};

#endif

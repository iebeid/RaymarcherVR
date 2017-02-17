#ifndef RAYMARCHER_CAVE_H
#define RAYMARCHER_CAVE_H 1

#include <vrj/vrjConfig.h>

#include <iostream>
#include <iomanip>

#include <vrj/Draw/OpenGL/App.h>

#include <gadget/Type/PositionInterface.h>
#include <gadget/Type/DigitalInterface.h>

#include <GL/glew.h>

using namespace vrj;

class CaveApp : public opengl::App
{
public:
	CaveApp() : mCurTime(0.0f)
	{
		;
	}

	virtual ~CaveApp(void) { ; }

public:

	virtual void init();

	virtual void apiInit();

public:

	virtual void bufferPreDraw();

	virtual void preFrame();

	virtual void intraFrame();

	virtual void postFrame();

	virtual void latePreFrame();

	virtual void contextPostDraw();

	virtual void contextPreDraw();

public:

	virtual void contextInit()
	{
		initGLState();
	}

	virtual void draw();

private:

	void initGLState();

public:

	float mCurTime;
	gadget::PositionInterface  mWand;
	gadget::PositionInterface  mHead;
	gadget::DigitalInterface   mButton0;
	gadget::DigitalInterface   mButton1;

};

#endif
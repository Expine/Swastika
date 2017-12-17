#ifndef  _GAME_SCENE_H_
#define  _GAME_SCENE_H_

#include "cocos2d.h"

enum class Mode
{
	none, hor, ver
};

class GameScene : public cocos2d::Layer
{
private:
	double r;
	double x;
	double y;

	Mode _mode;
	std::queue<cocos2d::Vec2> _poses;
	cocos2d::Sprite* _c;


	cocos2d::Vec2 touch_start;

	cocos2d::Vec2 hor_start;
	cocos2d::Vec2 hor_end;

	cocos2d::Vec2 ver_start;
	cocos2d::Vec2 ver_end;

	cocos2d::EventListenerTouchOneByOne* _lis;

	void showLine();
	void checkClear();
	void clear();
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
};

#endif // _GAME_SCENE_H_
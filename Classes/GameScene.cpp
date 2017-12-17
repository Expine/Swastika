#include "HelloWorldScene.h"
#include "GameScene.h"

#include <random>

static std::mt19937 _mt;

constexpr float elim = 20.0f;

USING_NS_CC;

void initRand()
{
	std::random_device rndDev;
	_mt = std::mt19937(rndDev());
}

int getRand(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(_mt);
}

double getRand(double min, double max)
{
	std::uniform_real_distribution<double> dist(min, max);
	return dist(_mt);
}

cocos2d::Scene * GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initRand();
	r = getRand(50.0, 250.0);
	x = getRand(r + 10, 800 - r - 10);
	y = getRand(r + 10, 600 - r - 10);
	for (int i = 0; i < 100; i++)
		_poses.push(Vec2(getRand(r + 10, 800 - r - 10), getRand(r + 10, 600 - r - 10)));
	hor_start = hor_end = ver_start = ver_end = Vec2::ZERO;
	_mode = Mode::none;

	auto c = Sprite::create("c.png");
	c->setPosition(x, y);
	c->setScale(r / 50);
	this->addChild(c);
	_c = c;

	auto hor = Sprite::create("l.png");
	hor->setTag(0);
	this->addChild(hor);

	auto ver = Sprite::create("v.png");
	ver->setTag(1);
	this->addChild(ver);

	auto lis = EventListenerTouchOneByOne::create();
	lis->onTouchBegan = [this](Touch* touch, Event* event)
	{
		touch_start = touch->getLocation();
		return true;
	};
	lis->onTouchMoved = [this](Touch* touch, Event* event)
	{
		if (_mode == Mode::none)
		{
			auto vx = std::abs(touch_start.x - touch->getLocation().x);
			auto vy = std::abs(touch_start.y - touch->getLocation().y);
			if (vx > elim && vx > vy)
			{
				_mode = Mode::hor;
				hor_start = touch_start;
			}
			else if (vy > elim)
			{
				_mode = Mode::ver;
				ver_start = touch_start;
			}
		}
		if (_mode == Mode::hor)
		{
			hor_end = touch->getLocation();
			hor_start.y = hor_end.y;
		}
		else if (_mode == Mode::ver)
		{
			ver_end = touch->getLocation();
			ver_start.x = ver_end.x;
		}
		showLine();
	};
	lis->onTouchEnded = [this](Touch* touch, Event* event)
	{
		if (_mode == Mode::hor)
		{
			hor_end = touch->getLocation();
			hor_start.y = hor_end.y;
		}
		else if (_mode == Mode::ver)
		{
			ver_end = touch->getLocation();
			ver_start.x = ver_end.x;
		}
		_mode = Mode::none;
		showLine();
		checkClear();
	};
	lis->onTouchCancelled = lis->onTouchEnded;
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, this);
	_lis = lis;

	c->runAction(RepeatForever::create(Sequence::create(CallFunc::create([this] { _c->runAction(EaseSineInOut::create(MoveTo::create(1.0f, _poses.front()))); _poses.push(_poses.front()); _poses.pop(); }), DelayTime::create(1.5f), NULL)));

	return true;
}

void GameScene::showLine()
{
	auto hor = hor_start.x - hor_end.x;
	auto ver = ver_start.y - ver_end.y;
	this->getChildByTag(0)->setPosition((hor_start + hor_end) / 2);
	this->getChildByTag(0)->setScaleX(hor / 100);
	this->getChildByTag(1)->setPosition((ver_start + ver_end) / 2);
	this->getChildByTag(1)->setScaleY(ver / 100);
}

void GameScene::checkClear()
{
	if (hor_end != Vec2::ZERO && ver_end != Vec2::ZERO)
	{
		_lis->setEnabled(false);
		_c->stopAllActions();
		this->runAction(Sequence::create(
			Spawn::create(
				TargetedAction::create(this->getChildByTag(0), EaseElasticIn::create(ScaleTo::create(1.0f, 16.0f, 1.0f))),
				TargetedAction::create(this->getChildByTag(1), EaseElasticIn::create(ScaleTo::create(1.0f, 1.0f, 16.0f))),
			NULL),
			CallFunc::create([this] { clear(); }),
		NULL));
	}
}

void GameScene::clear()
{
	auto win = Director::getInstance()->getWinSize();

	x = _c->getPosition().x;
	y = _c->getPosition().y;

	auto back = Sprite::create();
	back->setTextureRect(Rect(0, 0, 800, 500));
	back->setPosition(win.width / 2, win.height / 2);
	back->setColor(Color3B::BLACK);
	back->setOpacity(200);
	this->addChild(back);

	auto s_1 = (ver_start.x - (x - r)) * ((y + r) - hor_start.y) / r / r / 4;
	auto s_2 = ((x + r) - ver_start.x) * ((y + r) - hor_start.y) / r / r / 4;
	auto s_3 = (ver_start.x - (x - r)) * (hor_start.y - (y - r)) / r / r / 4;
	auto s_4 = ((x + r) - ver_start.x) * (hor_start.y - (y - r)) / r / r / 4;
	if (Rect(x - r, y - r, x + r, y + r).containsPoint(Vec2(ver_start.x, hor_start.y)) && s_1 >= 0 && s_2 >= 0 && s_3 >= 0 && s_4 >= 0)
	{
		auto s_5 = (1.0 - std::max(std::abs(s_1 - s_2), std::max(std::abs(s_1 - s_3), std::max(std::abs(s_1 - s_4), std::max(std::abs(s_2 - s_3), std::max(std::abs(s_2 - s_4), std::abs(s_3 - s_4))))))) * 100;
		auto score = Label::createWithSystemFont(StringUtils::format("1:%f \n 2:%f \n 3:%f \n 4:%f", s_1, s_2, s_3, s_4), "Arial", 30);
		score->setPosition(win.width / 2, win.height / 2 + 100);
		this->addChild(score);

		auto s = Label::createWithSystemFont(StringUtils::format("Split Score %f", s_5), "Arial", 30);
		s->setPosition(win.width / 2, win.height / 2 - 30);
		this->addChild(s);

		auto user = UserDefault::getInstance();
		auto old = user->getDoubleForKey("high_score", 0.0f);
		if (old < s_5)
		{
			user->setDoubleForKey("high_score", s_5);
			old = s_5;
		}

		auto low = user->getDoubleForKey("low_score", 100.0f);
		if (low > s_5)
		{
			user->setDoubleForKey("low_score", s_5);
			low = s_5;
		}

		auto t = Label::createWithSystemFont(StringUtils::format("High  Score %f", old), "Arial", 30);
		t->setPosition(win.width / 2, win.height / 2 - 80);
		this->addChild(t);

		auto l = Label::createWithSystemFont(StringUtils::format("Low   Score %f", low), "Arial", 30);
		l->setPosition(win.width / 2, win.height / 2 - 130);
		this->addChild(l);
	}
	else
	{
		auto score = Label::createWithSystemFont(StringUtils::format("Split failure"), "Arial", 30);
		score->setPosition(win.width / 2, win.height / 2);
		this->addChild(score);
	}


	auto lis = EventListenerTouchOneByOne::create();
	lis->setSwallowTouches(true);
	lis->onTouchBegan = [](Touch* touch, Event* event)
	{
		Director::getInstance()->replaceScene(HelloWorld::createScene());
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, back);
}

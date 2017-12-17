#include "HelloWorldScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	auto win = Director::getInstance()->getWinSize();

	auto title = Label::createWithSystemFont("Swastika", "Arial", 40);
	title->setPosition(win.width / 2, win.height / 2);
	this->addChild(title);

	auto start = Label::createWithSystemFont("Tap to start", "Arial", 20);
	start->setPosition(win.width / 2, win.height / 2 - 60);
	this->addChild(start);
    
	auto lis = EventListenerTouchOneByOne::create();
	lis->onTouchBegan = [](Touch *touch, Event *event)
	{
		Director::getInstance()->replaceScene(GameScene::createScene());
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, this);

    return true;
}
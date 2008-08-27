/*
 * Clone.cpp
 *
 *  Created on: 14-Авг-08
 *      Author: sa-bu
 */

#include <Ogre.h>
#include <OIS/OIS.h>
#include <MyGUI.h>
#include "Server.h"
#include "Client.h"

#define CSCLONE_VERSION "0.0.1"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#if OGRE_DEBUG_MODE == 1
		#define PLUGINS_CFG ".\\plugins_win_d.cfg"
	#else
		#define PLUGINS_CFG ".\\plugins.cfg"
	#endif
	#define RESOURCES_CFG ".\\resources.cfg"
	#define OGRE_CFG ".\\ogre.cfg"
	#define CSCLONE_CFG ".\\csclone.cfg"
	#define OGRE_LOG ".\\ogre.log"
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	#if OGRE_DEBUG_MODE == 1
		#define PLUGINS_CFG "./plugins_linux.cfg"
		#define RESOURCES_CFG "./resources.cfg"
		#define OGRE_CFG "./ogre.cfg"
		#define CSCLONE_CFG "./blockpost.cfg"
		#define OGRE_LOG "./ogre.log"
	#else
		#define PLUGINS_CFG "/etc/csclone/plugins.cfg"
		#define RESOURCES_CFG "/etc/csclone/resources.cfg"
		#define OGRE_CFG "/etc/csclone/ogre.cfg"
		#define CSCLONE_CFG "/etc/csclone/blockpost.cfg"
		#define OGRE_LOG "/var/log/ogre.log"
	#endif
#endif

enum PlayerState {
	cmForceChooce,
	cmBuying,
	cmPlaying,
	cmGuest
};

class Application : public Ogre::FrameListener, public Ogre::WindowEventListener,
	public OIS::MouseListener, public OIS::KeyListener
{
private:
	Ogre::Root* 			mRoot;
	Ogre::RenderWindow* 	mWindow;
	Ogre::Camera* 			mCamera;
	Ogre::SceneManager*		mSceneMgr;

	OIS::InputManager*		mInputManager;
	OIS::Keyboard*			mKeyboard;
	OIS::Mouse*				mMouse;

	MyGUI::Gui*				mGUI;

	bool 					mExit;

	Ogre::Radian			mCameraAngleH;
	Ogre::Radian			mCameraAngleV;

	Server*					mServer;
	Client*					mClient;

	void initResources()
	{
		Ogre::ConfigFile cf;
		cf.load(RESOURCES_CFG);

		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		Ogre::String secName, typeName, archName;

		while (seci.hasMoreElements()) {
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
				archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
			}
		}
	}

	void centerWidget(MyGUI::WidgetPtr widget)
	{
		widget->setPosition(mGUI->getViewWidth()/2 - widget->getWidth()/2,
			mGUI->getViewHeight()/2 - widget->getHeight()/2);
	}

	void notifyResumeGame(MyGUI::WidgetPtr sender)
	{
	}

	void notifyDisconnect(MyGUI::WidgetPtr sender)
	{
	}

	void notifyPlayerList(MyGUI::WidgetPtr sender)
	{
	}

	void notifyNewGame(MyGUI::WidgetPtr sender)
	{
		mGUI->findWidgetT("Create")->show();
	}

	void notifyFindServers(MyGUI::WidgetPtr sender)
	{
		mGUI->findWidgetT("Servers")->show();
		mClient->sendFindServer();
	}

	void notifyOptions(MyGUI::WidgetPtr sender)
	{
		mGUI->findWidgetT("Options")->show();
	}

	void notifyExit(MyGUI::WidgetPtr sender)
	{
		MyGUI::WidgetPtr window = mGUI->findWidgetT("Quit");
		window->show();
		MyGUI::InputManager::getInstance().addWidgetModal(window);
	}

	void notifyServersClose(MyGUI::WidgetPtr sender, const std::string& name)
	{
		if (name == "close")
			sender->hide();
	}

	void notifyServersConnect(MyGUI::WidgetPtr sender)
	{
		MyGUI::MultiListPtr list = mGUI->findWidget<MyGUI::MultiList>("Servers/List");
		size_t sel = list->getItemSelect();
		if (sel == MyGUI::ITEM_NONE)
			return;
		const Ogre::UTFString& serverName = list->getItem(sel);
		mClient->connectToServer(serverName);
	}

	void notifyOptionsOK(MyGUI::WidgetPtr sender)
	{
		mGUI->findWidgetT("Options")->hide();
	}

	void notifyOptionsCancel(MyGUI::WidgetPtr sender)
	{
		mGUI->findWidgetT("Options")->hide();
	}

	void notifyOptionsApply(MyGUI::WidgetPtr sender)
	{
	}

	void notifyQuitQuit(MyGUI::WidgetPtr sender)
	{
		mExit = true;
	}

	void notifyQuitCancel(MyGUI::WidgetPtr sender)
	{
		MyGUI::WidgetPtr window = mGUI->findWidgetT("Quit");
		window->hide();
		MyGUI::InputManager::getInstance().removeWidgetModal(window);
	}

public:
	Application() : mRoot(0), mWindow(0), mCamera(0),
		mInputManager(0), mKeyboard(0), mMouse(0), mExit(false),
		mCameraAngleH(0), mCameraAngleV(0), mServer(0), mClient(0)
	{

	}

	~Application()
	{
		Socket::uninitialise();

		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);

		if (mRoot) {
			if (mSceneMgr)
				mRoot->destroySceneManager(mSceneMgr);
			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
			delete mRoot;
		}
	}

	void parseCommandLine(const char* cmdLine)
	{
	}

	void commandLine(int argc, char** argv)
	{
		int icmd = 1;
		while (icmd < argc) {
			if (strcmp(argv[icmd], "-map") == 0) { //server load map
				mServer->loadMap(argv[icmd+1]);
			} else if (strcmp(argv[icmd], "-server") == 0) { //client connect to server
			}
		}
	}

	void windowResized(Ogre::RenderWindow* rw)
	{
		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = rw->getWidth();
		ms.height = rw->getHeight();
	}

	void windowClosed(Ogre::RenderWindow* rw)
	{
		mExit = true;

		if (mInputManager) {
			Ogre::LogManager::getSingletonPtr()->logMessage("*-*-* OIS Shutdown");
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}

	bool frameStarted(const Ogre::FrameEvent& evt)
	{
		if (mExit)
			return false;
		if (mWindow->isClosed())
			return false;

		mKeyboard->capture();
		mMouse->capture();

		if (mKeyboard->isKeyDown(OIS::KC_A) || mKeyboard->isKeyDown(OIS::KC_LEFT))
			mCamera->moveRelative(Ogre::Vector3(-evt.timeSinceLastFrame*20, 0, 0));

		if (mKeyboard->isKeyDown(OIS::KC_D) || mKeyboard->isKeyDown(OIS::KC_RIGHT))
			mCamera->moveRelative(Ogre::Vector3(evt.timeSinceLastFrame*20, 0, 0));

		if (mKeyboard->isKeyDown(OIS::KC_W) || mKeyboard->isKeyDown(OIS::KC_UP))
			mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.timeSinceLastFrame*20));

		if (mKeyboard->isKeyDown(OIS::KC_S) || mKeyboard->isKeyDown(OIS::KC_DOWN))
			mCamera->moveRelative(Ogre::Vector3(0, 0, evt.timeSinceLastFrame*20));

		if (mServer)
			mServer->checkRecv();
		if (mClient)
			mClient->checkRecv();

		//check exists servers
		if (mGUI->findWidgetT("Servers")->isShow()) {
			MyGUI::MultiListPtr list = mGUI->findWidget<MyGUI::MultiList>("Servers/List");
			const Client::ServerInfoMap& servs = mClient->getServerList();
			if (list->getItemCount() != servs.size()) {
				list->deleteAllItems();
				for (Client::ServerInfoMap::const_iterator iserv = servs.begin(); iserv != servs.end(); iserv++) {
					list->addItem(iserv->first);
					list->setSubItem(1, list->getItemCount()-1, iserv->second.mapName);
					list->setSubItem(2, list->getItemCount()-1, Ogre::StringConverter::toString(iserv->second.numPlayers) + "/" + Ogre::StringConverter::toString(iserv->second.maxPlayers));
				}
			}
		}

		mGUI->injectFrameEntered(evt.timeSinceLastFrame);

		return true;
	}

	bool keyPressed(const OIS::KeyEvent &arg)
	{
		switch (arg.key) {
		case OIS::KC_ESCAPE:
			mExit = true;
			break;
		case OIS::KC_GRAVE:
			break;
		default:
			break;
		}

		mGUI->injectKeyPress(arg);
		return true;
	}

	bool keyReleased(const OIS::KeyEvent &arg)
	{
		mGUI->injectKeyRelease(arg);
		return true;
	}

	bool mouseMoved(const OIS::MouseEvent &arg)
	{
		mCameraAngleH -= Ogre::Degree((Ogre::Real)arg.state.X.rel);
		mCameraAngleV -= Ogre::Degree((Ogre::Real)arg.state.Y.rel);
		mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
		mCamera->yaw(mCameraAngleH*0.1);
		mCamera->pitch(mCameraAngleV*0.1);

		mGUI->injectMouseMove(arg);
		return true;
	}

	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		mGUI->injectMousePress(arg, id);
		return true;
	}

	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		mGUI->injectMouseRelease(arg, id);
		return true;
	}

    bool initialise()
    {
		mRoot = new Ogre::Root(PLUGINS_CFG, OGRE_CFG, OGRE_LOG);

		if (!mRoot->restoreConfig())
			if (!mRoot->showConfigDialog())
				return false;

		initResources();

        mWindow = mRoot->initialise(true, "CS Clone v"CSCLONE_VERSION);
        Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
		mCamera = mSceneMgr->createCamera("camera");
        mWindow->addViewport(mCamera);
        mCamera->setAutoAspectRatio(true);
        mCamera->setNearClipDistance(0.1);
        mCamera->setFarClipDistance(10000);
        mCamera->setPosition(10, 10, 10);
//        mCamera->lookAt(0, 0, 0);

        mRoot->addFrameListener(this);

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
//Initializing OIS
		Ogre::LogManager::getSingletonPtr()->logMessage("*-*-* OIS Initialising");

		OIS::ParamList pl;
		size_t windowHnd = 0;
		mWindow->getCustomAttribute("WINDOW", &windowHnd);
		pl.insert(std::make_pair(std::string("WINDOW"), Ogre::StringConverter::toString(windowHnd)));

#if OGRE_DEBUG_MODE == 1
	#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		#define NO_EXCLUSIVE_INPUT
	#endif
#endif

#ifdef NO_EXCLUSIVE_INPUT
	#if defined OIS_WIN32_PLATFORM
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	#elif defined OIS_LINUX_PLATFORM
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
	#endif
#endif
		mInputManager = OIS::InputManager::createInputSystem(pl);

		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mKeyboard->setEventCallback(this);
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
		mMouse->setEventCallback(this);

		windowResized(mWindow);
//Initializing GUI
		Ogre::LogManager::getSingletonPtr()->logMessage("*-*-* MyGUI Initialising");
		mGUI = new MyGUI::Gui;
		mGUI->initialise(mWindow, "csclone.gui");

		centerWidget(mGUI->findWidgetT("Create"));
		centerWidget(mGUI->findWidgetT("Servers"));
		centerWidget(mGUI->findWidgetT("Options"));
		centerWidget(mGUI->findWidgetT("Quit"));

		MyGUI::MultiListPtr list = mGUI->findWidget<MyGUI::MultiList>("Servers/List");
		list->addColumn(175, "Name");
		list->addColumn(85, "Map");
		list->addColumn(70, "Players");
		list->addColumn(55, "Latency");
		//main menu
		MyGUI::ButtonPtr btn;
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-50, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Exit");
		btn->setCaption("Exit");
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyExit);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-90, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Options");
		btn->setCaption("Options");
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyOptions);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-110, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Find");
		btn->setCaption("Find Servers");
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyFindServers);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-130, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/New");
		btn->setCaption("New Game");
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyNewGame);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-170, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Players");
		btn->setCaption("Player list");
		btn->hide();
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyPlayerList);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-190, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Disconnect");
		btn->setCaption("Disconnect");
		btn->hide();
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyDisconnect);
		btn = mGUI->createWidget<MyGUI::Button>("MainMenuText",
			MyGUI::IntCoord(20, mGUI->getViewHeight()-210, 100, 24),
			MyGUI::ALIGN_DEFAULT, "Back", "MainMenu/Resume");
		btn->setCaption("Resume game");
		btn->hide();
		btn->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyResumeGame);
		//servers dialog
		mGUI->findWidget<MyGUI::Window>("Servers")->eventWindowButtonPressed = MyGUI::newDelegate(this, &Application::notifyServersClose);
		mGUI->findWidget<MyGUI::Button>("Servers/Connect")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyServersConnect);
		//options dialog
		mGUI->findWidget<MyGUI::Button>("Options/OK")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyOptionsOK);
		mGUI->findWidget<MyGUI::Button>("Options/Cancel")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyOptionsCancel);
		mGUI->findWidget<MyGUI::Button>("Options/Apply")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyOptionsApply);
		//create dialog
		//mGUI->findWidget<MyGUI::Window>("Create/Cancel")->eventWindowButtonPressed = MyGUI::newDelegate(this, &Application::notifyCreateCancel);
		//quit dialog
		mGUI->findWidget<MyGUI::Button>("Quit/Quit")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyQuitQuit);
		mGUI->findWidget<MyGUI::Button>("Quit/Cancel")->eventMouseButtonClick = MyGUI::newDelegate(this, &Application::notifyQuitCancel);
//Initializing Game
		Socket::initialise();
		Ogre::LogManager::getSingletonPtr()->logMessage("*-*-* Initialising Game ***");
		Ogre::LogManager::getSingletonPtr()->logMessage("*-*-* CS Clone v"CSCLONE_VERSION);
		mServer = new Server("testServer");
		mServer->loadMap("test.map");
		mClient = new Client(new WorldManagerClient(mSceneMgr));
		Ogre::LogManager::getSingletonPtr()->logMessage("*** Game initialised ***");

		return true;
    }

	void executeCommand(const Ogre::String& command)
	{
		std::vector<Ogre::String> params;
		params = Ogre::StringUtil::split(command, " ");

		if (params[0] == "map") {
			mServer->loadMap(params[1]);
			return;
		}

		if (params[0] == "exit") {
			mExit = true;
			return;
		}

		Ogre::LogManager::getSingleton().logMessage("CS Clone: Unknown command: " + params[0]);
	}

	void run()
	{
		mRoot->startRendering();
	}
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
	Application app;

	try {
		if (!app.initialise())
			return 0;
		//command line parser
		Ogre::String command;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		app.parseCommandLine(lpCmdLine);
#else
		app.commandLine(argc, argv);
#endif
		app.run();
	} catch(Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK|MB_ICONERROR|MB_TASKMODAL);
#else
        std::cerr << "An exception has occurred: " << e.getFullDescription();
#endif
        return 1;
	} catch(OIS::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.eText, "An exception has occurred!", MB_OK|MB_ICONERROR|MB_TASKMODAL);
#else
		std::cerr << "An exception has occurred: " << e.eText << " at "
			<< e.eFile << "(" << e.eLine << ")";
#endif
		return 1;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

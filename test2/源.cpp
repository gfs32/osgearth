#include "../Common/Common.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/ViewerEventHandlers>
#include <iostream>
#ifdef _DEBUG
#pragma comment(lib,"../Relase/Common.lib")
#endif // _DEBUG
/*

class PrintName :public osgGA::GUIEventHandler
{
public:
	virtual void getUsage(osg::ApplicationUsage& usage) const
	{
		usage.addKeyboardMouseBinding("gfs", "aaa");
	}
};

int main(int argc, char **argv)

{

	osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer;
	osg::ref_ptr<osg::Node> node = new osg::Node;
	// osg::ref_ptr<osg::Timer> timer = new osg::Timer;
	osg::Timer* timer = new osg::Timer;
	osg::Timer_t start_timer = 0;
	osg::Timer_t end_timer = 0;
	std::cout << timer->getSecondsPerTick() << std::endl;
	//start_timer = timer->tick();
	start_timer = viewer->elapsedTime();
	timer->setStartTick();
	node = osgDB::readNodeFile("glider.osg");
	std::cout << "时间" << timer->time_s() <<std::endl;
	//end_timer = timer->tick();
	end_timer = viewer->elapsedTime();
	std::cout << "时间" << osg::Timer::instance()->delta_s(start_timer, end_timer) << std::endl;

	viewer->addEventHandler(new osgViewer::HelpHandler);
	viewer->addEventHandler(new PrintName);
	viewer->addEventHandler(new osgViewer::WindowSizeHandler);
	viewer->addEventHandler(new osgViewer::ScreenCaptureHandler);
	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->addEventHandler(new osgViewer::RecordCameraPathHandler);


    viewer->setSceneData(node.get());
	viewer->realize();
	return viewer->run();

}
*/
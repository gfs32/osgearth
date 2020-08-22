#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
/*

int main()
{
	osgViewer::Viewer viewer;
	viewer.setSceneData(osgDB::readNodeFile("cow.osg"));
	//viewer.run();
	viewer.getCamera()->setProjectionMatrixAsPerspective(40., 1., 1., 100.);
	osg::Matrix trans;
	trans.makeTranslate(0., 0., -12.);
	double angle(0.);
	while (!viewer.done())
	{
		osg::Matrix rot;
		rot.makeRotate(angle, osg::Vec3(1., 0., 0.));
		angle += 0.01;
		viewer.getCamera()->setViewMatrix(rot*trans);
		viewer.frame();
	}
	return 0;
}


*/
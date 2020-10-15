#include "ViewControl.h"
#include <osgGA/CameraManipulator>
#include <osgGA/GUIActionAdapter>


void ViewSelect::moveToView(const osgGA::GUIEventAdapter & ea)
{
	_thrown = false;

	home(0.0);                 //this fuction does not trigger redraw
	switch (ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Down:                            //TOP_VIEW :
		this->setRotation(osg::Quat(1.0f, 0.0, 0.0, 0.0));
		break;
	case osgGA::GUIEventAdapter::KEY_Up:                         //BOTTOM_VIEW :
		this->setRotation(osg::Quat(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case osgGA::GUIEventAdapter::KEY_Left:                         // LEFT_VIEW :
		this->setRotation(osg::Quat(1.0f, -1.0, -1.0, 1.0f));
		break;
	case osgGA::GUIEventAdapter::KEY_Right:                          //RIGHT_VIEW:
		this->setRotation(osg::Quat(1.0f, 1.0f, 1.0f, 1.0f));
		break;
		//case osgGA::GUIEventAdapter::KEY_Down:                          // BACK_VIEW:
			// this->setRotation(osg::Quat(0.0f,1.0f,1.0f,0.0f));
		break;
	default:
		break;
	}

	ea.setHandled(true);           //been handled
}


bool ViewSelect::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{

	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::KEYDOWN:
		moveToView(ea);
		break;
	default:
		break;
	}

	if (ea.getHandled())
		return false;

	return   osgGA::StandardManipulator::handle(ea, us);

}


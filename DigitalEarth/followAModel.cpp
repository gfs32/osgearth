#include "followAModel.h"

bool FollowUpdater::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = static_cast<osgViewer::View*>(&aa);
	if (!view || !_target || ea.getEventType() != osgGA::GUIEventAdapter::FRAME)
		return false;
	osgGA::OrbitManipulator* orbit = dynamic_cast<osgGA::OrbitManipulator*>(view->getCameraManipulator());
	if (orbit)
	{
		osg::Matrix matrix = computeTargetToWorldMatrix(_target.get());
		osg::Vec3d targetCenter = _target->getBound().center() *matrix;
		orbit->setCenter(targetCenter);
	}
	//osgGA::FirstPersonManipulator* first = dynamic_cast<osgGA::FirstPersonManipulator*>(view->getCameraManipulator());
	//if (first)
	//{
	//	osg::Matrix matrix = computeTargetToWorldMatrix(_target.get());
	//	osg::Vec3d targetCenter = _target->getBound().center() *matrix;
	//	//first->setCenter(targetCenter);
	//	first->setHomePosition(targetCenter);
	//}
	return false;
}

osg::Matrix FollowUpdater::computeTargetToWorldMatrix(osg::Node* node) const
{
	osg::Matrix l2w;
	if (node && node->getNumParents() > 0)
	{
		osg::Group* parent = node->getParent(0);
		l2w = osg::computeLocalToWorld(parent->getParentalNodePaths()[0]);
	}
	return l2w;
}
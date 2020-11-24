#include <osg/Camera>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/OrbitManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgViewer/Viewer>

class FollowUpdater : public osgGA::GUIEventHandler
{
public:
	FollowUpdater(osg::Node* node) : _target(node) {}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	osg::Matrix computeTargetToWorldMatrix(osg::Node* node) const;

protected:
	osg::observer_ptr<osg::Node> _target;
};
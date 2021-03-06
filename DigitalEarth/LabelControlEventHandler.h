#pragma once

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgUtil/LineSegmentIntersector>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>
#include <osgEarthUtil/LatLongFormatter>

class CLabelControlEventHandler :public osgGA::GUIEventHandler
{
public:
	CLabelControlEventHandler(
		osgEarth::Util::Controls::LabelControl* viewLabel,
		osgEarth::Util::Controls::LabelControl* mouseLabel,
		osgEarth::MapNode* mn);
	~CLabelControlEventHandler(void);
	bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&);

private:
	osgEarth::Util::Controls::LabelControl* viewCoords;
	osgEarth::Util::Controls::LabelControl* mouseCoords;
	osgEarth::MapNode* mapNode;
	osg::NodePath nodePath;
};


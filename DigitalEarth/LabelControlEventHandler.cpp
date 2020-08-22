
#include "LabelControlEventHandler.h"

#include <osgEarth/TerrainEngineNode>

#include <osgEarth/Terrain>

#include <osgEarth/GeoData>


CLabelControlEventHandler::CLabelControlEventHandler(osgEarth::Util::Controls::LabelControl* viewLabel,
	osgEarth::Util::Controls::LabelControl* mouseLabel,
	osgEarth::MapNode* mn)
{
	viewCoords = viewLabel;
	mouseCoords = mouseLabel;
	mapNode = mn;
	nodePath.push_back(mapNode->getTerrainEngine());
}
CLabelControlEventHandler::~CLabelControlEventHandler(void)
{
}



bool CLabelControlEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (viewer)
	{
		if (ea.getEventType()==ea.FRAME)
		{
			osgUtil::LineSegmentIntersector::Intersections results;
			if (viewer->computeIntersections(ea.getX(), ea.getY(), nodePath, results))
			{
				osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
				//取出交点坐标(osg的x,y,z坐标)
				osg::Vec3d point = first.getWorldIntersectPoint();
				//osg::Vec3d lla;
				double x;
				double y;
				double z;

				////转化为地球的地理坐标
				//osgEarth::GeoPoint mapPoint;
				//mapPoint.fromWorld(mapNode->getTerrain()->getSRS(), point);

				//屏幕坐标转经纬度
				mapNode->getMapSRS()->getEllipsoid()->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),x,y,z);
				char wsrc[512];
				sprintf(wsrc, "Lon:%.2f Lat:%.2f Hei:%.2f", x,y,z);
				mouseCoords->setText(wsrc);
			}
			//视点坐标
			osgEarth::Util::EarthManipulator* em = dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());

			if (em)
			{
				osgEarth::Viewpoint vp = em->getViewpoint();
				char wsrc[512];
				sprintf(wsrc, "Lon:%.2f Lat:%.2f Hei:%.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());
				//sprintf(wsrc, "(%.2f, %.2f, %.2f), %.2f, %.2f, %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.focalPoint().get().z(), vp.getHeading(), vp.getPitch(), vp.getRange());
				viewCoords->setText(wsrc);
			}
		}
		if (ea.getEventType() == ea.KEYDOWN)
		{
			/*osgEarth::Util::EarthManipulator * em = dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
			osgEarth::Viewpoint vm = em ->getViewpoint();
			double fx = vm.focalPoint().get().x();
			double fy = vm.focalPoint().get().y();
			double fz = vm.focalPoint().get().z();
			double heading_deg = vm.getHeading();
			double pitch_deg = vm.getPitch();
			double range = vm.getRange();


			if(ea.getKey() == 'P')
			{
			    fx ++;
			}
			else if(ea.getKey() == 'p')
			{
			    fx --;
			}
			else if(ea.getKey() == 'O')
			{
			    fy ++;
			}
			else if(ea.getKey() == 'o')
			{
			    fy ++;
			}
			else if(ea.getKey() == 'I')
			{
			    fz ++;
			}
			else if(ea.getKey() == 'i')
			{
			    fz --;
			}
			else if(ea.getKey() == 'U')
			{
			    heading_deg ++;
			}
			else if(ea.getKey() == 'u')
			{
			    heading_deg --;
			}
			else if(ea.getKey() == 'Y')
			{
			    pitch_deg ++;
			}
			else if(ea.getKey() == 'y')
			{
			    pitch_deg --;
			}
			else if(ea.getKey() == 'T')
			{
			    range += 1000;
			}
			else if(ea.getKey() == 't')
			{
			    range -= 1000;
			}

			em->setViewpoint(osgEarth::Viewpoint("qre",fx, fy, fz, heading_deg, pitch_deg, range));
		*/}
	}
	return false;
}
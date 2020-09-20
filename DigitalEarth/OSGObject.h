#pragma once

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/EarthManipulator>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osgEarth/ImageLayer>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthUtil/Controls>
#include <osgEarth/GeoData>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/Shape>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/Notify>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthAnnotation/FeatureNode>
#include <osgViewer/CompositeViewer>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/mbtiles/MBTilesOptions>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgUtil/SmoothingVisitor>
#include <osg/LineWidth>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#include <osgGA/NodeTrackerManipulator>
#include "LabelControlEventHandler.h"

using namespace std;
using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Annotation;
using namespace osgEarth::Drivers;

class OSGObject
{
public:
	OSGObject(HWND hwnd);
	~OSGObject(void);

	void InitOSG();
	void InitSceneGraph();
	void InitCameraConfig();
	void PreFrameUpdate();
	void PostFrameUpdate();
	static void Render(void* ptr);
	void InitOsgEarth();

	void setChinaBoundariesOpacity(double opt);
	double getChinaBoundariesOpacity();

	void rmvChinaBoundaryes();
	void addChinaBoundaryes();

	//新增地标
	void addLabel();

	//添加显示视点信息的控件
	void addViewPointLabel();

	//坐标控件
	CLabelControlEventHandler * labelEvent;

	//飞往
	void FlyTo(double lon, double lat, double hei);

	//地表的矩阵操作
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;

public:

	osg::ref_ptr<osg::MatrixTransform> mtAirport;
	
	void addAirport();

	osg::ref_ptr<osg::Node> flyAirport;
	//osg::ref_ptr<osgEarth::Annotation::PlaceNode> flyAirport;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtfly;
	//osg::ref_ptr<osg::Vec4Array> vaTemp;
	
	//osg::ref_ptr<osg::Vec3dArray> vertices1;

public:
	HWND m_hwnd;
	//OSGObject* mOSG;
	osgViewer::Viewer *getViewer();
	osgViewer::Viewer *mViewer;
	osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osg::MatrixTransform> maxax;
	osg::ref_ptr<osgEarth::MapNode> mapNode;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> em;
	osg::ref_ptr < osgGA::NodeTrackerManipulator> tm;
	
	osg::ref_ptr<osg::Node>  mp;
	
	//省界线图层
	osg::ref_ptr<osgEarth::ImageLayer> china_boundaries;

	//地标
	osg::ref_ptr<osg::Group> earthLabel;

public:

	//根据输入的控制点，输出一个路径，控制点格式为（经，纬，高，速）
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);

	//两点间的距离公式
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//计算两点间需要行驶的时间
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//设置一个预设置路径
	void DoAPreLine();

	//生成的预设置路径
	osg::ref_ptr<osg::AnimationPath> apc;

	//启动预设置路径
	void DoPreLineNow();

	//是否跟踪飞行器
	void isTrackFly(bool btrack);

	//链接数据库
	void connectMysql(); 

	//自动化入库
	void automation();
	
	//轨道
	static void OrbitsThread(void * ptr );

	//轨迹
	static void flyThread(void * ptr);

};
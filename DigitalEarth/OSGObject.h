#pragma once

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Sky>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osgEarth/ImageLayer>
#include <osg/ShapeDrawable>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgEarthUtil/EarthManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgEarthAnnotation/PlaceNode>
#include <osg/LineWidth>
#include <osg/PolygonMode>
//#include <osgEarthUtil/LogarithmicDepthBuffer>

#include "LabelControlEventHandler.h"
#include "KeyHandler.h"
//#include "TransformAccumulator.h"
//#include "followAModel.h"
 
using namespace osgEarth::Util;

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


public:
	
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> m_pLakeFeatureNode;

public:

	void addAirport();
	void drawLink();
	void hideLine();
	void missileStart();
	void firstView();

	osg::ref_ptr<osg::Group> flyAirport1;
	osg::ref_ptr<osg::Group> flyAirport3;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf1;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf2;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf3;
	osg::ref_ptr<osg::MatrixTransform> mtfly;
	osg::ref_ptr<osg::MatrixTransform> mtfly1;
	osg::ref_ptr<osg::MatrixTransform> mtfly2;
	osg::ref_ptr<osg::MatrixTransform> mtfly3;
	osg::ref_ptr<osg::AnimationPathCallback> apcFly;
	osg::ref_ptr<osg::AnimationPathCallback> apcFly1;
	osg::ref_ptr<osg::AnimationPathCallback> apcFly2;

public:
	HWND m_hwnd;
	//OSGObject* mOSG;
	osgViewer::Viewer *getViewer();
	osgViewer::Viewer *mViewer;
	osg::ref_ptr<osg::Group> mRoot;
	//osg::ref_ptr<osg::MatrixTransform> maxax;
	//osg::ref_ptr<osgEarth::MapNode> mapNode;
	
	//osg::ref_ptr < osgGA::NodeTrackerManipulator> tm;

	osg::ref_ptr<osg::Node>  mp;

	//省界线图层
	osg::ref_ptr<osgEarth::ImageLayer> china_boundaries;

	//地标
	osg::ref_ptr<osg::Group> earthLabel;

public:

	//根据输入的控制点，输出一个路径，控制点格式为（经，纬，高，速）
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);
	//根据输入的控制点，输出一个路径，控制点格式为（x,y,h,t）
	osg::AnimationPath* CreateAirLinePath1(osg::Vec4Array* ctrl);
	//根据输入的控制点，输出一个路径，控制点格式为（经，纬，高,t）
	osg::AnimationPath* CreateAirLinePath2(osg::Vec4Array* ctrl);

	//两点间的距离公式
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//计算两点间需要行驶的时间
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//设置一个预设置路径
	void DoAPreLine();

	void pause();

	void again();

	//生成的预设置路径
	osg::ref_ptr<osg::AnimationPath> ap;
	osg::ref_ptr<osg::AnimationPath> ap1;

	//启动预设置路径
	void DoPreLineNow(bool isTart);

	//是否跟踪飞行器
	void isTrackFly(bool btrack);

	//链接数据库
	void connectMysql();
	void connectMysql1();

	//自动化入库
	void automation();

	//轨道
	static void OrbitsThread(void * ptr);

	//轨迹
	static void flyThread(void * ptr);

};
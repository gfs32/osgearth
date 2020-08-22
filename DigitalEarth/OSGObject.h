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

#include <osgEarthUtil/Controls>

#include "LabelControlEventHandler.h"

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

	osgViewer::Viewer *getViewer();

	void setChinaBoundariesOpacity(double opt);
	double getChinaBoundariesOpacity();

	void rmvChinaBoundaryes();
	void addChinaBoundaryes();

	//添加显示视点信息的控件
	void addViewPointLabel();

	CLabelControlEventHandler *labelEvent;

	//飞往
	void FlyTo(double lon, double lat, double hei);

	//地表的矩阵操作
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;

public:
	osg::ref_ptr<osg::Node> airport;
	osg::ref_ptr<osg::MatrixTransform> mtAirport;

	//设置机场
	void addAirport();

	osg::ref_ptr<osg::Node> flyAirport;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtfly;


private:
	HWND m_hwnd;
	osgViewer::Viewer * mViewer;
	osg::ref_ptr<osg::Group> mRoot;
	
	osg::ref_ptr<osgEarth::MapNode> mapNode;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> em;

	osg::ref_ptr<osg::Node>  mp;
	 
	osg::ref_ptr<osgEarth::ImageLayer> china_boundaries;



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
	void isTrackFly(bool btrack);

};
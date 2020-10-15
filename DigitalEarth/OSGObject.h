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
#include "KeyHandler.h"
#include "TransformAccumulator.h"

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

	//void toggleTankOrbiterView();

	//�����ر�
	void addLabel();

	//�����ʾ�ӵ���Ϣ�Ŀؼ�
	void addViewPointLabel();

	//����ؼ�
	CLabelControlEventHandler * labelEvent;

	//����
	void FlyTo(double lon, double lat, double hei);

	//�ر�ľ������
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;

	//osg::ref_ptr<osg::CoordinateSystemNode> csn1;

	osg::ref_ptr<osgEarth::Annotation::FeatureNode> m_pLakeFeatureNode;

public:

	//bool isPathStart;
	//osg::ref_ptr<osg::MatrixTransform> mtAirport;

	void addAirport();
	void drawLink();
	void hideLine();
	void upView();
	void downView();
	void leftView();
	void rightView();
	void backView();
	osg::ref_ptr<osg::Group> flyAirport;
	osg::ref_ptr<osg::Group> flyAirport1;
	//osg::ref_ptr<osgEarth::Annotation::PlaceNode> flyAirport;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf1;
	//osg::ref_ptr<osg::PositionAttitudeTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtfly;
	osg::ref_ptr<osg::MatrixTransform> mtfly1;
	//osg::ref_ptr<osg::PositionAttitudeTransform> mtfly;
	//osg::ref_ptr<osg::Vec4Array> vaTemp;
	osg::ref_ptr<osg::AnimationPathCallback> apcFly;
	//osg::ref_ptr<osg::Vec3dArray> vertices1;

	//osg::Matrixd* getWorldCoords(osg::Node* node);

public:
	HWND m_hwnd;
	//OSGObject* mOSG;
	osgViewer::Viewer *getViewer();
	osgViewer::Viewer *mViewer;
	osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osg::MatrixTransform> maxax;
	//osg::ref_ptr<osgEarth::MapNode> mapNode;
	
	osg::ref_ptr < osgGA::NodeTrackerManipulator> tm;

	osg::ref_ptr<osg::Node>  mp;

	//ʡ����ͼ��
	osg::ref_ptr<osgEarth::ImageLayer> china_boundaries;

	//�ر�
	osg::ref_ptr<osg::Group> earthLabel;

public:

	//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ���ߣ��٣�
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);
	//osg::AnimationPath* CreateAirLinePath1(osg::Vec4Array* ctrl);
	//�����ľ��빫ʽ
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//�����������Ҫ��ʻ��ʱ��
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//����һ��Ԥ����·��
	void DoAPreLine();

	//���ɵ�Ԥ����·��
	//osg::ref_ptr<osg::AnimationPath> apc;

	//����Ԥ����·��
	void DoPreLineNow(bool isTart);

	//�Ƿ���ٷ�����
	void isTrackFly(bool btrack);

	//�������ݿ�
	void connectMysql();

	//�Զ������
	void automation();

	//���
	static void OrbitsThread(void * ptr);

	//�켣
	static void flyThread(void * ptr);

};
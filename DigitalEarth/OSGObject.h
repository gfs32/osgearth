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

	//�����ر�
	void addLabel();

	//�����ʾ�ӵ���Ϣ�Ŀؼ�
	void addViewPointLabel();

	//����ؼ�
	CLabelControlEventHandler * labelEvent;

	//����
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

	//ʡ����ͼ��
	osg::ref_ptr<osgEarth::ImageLayer> china_boundaries;

	//�ر�
	osg::ref_ptr<osg::Group> earthLabel;

public:

	//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ���ߣ��٣�
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);
	//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ��x,y,h,t��
	osg::AnimationPath* CreateAirLinePath1(osg::Vec4Array* ctrl);
	//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ����,t��
	osg::AnimationPath* CreateAirLinePath2(osg::Vec4Array* ctrl);

	//�����ľ��빫ʽ
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//�����������Ҫ��ʻ��ʱ��
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//����һ��Ԥ����·��
	void DoAPreLine();

	void pause();

	void again();

	//���ɵ�Ԥ����·��
	osg::ref_ptr<osg::AnimationPath> ap;
	osg::ref_ptr<osg::AnimationPath> ap1;

	//����Ԥ����·��
	void DoPreLineNow(bool isTart);

	//�Ƿ���ٷ�����
	void isTrackFly(bool btrack);

	//�������ݿ�
	void connectMysql();
	void connectMysql1();

	//�Զ������
	void automation();

	//���
	static void OrbitsThread(void * ptr);

	//�켣
	static void flyThread(void * ptr);

};
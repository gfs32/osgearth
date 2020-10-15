#include "pch.h"
#include "OSGObject.h"
#include "DigitalEarth.h"
#include <stdio.h> 
#include <stdlib.h> 
#include<Windows.h>
#include<mysql.h>
#include<Python.h>
#pragma comment(lib, "libmysql.lib")

osg::ref_ptr<osg::Vec3dArray> vertices;
osg::ref_ptr<osg::Vec4Array> vaTemp;
osg::PositionAttitudeTransform * followerOffset = NULL;
transformAccumulator* tankFollowerWorldCoords=NULL;
osg::ref_ptr<osgEarth::MapNode> mapNode;


osg::ref_ptr<osgEarth::Util::EarthManipulator> em;

OSGObject::OSGObject(HWND hwnd)
{
	m_hwnd = hwnd;
	labelEvent = 0;
	vaTemp = new osg::Vec4Array();
	//mOSG = new OSGObject(m_hWnd);
	//vertices1 = new osg::Vec3dArray();
	//isPathStart = false;
}

OSGObject::~OSGObject(void)
{

}

void OSGObject::InitOSG()
{
	InitSceneGraph();
	InitCameraConfig();
	InitOsgEarth();
}

void OSGObject::InitSceneGraph()
{
	mRoot = new osg::Group;
	//maxax = new osg::MatrixTransform;
	mp = osgDB::readNodeFile("../../sample.earth");
	//maxax->addChild(mp.get());
	mapNode = dynamic_cast<osgEarth::MapNode*>(mp.get());
	mRoot->addChild(mapNode);

	//地球自转
	//maxax->setUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0, 0.0, 0.0), osg::Z_AXIS, 0.02));

	//地标初始化
	earthLabel = new osg::Group;
	mRoot->addChild(earthLabel);
}

void OSGObject::InitCameraConfig()
{
	RECT rect;
	mViewer = new osgViewer::Viewer;
	::GetWindowRect(m_hwnd, &rect);
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hwnd);
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right;
	traits->height = rect.bottom;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits);

	osg::ref_ptr<osg::Camera>  camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0, 1000.0);
	//设置渲染顺序，必须在最后渲染
	//camera->setRenderOrder(osg::Camera::POST_RENDER);
	//camera->setAllowEventFocus(true);
	//开启深度测试，模型不遮挡
	camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	//加上自动裁剪。否则显示会出现遮挡	
	mViewer->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(mapNode));
	mViewer->setCamera(camera);
	//mViewer->setCameraManipulator(new osgGA::TrackballManipulator);
	//mViewer->setCameraManipulator(new osgGA::NodeTrackerManipulator);
	mViewer->setSceneData(mRoot);
	mViewer->realize();
	mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
	mViewer->getCamera()->setNearFarRatio(0.000003f);
	//LogarithmicDepthBuffer buf;
	//buf.install(mViewer->getCamera());

}

bool useTankOrbiterView = false;

void toggleTankOrbiterView()
{
	if (!useTankOrbiterView)
		useTankOrbiterView = true;
	else
		useTankOrbiterView = false;
}

void OSGObject::PreFrameUpdate()
{
	while (theApp.bNeedModify) Sleep(1);
	theApp.bCanModify = FALSE;
}

void OSGObject::PostFrameUpdate()
{
	if (theApp.bNeedModify)
		theApp.bCanModify = TRUE;
}


osgViewer::Viewer *OSGObject::getViewer()
{
	return mViewer;
}

void OSGObject::Render(void * ptr)
{
	OSGObject *osg = (OSGObject*)ptr;
	osgViewer::Viewer * viewer = osg->getViewer();
	osgEarth::Viewpoint vp;	
	while (!viewer->done())
	{
		osg->PreFrameUpdate();
		if (useTankOrbiterView && (tankFollowerWorldCoords != NULL))
		{
			//aaa = getWorldCoords(followerOffset);*
			//osg::Matrix::Matrixd(1.0,1.0,1.0)
			
			em->setByInverseMatrix(tankFollowerWorldCoords->getMatrix()*osg::Matrix::rotate(-osg::PI_2, 1, 0, 0));			
			//em->setByMatrix(tankFollowerWorldCoords->getMatrix());
			vp.heading()->set(88.0, Units::DEGREES);
			//vp.focalPoint() = GeoPoint(mapNode->getMapSRS(), 104.1347, 33.3834, 0, ALTMODE_ABSOLUTE);
			vp.pitch()->set(-28.0, Units::DEGREES);	
			vp.positionOffset()->set(0, 0, 0);
			vp.range()->set(mapNode->getMapSRS()->getEllipsoid()->getRadiusEquator() / 15.0, Units::METERS);
			em->setViewpoint(vp);
		}
		viewer->frame();
		osg->PostFrameUpdate();
	}
	_endthread();
}

void OSGObject::InitOsgEarth()
{

	//初始化操作器
	em = new osgEarth::Util::EarthManipulator;
	//tm = new osgGA::NodeTrackerManipulator;
	if (mapNode.valid())
	{
		em->setNode(mapNode);//mapNode只能放一个模型
		//tm->setNode(mapNode);
	}
	em->getSettings()->setArcViewpointTransitions(true);

	mViewer->setCameraManipulator(em);

	//em->setRotation(osg::Quat(1.0f, 0.0, 0.0, 0.0));

	//mViewer->setCameraManipulator(tm);
	em->setHomeViewpoint(osgEarth::Viewpoint("", 102.1347, 31.3834, 444.02, 100.84, -20.01, 4028182.75), 2);
	em->setViewpoint(osgEarth::Viewpoint("", 98.27, 31.23, 444.02, -0.0, -89.01, 47112465.0), 2);

	//初始化天空
	osg::ref_ptr<osgEarth::Util::SkyNode> sky_node = osgEarth::Util::SkyNode::create(mapNode);
	//osgEarth::DateTime dateTime(2020, 7, 30, 8);
	osg::ref_ptr<osgEarth::Util::Ephemeris> ephemeris = new osgEarth::Util::Ephemeris;

	sky_node->setEphemeris(ephemeris);
	sky_node->setSunVisible(true);
	sky_node->setMoonVisible(true);
	sky_node->setStarsVisible(true);
	sky_node->setAtmosphereVisible(true);
	//sky_node->setDateTime(dateTime);
	sky_node->setLighting(true);
	sky_node->attach(mViewer, 1);
	mRoot->addChild(sky_node);

	//获取省界线图层
	china_boundaries = mapNode->getMap()->getImageLayerByName("china_boundaries");

	//新增地标
	addLabel();

	//自动化入库
	//automation();

	//链接数据库
	connectMysql();

	//设置卫星
	addAirport();

	//卫星 地面站 通信
	drawLink();

	//hideLine();

	//设置预设置路径
	DoAPreLine();

	//新增显示视点信息的控件
	addViewPointLabel();

}

void OSGObject::setChinaBoundariesOpacity(double opt)
{
	if (china_boundaries)
	{
		china_boundaries->setOpacity(opt);
	}
}

double OSGObject::getChinaBoundariesOpacity()
{
	if (china_boundaries)
	{
		return china_boundaries->getOpacity();
	}
	else
	{
		return -1.0;
	}
}

void OSGObject::rmvChinaBoundaryes()
{
	if (china_boundaries)
	{
		mapNode->getMap()->removeImageLayer(china_boundaries);
	}
}

void OSGObject::addChinaBoundaryes()
{
	if (china_boundaries)
	{
		mapNode->getMap()->addImageLayer(china_boundaries);
	}
}

void OSGObject::addViewPointLabel()
{
	osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::get(mViewer);
	//添加控件，用来显示视点信息
	osgEarth::Util::Controls::LabelControl* viewCoords = new osgEarth::Util::Controls::LabelControl("TestViewPoint", osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	viewCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
	viewCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_TOP);
	viewCoords->setMargin(10);
	canvas->addChild(viewCoords);
	//添加控件，用于显示鼠标交点信息
	osgEarth::Util::Controls::LabelControl* mouseCoords = new osgEarth::Util::Controls::LabelControl("FocusPoint", osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	mouseCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_RIGHT);
	mouseCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
	mouseCoords->setMargin(10);
	canvas->addControl(mouseCoords);
	if (labelEvent == 0)
	{
		labelEvent = new CLabelControlEventHandler(viewCoords, mouseCoords, mapNode);
	}
	//mRoot->addChild(canvas);
	mViewer->addEventHandler(labelEvent);
}

void OSGObject::FlyTo(double lon, double lat, double hei)
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	em->setViewpoint(osgEarth::Viewpoint("", lon, lat, 0, 0, -45, hei), 4.0);

	theApp.bNeedModify = FALSE;
}

class UpdateLink :public osg::NodeCallback
{
public:
	osg::Node *plane;
	osg::Node *car;
public:
	UpdateLink(osg::Node *p, osg::Node *c)
	{
		plane = p;
		car = c;
	}
	virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> m_pLakeFeatureNode1 = dynamic_cast<osgEarth::Annotation::FeatureNode*>(node);

		if (m_pLakeFeatureNode1.get())
		{
			std::vector<osg::Vec3> m_vecLakePoint;
			//获取car节点所在的世界坐标     
			osgEarth::Features::Feature* m_pLakeFeature = m_pLakeFeatureNode1->getFeature();
			//删除所有的geomertry对象
			osgEarth::Symbology::Style m_lineLakeStyle;
			osg::Vec3 center = car->getBound().center() * car->getWorldMatrices()[0];
			double lon, lat, height;
			//将世界坐标XYZ转换成经度、纬度、高度信息    
			osg::EllipsoidModel* emodel = new osg::EllipsoidModel();
			emodel->convertXYZToLatLongHeight(center.x(), center.y(), center.z(), lat, lon, height);
			//存储当前点   
			m_vecLakePoint.push_back(osg::Vec3(osg::RadiansToDegrees(lon), osg::RadiansToDegrees(lat), height));
			//获取plane节点所在的世界坐标     
			osg::Vec3 center1 = plane->getBound().center() * plane->getWorldMatrices()[0];
			double lon1, lat1, height1;
			//将世界坐标XYZ转换成经度、纬度、高度信息    
			emodel->convertXYZToLatLongHeight(center1.x(), center1.y(), center1.z(), lat1, lon1, height1);
			//存储当前点    
			m_vecLakePoint.push_back(osg::Vec3(osg::RadiansToDegrees(lon1), osg::RadiansToDegrees(lat1), height1));
			// 初始化绘图参数 
			m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color::Blue;
			m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4.0;
			m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20.0;
			m_lineLakeStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 0.1;
			m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->stipple() = 255;
			//添加所要连接的点的经纬度和高度     
			m_pLakeFeature->getGeometry()->clear();
			m_pLakeFeatureNode1->setStyle(m_lineLakeStyle);
			for (int i = 0; i < m_vecLakePoint.size(); ++i)
			{
				m_pLakeFeature->getGeometry()->push_back(m_vecLakePoint[i]);

			}
			m_pLakeFeatureNode1->init();
		}
		traverse(node, nv);
	}
};

void OSGObject::drawLink()
{ 
	//初始化绘图参数
	const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
	//获取car节点所在的世界坐标
	// 初始化绘图参数 
	osgEarth::Symbology::Style m_lineLakeStyle;
	m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color::Blue;
	m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4.0;
	m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20.0;
	m_lineLakeStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 0.1;
	m_lineLakeStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->stipple() = 255;
	{
		osgEarth::Features::Feature* m_pLakeFeature;
		std::vector<osg::Vec3> m_vecLakePoint;
		m_pLakeFeature = new osgEarth::Features::Feature(new osgEarth::Annotation::LineString, geoSRS, m_lineLakeStyle);
		m_pLakeFeatureNode = new osgEarth::Annotation::FeatureNode(mapNode, m_pLakeFeature);

		osg::Vec3 center = flyAirport1->getBound().center() * flyAirport1->getWorldMatrices()[0];  
		double lon, lat, height;
		//将世界坐标XYZ转换成经度、纬度、高度信息  
		csn->getEllipsoidModel()->convertXYZToLatLongHeight(center.x(), center.y(), center.z(), lat, lon, height);
		//存储当前点    
		m_vecLakePoint.push_back(osg::Vec3(osg::RadiansToDegrees(lon), osg::RadiansToDegrees(lat), height));
		//获取plane节点所在的世界坐标   
		osg::Vec3 center1 = flyAirport->getBound().center() * flyAirport->getWorldMatrices()[0];
		double lon1, lat1, height1;
		//将世界坐标XYZ转换成经度、纬度、高度信息   
		csn->getEllipsoidModel()->convertXYZToLatLongHeight(center1.x(), center1.y(), center1.z(), lat1, lon1, height1);
		//存储当前点   
		m_vecLakePoint.push_back(osg::Vec3(osg::RadiansToDegrees(lon1), osg::RadiansToDegrees(lat1), height1));
		//添加所要连接的点的经纬度和高度  
		m_pLakeFeature->getGeometry()->clear();
		m_pLakeFeatureNode->setStyle(m_lineLakeStyle);
		for (int i = 0; i < m_vecLakePoint.size(); ++i)
		{
			m_pLakeFeature->getGeometry()->push_back(m_vecLakePoint[i]);
		}
		m_pLakeFeatureNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		//关闭深度测试   
		m_pLakeFeatureNode->init();
		m_pLakeFeatureNode->addUpdateCallback(new UpdateLink(flyAirport, flyAirport1));
		mRoot->addChild(m_pLakeFeatureNode);
	}
}

void OSGObject::hideLine()
{
	mRoot->removeChild(m_pLakeFeatureNode);
}

void OSGObject::upView() 
{
	em->setRotation(osg::Quat(1.0f, 0.0, 0.0, 0.0));
}

void OSGObject::downView()
{
	em->setRotation(osg::Quat(0.0f, 0.0f, 0.0f, 1.0f));
}

void OSGObject::leftView()
{
	em->setRotation(osg::Quat(1.0f, -1.0, -1.0, 1.0f));
}

void OSGObject::rightView()
{
	em->setRotation(osg::Quat(1.0f, 1.0f, 1.0f, 1.0f));
}

void OSGObject::backView()
{
	em->setRotation(osg::Quat(0.0f, 1.0f, 1.0f, 0.0f));
}

void OSGObject::addAirport()
{

	csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());
	osg::Matrixd mtTemp;
	flyAirport = (osg::Group*)osgDB::readNodeFile("../../satellite.ive");
	flyAirport1 = (osg::Group*)osgDB::readNodeFile("../../Satellite_dish_Blender_Internal_render.3ds");
	mtfly1 = new osg::MatrixTransform;
	mtFlySelf1 = new osg::MatrixTransform;
	mtFlySelf1->addChild(flyAirport1);
	osg::ref_ptr<osg::StateSet> stt = mtfly1->getOrCreateStateSet();
	stt->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stt->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stt->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	mtfly1->setStateSet(stt);
	mtfly1->addChild(mtFlySelf1);
	mtFlySelf1->setMatrix(osg::Matrixd::scale(50000, 50000, 50000));
	mRoot->addChild(mtfly1);
	/*csn1 = new osg::CoordinateSystemNode;
	csn1->setEllipsoidModel(new osg::EllipsoidModel());*/
	osg::Matrixd mtTemp1;
	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(34.3834), osg::DegreesToRadians(104.1347), 2000, mtTemp1);
	mtfly1->setMatrix(mtTemp1);
	/*osgGA::NodeTrackerManipulator::TrackerMode trackerMode = osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION;
	osgGA::NodeTrackerManipulator::RotationMode rotationMode = osgGA::NodeTrackerManipulator::TRACKBALL;
	tm->setTrackerMode(trackerMode);
	tm->setRotationMode(rotationMode);
	tm->setTrackNode(flyAirport);*/

	/*osg::ref_ptr<osg::StateSet> st = flyAirport->getOrCreateStateSet();
	st->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	st->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	st->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	flyAirport->setStateSet(st);*/

	if (flyAirport.valid())
	{
		mtFlySelf = new osg::MatrixTransform;
		mtFlySelf->addChild(flyAirport);//0.0* osg::Matrixd::rotate(-osg::PI_2/2, osg::Vec3(0, 1, 0))
		mtFlySelf->setMatrix(osg::Matrixd::scale(15000, 15000, 15000));
		//模型缩放，法线随之
		/*osg::ref_ptr<osg::StateSet> st1 = mtFlySelf->getOrCreateStateSet();
		st1->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		mtFlySelf->setStateSet(st1);*/
		/*osg::ref_ptr<osg::StateSet> st = mtFlySelf->getOrCreateStateSet();
		st->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		st->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		st->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		mtFlySelf->setStateSet(st);*/
		/*mtFlySelf->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
		mtFlySelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);*/
		mtfly = new osg::MatrixTransform;
		mtfly->addChild(mtFlySelf);
		mRoot->addChild(mtfly);
		//mtfly->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		osg::ref_ptr<osg::StateSet> st = mtFlySelf->getOrCreateStateSet();
		st->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		st->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		st->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		mtfly->setStateSet(st);
		//osg::Vec3d center = mtfly->getBound().center(); //获得包围盒中心	
		//osg::Matrixd originPos = mtfly->getMatrix(); //获得当前矩阵		
		//mtfly->setMatrix(originPos*osg::Matrixd::translate(-center) //先将物体中心平移到世界坐标的原点 
		//	*osg::Matrixd::scale(800, 800, 800)//缩放 
		//	*osg::Matrixd::rotate(-1.57, osg::Vec3(0, 1, 0))//旋转
		//	*osg::Matrixd::translate(center));//变换后再将物体移回

		//第一视角
		followerOffset =new osg::PositionAttitudeTransform();
		/*
		double x1, y1, z1;
		osg::CoordinateSystemNode* csn1 = new osg::CoordinateSystemNode;
		csn1->setEllipsoidModel(new osg::EllipsoidModel());
		csn1->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(31.3834), osg::DegreesToRadians(102.1347), 300000.0, x1, y1, z1);
		*/
		//0.0不行-15.0垂直-30.0看海-60.0看云-50.0看北极-40.0北极-45.0蓝
		//osg::Quat rNeg90(osg::DegreesToRadians(-90.0), osg::Vec3(1, 0, 0));
		//osg::Quat rPos90(osg::DegreesToRadians(90.0), osg::Vec3(1, 0, 0));
		//rNeg90 * rPos90;
		//followerOffset->setAttitude(osg::Quat(osg::DegreesToRadians(-70.0), osg::Vec3( 1, 0,0)));
		//followerOffset->setPosition(osg::Vec3(x1, y1, z1));
		flyAirport.get()->addChild(followerOffset);		
		tankFollowerWorldCoords =new transformAccumulator();
		tankFollowerWorldCoords->attachToGroup(followerOffset);
		keyboardEventHandler* keh = new keyboardEventHandler();
		keh->addFunction('v', toggleTankOrbiterView);
		mViewer->addEventHandler(keh);

		////锥形波束、扇形雷达
		//osg::ref_ptr<osg::Cone> cone = new osg::Cone;
		//osg::ref_ptr<osg::ShapeDrawable>  shap = new osg::ShapeDrawable(cone);
		//osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		//geode->addChild(shap);
		//mtfly->addChild(geode);
		//cone->setHeight(100000);
		//cone->setRadius(100000);
		//cone->setCenter(osg::Vec3(0, 0, -120000));
		////设置圆锥透明效果
		//osg::ref_ptr<osg::StateSet> stateset1 = geode->getOrCreateStateSet();
		//stateset1->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		//stateset1->setMode(GL_BLEND, osg::StateAttribute::ON);
		//stateset1->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		//stateset1->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		//stateset1->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		//stateset1->setAttribute(polyMode);
		//geode->setStateSet(stateset1);
		////设置圆锥的颜色，第四个参数0.25表示不透明度，0表示完全透明，1表示完全不透明
		//shap->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.2));

		//锥形波束、扇形雷达
		osg::ref_ptr<osg::Cone> cone = new osg::Cone;
		osg::ref_ptr<osg::ShapeDrawable>  shap = new osg::ShapeDrawable(cone);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addChild(shap);
		mtfly->addChild(geode);
		cone->setHeight(100000);
		cone->setRadius(100000);
		cone->setCenter(osg::Vec3(0, 0, -130000));
		//设置圆锥透明效果
		osg::ref_ptr<osg::StateSet> stateset1 = geode->getOrCreateStateSet();
		stateset1->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		stateset1->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset1->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		stateset1->setAttribute(polyMode);
		//设置圆锥的颜色，第四个参数0.25表示不透明度，0表示完全透明，1表示完全不透明
		shap->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.2));

		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(31.3834), osg::DegreesToRadians(102.1347), 200000, mtTemp);
		mtfly->setMatrix(mtTemp);
	}
}

//根据输入的控制点，输出一个路径，控制点格式为（经，纬，高，速）
osg::AnimationPath* OSGObject::CreateAirLinePath(osg::Vec4Array* ctrl)
{
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double shuiPingAngle;
	double chuiZhiAngle;
	double time = 0;

	osg::Matrix matrix;
	osg::Quat _rotation;

	//当前点
	osg::Vec3d positionCur;
	//下一点
	osg::Vec3d positionNext;

	for (osg::Vec4Array::iterator iter = ctrl->begin(); iter != ctrl->end(); iter++)
	{
		osg::Vec4Array::iterator iter2 = iter;
		iter2++;

		//需要判断是不是已经到顶
		//iter2++
		if (iter2 == ctrl->end())
		{
			break;
		}

		double x, y, z;
		csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), x, y, z);
		positionCur = osg::Vec3(x, y, z);
		csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter2->y()), osg::DegreesToRadians(iter2->x()), iter2->z(), x, y, z);
		positionNext = osg::Vec3(x, y, z);

		//求出水平夹角
		if (iter->x() == iter2->x())
		{
			shuiPingAngle = osg::PI_2;
		}
		else
		{
			shuiPingAngle = atan((iter2->y() - iter->y()) / (iter2->x() - iter->x()));
			if (iter2->x() > iter->x())
			{
				shuiPingAngle += osg::PI;
			}
		}

		//求垂直夹角
		if (iter->z() == iter2->z())
		{
			chuiZhiAngle = 0;
		}
		else
		{
			if (0 == sqrt(pow(GetDis(positionCur, positionNext), 2)) - pow((iter2->z() - iter->z()), 2))
			{
				chuiZhiAngle = osg::PI_2;
			}
			else
			{
				chuiZhiAngle = atan((iter2->z() - iter->z()) / sqrt(pow(GetDis(positionCur, positionNext), 2)) - pow((iter2->z() - iter->z()), 2));
			}

			if (chuiZhiAngle >= osg::PI_2)
				chuiZhiAngle = osg::PI_2;
			if (chuiZhiAngle <= -osg::PI_2)
			{
				chuiZhiAngle = -osg::PI_2;
			}
		}

		//求飞机的变换矩阵
		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), matrix);
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), chuiZhiAngle , osg::Vec3(0.0, 1.0, 0.0), shuiPingAngle, osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);//+ osg::PI_2
		animationPath->insert(time, osg::AnimationPath::ControlPoint(positionCur, matrix.getRotate()));

		//把下一个点的时间求出来
		time += GetRunTime(positionCur, positionNext, iter2->w());
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(positionNext, matrix.getRotate()));
	return animationPath.release();
}

double OSGObject::GetDis(osg::Vec3 from, osg::Vec3 to)
{
	return sqrt((to.x() - from.x())*(to.x() - from.x()) + (to.y() - from.y())*(to.y() - from.y()) + (to.z() - from.z())*(to.z() - from.z()));
}

double  OSGObject::GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed)
{
	double dist = GetDis(from, to);
	if (speed == 0)
		return 100;
	return dist / speed;
}

void OSGObject::DoAPreLine()
{
	osg::ref_ptr<osg::Vec4Array> vertices5 = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec3dArray> vertices6 = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> vertices7 = new osg::Vec3dArray;
	const osgEarth::SpatialReference*mapsrs5 = mapNode->getMapSRS();
	osg::Vec3d  startWorld5;
	//102.1347, 31.3834
	for (double i = -180; i < 180; i += 10)
	{
		vertices7->push_back(osg::Vec3d(i, 31.3834, 150000.0));
		vertices5->push_back(osg::Vec4(i, 31.3834, 150000,10000));
	}
	// 将经纬度转换为坐标
	for (int i = 0; i < vertices7->size();i++)
	{
	osgEarth::GeoPoint map3(mapsrs5, vertices7->at(i).x(), vertices7->at(i).y(), vertices7->at(i).z(), osgEarth::ALTMODE_ABSOLUTE);
	map3.toWorld(startWorld5);
	vertices7->at(i)= startWorld5;
	//(*vertices6)[i] = startWorld5;
	}

	//以上是一个完整轨道
	osg::ref_ptr<osg::Vec4Array> vaTemp1 = new osg::Vec4Array;
	vaTemp1->push_back(osg::Vec4(102.1347, 31.3834, 150000, 10000));
	//vaTemp1->push_back(osg::Vec4(106.1174, 36.3686, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(119.8794, 39.1944, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(125.1302, 44.3941, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(136.9387, 59.9202, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(147.5066, 66.5163, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(159.1347, 87.3834, 150000, 10000));
	vaTemp1->push_back(osg::Vec4(102.1347, 31.3834, 150000, 10000));

	osg::Vec3 startline(102.1347, 31.3834, 150000);
	osg::Vec3 endline(119.8794, 39.1944, 150000);

	osg::ref_ptr<osg::Geode> geode_redOrbit = new osg::Geode;
	const osgEarth::SpatialReference*mapsrs = mapNode->getMapSRS();

	osg::ref_ptr<osg::Geometry> linesgeom = new osg::Geometry();
	osg::Vec3d  startWorld;
	osg::Vec3d endWorld;
	osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray(2);

	// 将经纬度转换为坐标
	osgEarth::GeoPoint map(mapsrs, startline[0], startline[1], startline[2], osgEarth::ALTMODE_ABSOLUTE);
	map.toWorld(startWorld);

	osgEarth::GeoPoint map1(mapsrs, endline[0], endline[1], endline[2], osgEarth::ALTMODE_ABSOLUTE);
	map1.toWorld(endWorld);

	(*vertices)[0] = startWorld;
	(*vertices)[1] = endWorld;

	//将创建的顶点数组传递给几何对象。
	/*
	linesgeom->setVertexArray(vertices);
	linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINES, 0, 2));
	*/
	linesgeom->setVertexArray(vertices7);
	linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_LOOP, 0, vertices7->size()));

	//设置颜色
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	linesgeom->setColorArray(colors);
	linesgeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));


	//设置法线。
	osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
	norms->push_back(osg::Vec3(0.0, -1.0, 0.0));
	//osgUtil::SmoothingVisitor::smooth(*(linesgeom.get()));//自动生成法线
    linesgeom->setNormalArray(norms);
    linesgeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

   //限制线宽
	osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;

	//设置线宽
	width->setWidth(3.0);
	geode_redOrbit->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);

	osg::StateSet* ss = linesgeom->getOrCreateStateSet();
	ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	linesgeom->setStateSet(ss);

	//打开透明度
	geode_redOrbit->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//将点几何添加到大地测量。
	geode_redOrbit->addDrawable(linesgeom);

	mRoot->addChild(geode_redOrbit);

	//apc = CreateAirLinePath(vaTemp1); 
	//apc = CreateAirLinePath(vertices5);
	apcFly = new osg::AnimationPathCallback(CreateAirLinePath(vertices5), 0, 1);
}

void OSGObject::DoPreLineNow(bool isTart)
{

	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);	
	if (isTart)
	{
		mtfly->setUpdateCallback(apcFly);
		//跟踪节点
		em->setTetherNode(flyAirport);
		apcFly->reset();
	}
	else 
	{
		mtfly->removeUpdateCallback(apcFly);
		em->setTetherNode(0);
		//AfxMessageBox(_T("stop"));
	}

	theApp.bNeedModify = FALSE;
}

void OSGObject::isTrackFly(bool btrack)
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	if (btrack)
	{
		em->setTetherNode(flyAirport);
	}
	else
	{
		em->setTetherNode(0);
	}

	theApp.bNeedModify = FALSE;
}

void OSGObject::addLabel()
{
	const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
	osgEarth::Style style;
	osgEarth::Symbology::TextSymbol * textStyle = style.getOrCreateSymbol<osgEarth::Symbology::TextSymbol>();
	textStyle->fill()->color() = osg::Vec4f(1.0, 1.0, 1.0, 1.0);
	//设置边框
	textStyle->halo()->color() = osg::Vec4f(0.0, 0.0, 0.0, 1.0);
	textStyle->font() = "simsun.ttc";
	textStyle->size() = 20.0;
	textStyle->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	osg::Image* china = osgDB::readImageFile("../../china.png");
	osgEarth::Annotation::PlaceNode *pn = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint(geoSRS, 105.1, 30.3), china, "China", style);
	earthLabel->addChild(pn);
}

void OSGObject::automation()
{
	Py_Initialize();              //初始化，创建一个Python虚拟环境
	if (Py_IsInitialized())
	{
		PyObject* pModule = NULL;
		PyObject* pFunc = NULL;
		pModule = PyImport_ImportModule("satellites-orbit-all-in-one-4");  //参数为Python脚本的文件名
		AfxMessageBox(_T("Python环境初始化成功!"));
		
	}
	else
	{
		AfxMessageBox(_T("Python环境初始化失败!"));
	}
	Py_Finalize();

}

void OSGObject::connectMysql()
{
	MYSQL mysql;
	mysql_init(&mysql);
	const char * i_query = "select name,x,y,z from orbit";
	MYSQL_RES * result = NULL; //保存结果集的
	MYSQL_ROW row = NULL; //代表的是结果集中的一行 
	if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "199686", "satellite", 3306, NULL, 0))
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
	{
		if (mysql_query(&mysql, i_query) != 0) //如果连接成功，则开始查询 .成功返回0
		{
			AfxMessageBox(_T("查询失败!"));
		}
		else
		{
			if ((result = mysql_store_result(&mysql)) == NULL) //保存查询的结果 
			{
				AfxMessageBox(_T("保存失败!"));
			}
			else
			{
				vertices = new osg::Vec3dArray();

				while ((row = mysql_fetch_row(result)) != NULL) //读取结果集中的数据，返回的是下一行。因为保存结果集时，当前的游标在第一行【之前】 
				{
					vertices->push_back(osg::Vec3d(atof(row[1]), atof(row[2]), atof(row[3])));
					//vaTemp->push_back(osg::Vec4f(atof(row[1]), atof(row[2]), atof(row[3]), 1000000));
				}
				//int n = vertices->size();			
				//CString temp_value = _T("");   //temp_value用来处理int值
				//temp_value.Format(_T("%d"), n);//固定格式
				//AfxMessageBox(temp_value);
			}
		}
		mysql_free_result(result); //释放结果集 
		mysql_close(&mysql); //关闭连接 
	}
}

//线程画轨道
void OSGObject::OrbitsThread(void * ptr)
{
	//OSGObject
	OSGObject* osg = (OSGObject*)ptr;
	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
	//设置线宽
	//osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
	//width->setWidth(2.0);
	//osg::StateSet* stateSet1 = geode1->getOrCreateStateSet();
	//stateSet1->setAttributeAndModes(width, osg::StateAttribute::ON);
	////打开透明度
	//stateSet1->setMode(GL_BLEND, osg::StateAttribute::ON);
	////关闭默认光照
	//stateSet1->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//stateSet1->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
	osg::ref_ptr<osg::Geometry> linesgeom;

	for (int i = 0; i < 10; i++)
	{
		linesgeom = new osg::Geometry();
		linesgeom->setVertexArray(vertices);
		/*osg::ref_ptr<osg::Vec4Array> vv = new osg::Vec4Array();
		vv->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
		linesgeom->setColorArray(vv.get());
		osg::StateSet* ss = linesgeom->getOrCreateStateSet();
		osg::Material* mat1 = new osg::Material();
		mat1->setColorMode(osg::Material::ColorMode::DIFFUSE);
		ss->setAttribute(mat1);
		ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);*/
		//AfxMessageBox(_T("线程执行中!"));
		linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, i * 361, 361));
		linesgeom->setColorArray(colors);
		osg::StateSet* ss = linesgeom->getOrCreateStateSet();
		ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		linesgeom->setStateSet(ss);
		//设置法线。
		osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
		norms->push_back(osg::Vec3(0.0, -1.0, 0.0));
		//osgUtil::SmoothingVisitor::smooth(*(linesgeom.get()));//自动生成法线
		linesgeom->setNormalArray(norms);
		linesgeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
		geode1->addDrawable(linesgeom);
		theApp.bNeedModify = TRUE;
		while (!theApp.bCanModify)Sleep(1);
		osg->mRoot->addChild(geode1);
		theApp.bNeedModify = FALSE;
	}
	//AfxMessageBox(_T("线程执行完毕!"));
	_endthread();
}

//线程展示飞行轨迹
void OSGObject::flyThread(void * ptr)
{
	OSGObject* osg1 = (OSGObject*)ptr;
	//当前点
	osg::Vec3d positionCur1;
	//下一点
	osg::Vec3d positionNext1;
	osg::ref_ptr<osg::Node> asatellite;
	asatellite = osgDB::readNodeFile("../../satellite.ive");
	osg::StateSet* ss1 = asatellite->getOrCreateStateSet();
	/*ss1->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ss1->setMode(GL_NORMALIZE, osg::StateAttribute::ON);*/
	ss1->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	ss1->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss1->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	
	osg::ref_ptr<osg::MatrixTransform> satelliteGroup;
	osg::ref_ptr<osg::MatrixTransform> satelliteGroup1;
	osg::ref_ptr<osg::AnimationPath> animationPath1;

	for (int i = 0; i < 10; i++)
	{
		//asatellite->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
		satelliteGroup1 = new osg::MatrixTransform;
		satelliteGroup = new osg::MatrixTransform;
		asatellite->setStateSet(ss1);
		satelliteGroup->addChild(asatellite);
		satelliteGroup->setMatrix(osg::Matrixd::scale(600000, 600000, 600000));
		/*osg::StateSet* ss2 = satelliteGroup->getOrCreateStateSet();
		ss2->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		satelliteGroup->setStateSet(ss2);*/
		satelliteGroup1->addChild(satelliteGroup);
		//AfxMessageBox(_T("xianchneg"));
		double time1 = 0, dist1 = 0;
		animationPath1 = new osg::AnimationPath;
		animationPath1->setLoopMode(osg::AnimationPath::NO_LOOPING);

		for (int j = i*361; j < ((i + 1) * 361 - 1); j++)
		{
			
			positionCur1 = osg::Vec3d(vertices->at(j).x(), vertices->at(j).y(), vertices->at(j).z());
			positionNext1 = osg::Vec3d(vertices->at(1 + j).x(), vertices->at(1 + j).y(), vertices->at(1 + j).z());
			animationPath1->insert(time1, osg::AnimationPath::ControlPoint(positionCur1));
			dist1 = sqrt((positionNext1.x() - positionCur1.x())*(positionNext1.x() - positionCur1.x()) + (positionNext1.y() - positionCur1.y())*(positionNext1.y() - positionCur1.y()) + (positionNext1.z() - positionCur1.z())*(positionNext1.z() - positionCur1.z()));
			time1 += dist1 / 600000.0;
		}
		animationPath1->insert(time1, osg::AnimationPath::ControlPoint(positionNext1));

		satelliteGroup1->setUpdateCallback(new osg::AnimationPathCallback(animationPath1.release(), 0.0, 1.0));
		//osg1->em->setTetherNode(satelliteGroup1);
		theApp.bNeedModify = TRUE;
		while (!theApp.bCanModify)Sleep(1);
		osg1->mRoot->addChild(satelliteGroup1);
		theApp.bNeedModify = FALSE;
	}
	_endthread();
}
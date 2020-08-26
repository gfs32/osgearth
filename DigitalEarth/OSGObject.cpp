#include "pch.h"
#include "OSGObject.h"
#include "DigitalEarth.h"

OSGObject::OSGObject(HWND hwnd)
{
	m_hwnd = hwnd;
	labelEvent = 0;
	vaTemp = new osg::Vec4Array;
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
	mp = osgDB::readNodeFile("../../sample.earth");
	mRoot->addChild(mp.get());
	mapNode = dynamic_cast<osgEarth::MapNode*>(mp.get());

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
	traits->width = rect.right ;
	traits->height = rect.bottom ; 
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits);

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0, 1000.0);
	//camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//设置渲染顺序，必须在最后渲染
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(true);
	//开启深度测试，模型不遮挡
	camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	mViewer->setCamera(camera);
	//mViewer->setCameraManipulator(new osgGA::TrackballManipulator);
	mViewer->setSceneData(mRoot);
	mViewer->realize();
	mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
	mViewer->getCamera()->setNearFarRatio(0.000003f);
	
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

void OSGObject::Render(void * ptr)
{
	OSGObject *osg = (OSGObject*)ptr;
	osgViewer::Viewer * viewer = osg->getViewer();
	while (!viewer->done())
	{
		osg->PreFrameUpdate();
		viewer->frame();
		osg->PostFrameUpdate();
	}
	_endthread();
}

osgViewer::Viewer *OSGObject::getViewer()
{
	return mViewer;
}

void OSGObject::InitOsgEarth()
{

	//初始化操作器
	em = new osgEarth::Util::EarthManipulator;
	if (mapNode.valid())
	{
		em->setNode(mapNode);//mapNode只能放一个模型
	}
	em->getSettings()->setArcViewpointTransitions(true);
	mViewer->setCameraManipulator(em);
	em->setViewpoint(osgEarth::Viewpoint("", 102.1347, 31.3834, 444.02, -15.84, -53.01, 4028182.75), 2);

	//初始化天空
	osg::ref_ptr<osgEarth::Util::SkyNode> sky_node = osgEarth::Util::SkyNode::create(mapNode);
	osgEarth::DateTime dateTime(2020, 7, 30, 8);
	osg::ref_ptr<osgEarth::Util::Ephemeris> ephemeris = new osgEarth::Util::Ephemeris;
	sky_node->setEphemeris(ephemeris);
	sky_node->setSunVisible(true);
	sky_node->setMoonVisible(true);
	sky_node->setStarsVisible(true);
	sky_node->setAtmosphereVisible(true);
	sky_node->setDateTime(dateTime);
	sky_node->setLighting(true);
	sky_node->attach(mViewer, 1);
	//sky_node->addChild(mapNode);
	mRoot->addChild(sky_node);

	//获取省界线图层
	china_boundaries = mapNode->getMap()->getImageLayerByName("china_boundaries");

	//新增地标
	addLabel();
	
	//地球自转
	//osg::ref_ptr<osg::MatrixTransform> max = new osg::MatrixTransform;
	////mp = osgDB::readNodeFile("../../sample.earth");
	//max->addChild(mp.get());
	//max->setUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0, 0.0, 0.0), osg::Z_AXIS, 0.2));
	//mRoot->addChild(max.get());

	//设置卫星
	addAirport();

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
	viewCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
	viewCoords->setSize(800, 500);
	viewCoords->setMargin(50);
	viewCoords->setPosition(80.0,80.0);
	viewCoords->setBackColor(0, 0, 0, 0.5);
	canvas->addChild(viewCoords);
	//添加控件，用于显示鼠标交点信息
	osgEarth::Util::Controls::LabelControl* mouseCoords = new osgEarth::Util::Controls::LabelControl("FocusPoint", osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	mouseCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_RIGHT);
	mouseCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_TOP);
	mouseCoords->setBackColor(0, 0, 0, 0.5);
	mouseCoords->setSize(400, 500);
	mouseCoords->setMargin(10);
	canvas->addControl(mouseCoords);
	canvas->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    //加上自动裁剪。否则显示会出现遮挡	
	mViewer->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(mapNode));
	if (labelEvent == 0)
	{}
		labelEvent = new CLabelControlEventHandler(viewCoords, mouseCoords, mapNode);
	
	mRoot->addChild(canvas);
	mViewer->addEventHandler(labelEvent);
	
}

void OSGObject::FlyTo(double lon, double lat, double hei)
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	em->setViewpoint(osgEarth::Viewpoint("",lon, lat, 0, 0, -45, hei),4.0);

	theApp.bNeedModify = FALSE;
}

void OSGObject::addAirport()
{
	csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());	
	osg::Matrixd mtTemp;
	flyAirport = osgDB::readNodeFile("../../satellite.ive");
	flyAirport->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	if (flyAirport.valid())
	{
		mtFlySelf = new osg::MatrixTransform;
		mtFlySelf->addChild(flyAirport);

		mtFlySelf->setMatrix(osg::Matrixd::scale(2000, 2000, 2000));

		//mtFlySelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

		mtfly = new osg::MatrixTransform;
		//mtfly->setMatrix(osg::Matrix::rotate(-1.57, osg::Vec3(0, 1, 0)));
		mtfly->addChild(mtFlySelf);
		mRoot->addChild(mtfly);

		osg::Vec3d center = mtfly->getBound().center(); //获得包围盒中心	
		osg::Matrixd originPos = mtfly->getMatrix(); //获得当前矩阵		
		mtfly->setMatrix(originPos*osg::Matrixd::translate(-center) //先将物体中心平移到世界坐标的原点 
			*osg::Matrixd::scale(800, 800, 800)//缩放 
			*osg::Matrixd::rotate(-1.57, osg::Vec3(0, 1, 0))//旋转
			*osg::Matrixd::translate(center));//变换后再将物体移回

		//卫星轨道
		osg::Vec3 startline(  102.1347,31.3834,150000);
		osg::Vec3 endline( 106.1174,36.3686, 150000);
		osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
		const osgEarth::SpatialReference *mapsrs = mapNode->getMapSRS();
		osg::ref_ptr<osg::Geometry> linesgeom = new osg::Geometry();
		osg::Vec3d  startWorld;
		osg::Vec3d endWorld;
		osg::Vec3dArray* vertices2 = new osg::Vec3dArray(2);
		osg::Vec3dArray* vertices = new osg::Vec3dArray(180);
		//osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray(6);
		for (double i = -180; i < 175; i += 5)
		{
			vertices->push_back(osg::Vec3d(i, 31.3834, 200000.0));
			vaTemp->push_back(osg::Vec4f(i, 31.3834, 200000.0,1000000));
		}
		vertices->push_back(osg::Vec3d(-180, 31.3834, 200000.0));
		//vaTemp->push_back(osg::Vec4f(-180, 31.3834, 200000.0, 1000000));
		// 将经纬度转换为坐标
		for (int i = 0; i < vertices->size();i++)
		{
		osgEarth::GeoPoint map(mapsrs, vertices->at(i).x(), vertices->at(i).y(),vertices->at(i).z(), osgEarth::ALTMODE_ABSOLUTE);
		map.toWorld(startWorld);
		(*vertices)[i] = startWorld;
		}		
		//两点之间的线段
		/*osgEarth::GeoPoint map(mapsrs, startline[0], startline[1], startline[2], osgEarth::ALTMODE_ABSOLUTE);
		map.toWorld(startWorld);
		osgEarth::GeoPoint map1(mapsrs, endline[0], endline[1], endline[2], osgEarth::ALTMODE_ABSOLUTE);
		map1.toWorld(endWorld);
		(*vertices2)[0] = startWorld;
		(*vertices2)[1] = endWorld;*/
		//将创建的顶点数组传递给几何对象。
		linesgeom->setVertexArray(vertices);
		//画线
		linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_LOOP, 0, vertices->size()));
		//设置颜色
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		linesgeom->setColorArray(colors);
		linesgeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
		//设置法线。
		osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
		norms->push_back(osg::Vec3(0.0, 0.0, 1.0));
		//osgUtil::SmoothingVisitor::smooth(*(linesgeom.get()));//自动生成法线
	    linesgeom->setNormalArray(norms);
	    linesgeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
		//设置线宽
		osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
		width->setWidth(3.0);
		geode1->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);
		//打开透明度
		geode1->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		//关闭默认光照
		geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//将点几何添加到大地测量。
		geode1->addDrawable(linesgeom);
		mRoot->addChild(geode1);

		//圆锥雷达
		osg::ref_ptr<osg::Cone> cone = new osg::Cone;
		osg::ref_ptr<osg::ShapeDrawable>  shap = new osg::ShapeDrawable(cone);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;		
		geode->addChild(shap);
		mtfly->addChild(geode);
		cone->setHeight(140000);
		cone->setRadius(80000);
		//shap->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.0f));
		cone->setCenter(osg::Vec3(0,0,-115000));
		//设置圆锥透明效果
		osg::ref_ptr<osg::StateSet> stateset1 = geode->getOrCreateStateSet();
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		stateset1->setAttribute(polyMode);
		//使圆锥由默认的z轴方向旋转到(1.0,1.0,1.0)方向
		//osg::Quat quat;
		////根据两个向量计算四元数
		//quat.makeRotate(osg::Z_AXIS, osg::Vec3(1.0, 1.0, 1.0));
		//cone->setRotation(quat);

		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight( osg::DegreesToRadians(31.3834), osg::DegreesToRadians(102.1347), 150000, mtTemp);
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
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), chuiZhiAngle, osg::Vec3(0.0, 1.0, 0.0), shuiPingAngle , osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);//上面代码chuiZhiAngle+osg::PI_2
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
	//osg::ref_ptr<osg::Vec4Array> vaTemp = new osg::Vec4Array;
	/*vaTemp->push_back(osg::Vec4(102.1347, 31.3834, 150000, 10000));
	vaTemp->push_back(osg::Vec4(106.1174, 36.3686, 150000, 20000));
	vaTemp->push_back(osg::Vec4(119.8794, 39.1944, 170000, 20000));
	vaTemp->push_back(osg::Vec4(125.1302, 44.3941, 180000, 20000));
	vaTemp->push_back(osg::Vec4(136.9387, 59.9202, 170000, 20000));
	vaTemp->push_back(osg::Vec4(147.5066, 66.5163, 160000, 20000));
	vaTemp->push_back(osg::Vec4(159.1347, 87.3834, 150000, 10000));
	vaTemp->push_back(osg::Vec4(102.1347, 31.3834, 140000, 10000));*/
	apc = CreateAirLinePath(vaTemp);
}

void OSGObject::DoPreLineNow()
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	//mtFlySelf->setMatrix(osg::Matrixd::scale(2000, 2000,2000));
	mtfly->setUpdateCallback(new osg::AnimationPathCallback(apc, 0.0, 1.0));

	//em->setViewpoint(osgEarth::Viewpoint("", 102.1347, 31.3834, 0, 45, 45, 150000),2.0);

	//跟踪节点
	em->setTetherNode(flyAirport);

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
	const osgEarth::SpatialReference* geoSRS =mapNode->getMapSRS()->getGeographicSRS();
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
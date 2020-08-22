#include "pch.h"
#include "OSGObject.h"
#include "DigitalEarth.h"

OSGObject::OSGObject(HWND hwnd)
{
	m_hwnd = hwnd;

	labelEvent = 0;
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
	//mRoot->addChild(osgDB::readNodeFile());
	mapNode = dynamic_cast<osgEarth::MapNode*>(mp.get());


}

void OSGObject::InitCameraConfig()
{
	RECT rect;
	mViewer = new osgViewer::Viewer;
	::GetWindowRect(m_hwnd, &rect);
	osg::ref_ptr<osg::GraphicsContext::Traits> traits
		= new osg::GraphicsContext::Traits;
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hwnd);

	/*traits->x = 20;
	traits->y = 20;
	traits->width = 600;
	traits->height =480;*/
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

	em->setViewpoint(osgEarth::Viewpoint("fsas",112.44, 33.75, 444.02, -15.84, -53.01, 4028182.75));

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

	//获取国界线图层
	china_boundaries = mapNode->getMap()->getImageLayerByName("china_boundaries");

	
	//地球自转
	//osg::ref_ptr<osg::MatrixTransform> max = new osg::MatrixTransform;
	////mp = osgDB::readNodeFile("../../sample.earth");
	//max->addChild(mp.get());
	//max->setUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0, 0.0, 0.0), osg::Z_AXIS, 0.2));
	//mRoot->addChild(max.get());
	//

	//新增显示视点信息的控件
	addViewPointLabel();

	//增加机场
	addAirport();

	//设置预设置路径
	DoAPreLine();
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
	mRoot->addChild(osgEarth::Util::Controls::ControlCanvas::get(mViewer));

	osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::get(mViewer);



	//添加控件，用来显示视点信息
	osgEarth::Util::Controls::LabelControl* viewCoords = new osgEarth::Util::Controls::LabelControl("TestViewPoint", osg::Vec4(0.0, 0.0, 0.0, 0.5));
	viewCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_CENTER);
	viewCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_CENTER);
	
	//viewCoords->setSize(800, 50);
	viewCoords->setMargin(50);
	//viewCoords->setPosition(80.0,80.0);
	viewCoords->setBackColor(0, 0, 0, 0.5);
	canvas->addControl(viewCoords);

	//添加控件，用于显示鼠标交点信息
	osgEarth::Util::Controls::LabelControl* mouseCoords = new osgEarth::Util::Controls::LabelControl("FocusPoint", osg::Vec4(1.0, 1.0, 1.0, 1.0));
	mouseCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_RIGHT);
	mouseCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
	mouseCoords->setBackColor(0, 0, 0, 0.5);
	//mouseCoords->setSize(400, 50);
	mouseCoords->setMargin(50);
	canvas->addControl(mouseCoords);

	//if (labelEvent == 0)
	{
		labelEvent = new CLabelControlEventHandler(viewCoords, mouseCoords, mapNode);
	}
	
	mViewer->addEventHandler(labelEvent);
	
}


void OSGObject::FlyTo(double lon, double lat, double hei)
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	em->setViewpoint(osgEarth::Viewpoint("gfs",lon, lat, 0, 0, -45, hei));

	theApp.bNeedModify = FALSE;
}

void OSGObject::addAirport()
{
	csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());
	//../../sample.earth
	airport = osgDB::readNodeFile("../../moon_1024x512.jpg");
	mtAirport = new osg::MatrixTransform;
	mtAirport->addChild(airport);
	mRoot->addChild(mtAirport);

	osg::Matrixd mtTemp;
	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(34.3762), osg::DegreesToRadians(109.1263), 99990, mtTemp);
	mtAirport->setMatrix(mtTemp);
	//aerospace.png
	flyAirport = osgDB::readNodeFile("../../sample.earth");
	mtFlySelf = new osg::MatrixTransform;
	mtFlySelf->setMatrix(osg::Matrixd::scale(0.01, 0.01, 0.01)* osg::Matrixd::rotate(-1.57 / 2, osg::Vec3(0, 0, 1)));
	mtFlySelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	mtFlySelf->addChild(flyAirport);

	mtfly = new osg::MatrixTransform;
	mtfly->addChild(mtFlySelf);
	mRoot->addChild(mtfly);

	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(34.3834), osg::DegreesToRadians(109.1347), 100000, mtTemp);
	mtfly->setMatrix(mtTemp);


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
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), chuiZhiAngle + osg::PI_2, osg::Vec3(0.0, 1.0, 0.0), shuiPingAngle, osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);
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
		return 1000000000;
	return dist / speed;
}

void OSGObject::DoAPreLine()
{
	osg::ref_ptr<osg::Vec4Array> vaTemp = new osg::Vec4Array;
	vaTemp->push_back(osg::Vec4(102.1347, 31.3834, 100000, 2));
	vaTemp->push_back(osg::Vec4(106.1174, 36.3686, 100880, 2));
	vaTemp->push_back(osg::Vec4(109.8794, 39.1944, 103000, 10));
	vaTemp->push_back(osg::Vec4(115.1302, 44.3941, 105000, 10));
	vaTemp->push_back(osg::Vec4(126.9387, 49.9202, 108000, 10));
	vaTemp->push_back(osg::Vec4(137.5066, 56.51, 103000, 10));
	vaTemp->push_back(osg::Vec4(149.1347, 67.3834,100000, 10));
	apc = CreateAirLinePath(vaTemp);
}

void OSGObject::DoPreLineNow()
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify)Sleep(1);

	mtFlySelf->setMatrix(osg::Matrixd::scale(0.01, 0.01,0.01)* osg::Matrixd::rotate(-1.57, osg::Vec3(0, 0, 1)));
	mtfly->setUpdateCallback(new osg::AnimationPathCallback(apc, 0.0, 1.0));
	em->setViewpoint(osgEarth::Viewpoint("op",109.1347, 34.3834, 0, 24.261, -21.6, 350000));
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
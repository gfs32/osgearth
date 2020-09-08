#include "pch.h"
#include "OSGObject.h"
#include "DigitalEarth.h"
#include <stdio.h> 
#include <stdlib.h> 
#include<Windows.h>
#include<mysql.h>
#pragma comment(lib, "libmysql.lib")

OSGObject::OSGObject(HWND hwnd)
{
	m_hwnd = hwnd;
	labelEvent = 0;
	vaTemp = new osg::Vec4Array();
	vertices = new osg::Vec3dArray();
	//vertices1 = new osg::Vec3dArray();
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

	//������ת
	//maxax->setUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0, 0.0, 0.0), osg::Z_AXIS, 0.02));

	//�ر��ʼ��
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

	osg::ref_ptr<osg::Camera>  camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0, 1000.0);
	//������Ⱦ˳�򣬱����������Ⱦ
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(true);
	//������Ȳ��ԣ�ģ�Ͳ��ڵ�
	camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	//�����Զ��ü���������ʾ������ڵ�	
	mViewer->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(mapNode));
	mViewer->setCamera(camera);
	mViewer->setCameraManipulator(new osgGA::TrackballManipulator);
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

	//��ʼ��������
	em = new osgEarth::Util::EarthManipulator;
	if (mapNode.valid())
	{
		em->setNode(mapNode);//mapNodeֻ�ܷ�һ��ģ��
	}
	em->getSettings()->setArcViewpointTransitions(true);
	mViewer->setCameraManipulator(em);
	em->setHomeViewpoint(osgEarth::Viewpoint("", 102.1347, 31.3834, 444.02, -15.84, -53.01, 4028182.75), 2);
	em->setViewpoint(osgEarth::Viewpoint("", 131.27,85.23, 444.02, -15.84, -53.01, 150613825), 2);

	//��ʼ�����
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
	//mRoot->addChild(sky_node);

	//��ȡʡ����ͼ��
	china_boundaries = mapNode->getMap()->getImageLayerByName("china_boundaries");

	//�����ر�
	addLabel();

	//�������ݿ�
	connectMysql();
		
	//��������
	addAirport();

	//����Ԥ����·��
	DoAPreLine();

	//������ʾ�ӵ���Ϣ�Ŀؼ�
	addViewPointLabel();

	//���
	//OrbitsThread();
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
	/*osgEarth::Util::Formatter* formatter = new osgEarth::Util::LatLongFormatter();
	osgEarth::Util::LabelControl* readout = new osgEarth::Util::LabelControl();
	osgEarth::Util::ControlCanvas::get(mViewer)->addControl(readout);
	osgEarth::Util::MouseCoordsTool* tool = new osgEarth::Util::MouseCoordsTool(mapNode);
	tool->addCallback(new osgEarth::Util::MouseCoordsLabelCallback(readout, formatter));
	mViewer->addEventHandler(tool);*/
	osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::get(mViewer);
	//��ӿؼ���������ʾ�ӵ���Ϣ
	osgEarth::Util::Controls::LabelControl* viewCoords = new osgEarth::Util::Controls::LabelControl("TestViewPoint", osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	viewCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
	viewCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_TOP);
	viewCoords->setMargin(10);
	canvas->addChild(viewCoords);
	//��ӿؼ���������ʾ��꽻����Ϣ
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

		mtFlySelf->setMatrix(osg::Matrixd::scale(60000, 60000, 60000));

		//mtFlySelf->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

		mtfly = new osg::MatrixTransform;
		//mtfly->setMatrix(osg::Matrix::rotate(-1.57, osg::Vec3(0, 1, 0)));
		mtfly->addChild(mtFlySelf);
		mRoot->addChild(mtfly);

		//osg::Vec3d center = mtfly->getBound().center(); //��ð�Χ������	
		//osg::Matrixd originPos = mtfly->getMatrix(); //��õ�ǰ����		
		//mtfly->setMatrix(originPos*osg::Matrixd::translate(-center) //�Ƚ���������ƽ�Ƶ����������ԭ�� 
		//	*osg::Matrixd::scale(800, 800, 800)//���� 
		//	*osg::Matrixd::rotate(-1.57, osg::Vec3(0, 1, 0))//��ת
		//	*osg::Matrixd::translate(center));//�任���ٽ������ƻ�

		//���ǹ��
		osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
		//const osgEarth::SpatialReference *mapsrs = mapNode->getMapSRS();
		osg::ref_ptr<osg::Geometry> linesgeom = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom1 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom2 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom3 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom4 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom5 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom6 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom7 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom8 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom9 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom10 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom11 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom12 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom13 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom14 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom15 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom16 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom17 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom18 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom19 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom20 = new osg::Geometry();
		osg::ref_ptr<osg::Geometry> linesgeom21 = new osg::Geometry();
		//osg::Vec3d  startWorld;
		//osg::EllipsoidModel *em = new osg::EllipsoidModel();
		//osg::Vec3dArray* vertices1 = new osg::Vec3dArray();

		/*for (double i = -180; i <= 180; i += 1)
		{
			vertices->push_back(osg::Vec3d(i, 0, 200000.0));
			vaTemp->push_back(osg::Vec4f(i, 0, 200000.0, 100000));
		}*/
		// ����γ��ת��Ϊ��������		
		//for (int i = 0; i < vertices->size();i++)
		//{
		/*em->convertLatLongHeightToXYZ(osg::DegreesToRadians(vertices->at(i).y()), osg::DegreesToRadians(vertices->at(i).x()),
		vertices->at(i).z(), startWorld.x(), startWorld.y(), startWorld.z());
		osgEarth::GeoPoint map(mapsrs, vertices->at(i).x(), vertices->at(i).y(),vertices->at(i).z(), osgEarth::ALTMODE_ABSOLUTE);
		map.toWorld(startWorld);*/
		//(*vertices1)[i] = osg::Vec3d(vertices->at(i).x(), vertices->at(i).y(), vertices->at(i).z() );
		//}
		
		//�������Ķ������鴫�ݸ����ζ���
		linesgeom->setVertexArray(vertices);
		linesgeom1->setVertexArray(vertices);
		linesgeom2->setVertexArray(vertices);
		linesgeom3->setVertexArray(vertices);
		linesgeom4->setVertexArray(vertices);
		linesgeom5->setVertexArray(vertices);
		linesgeom6->setVertexArray(vertices);
		linesgeom7->setVertexArray(vertices);
		linesgeom8->setVertexArray(vertices);
		linesgeom9->setVertexArray(vertices);
		//����
		linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 0, 361));
		linesgeom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361, 361));
		linesgeom2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 722, 361));
		linesgeom3->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 1083, 361));
		linesgeom4->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361*4, 361));
		linesgeom5->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361 * 5, 361));
		linesgeom6->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361 * 6, 361));
		linesgeom7->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361 * 7, 361));
		linesgeom8->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361 * 8, 361));
		linesgeom9->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 361 * 9, 361));
		//������ɫ
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		linesgeom->setColorArray(colors);
		linesgeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom1->setColorArray(colors);
		linesgeom1->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom2->setColorArray(colors);
		linesgeom2->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom3->setColorArray(colors);
		linesgeom3->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom4->setColorArray(colors);
		linesgeom4->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom5->setColorArray(colors);
		linesgeom5->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom6->setColorArray(colors);
		linesgeom6->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom7->setColorArray(colors);
		linesgeom7->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom8->setColorArray(colors);
		linesgeom8->setColorBinding(osg::Geometry::BIND_OVERALL);
		linesgeom9->setColorArray(colors);
		linesgeom9->setColorBinding(osg::Geometry::BIND_OVERALL);
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
		//���÷��ߡ�
		/*osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
		norms->push_back(osg::Vec3(0.0, 0.0, 1.0));*/
		osgUtil::SmoothingVisitor::smooth(*(linesgeom.get()));//�Զ����ɷ���
		osgUtil::SmoothingVisitor::smooth(*(linesgeom1.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom2.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom3.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom4.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom5.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom6.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom7.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom8.get()));
		osgUtil::SmoothingVisitor::smooth(*(linesgeom9.get()));
	    //linesgeom->setNormalArray(norms);
	    linesgeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom1->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom2->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom3->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom4->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom5->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom6->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom7->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom8->setNormalBinding(osg::Geometry::BIND_OVERALL);
		linesgeom9->setNormalBinding(osg::Geometry::BIND_OVERALL);
		//�����߿�
		osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
		width->setWidth(3.0);
		geode1->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);
		//��͸����
		geode1->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		//�ر�Ĭ�Ϲ���
		geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//geode1->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		//���㼸����ӵ���ز�����
		geode1->addDrawable(linesgeom);
		geode1->addDrawable(linesgeom1);
		geode1->addDrawable(linesgeom2);
		geode1->addDrawable(linesgeom3);
		geode1->addDrawable(linesgeom4);
		geode1->addDrawable(linesgeom5);
		geode1->addDrawable(linesgeom6);
		geode1->addDrawable(linesgeom7);
		geode1->addDrawable(linesgeom8);
		geode1->addDrawable(linesgeom9);
		//geode1->addChild(linesgeom);
		mRoot->addChild(geode1);

		//׶�β����������״�
		osg::ref_ptr<osg::Cone> cone = new osg::Cone;
		osg::ref_ptr<osg::ShapeDrawable>  shap = new osg::ShapeDrawable(cone);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;		
		geode->addChild(shap);
		mtfly->addChild(geode);
		cone->setHeight(100000);
		cone->setRadius(100000);
		//shap->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.0f));
		cone->setCenter(osg::Vec3(0,0,-130000));
		//����Բ׶͸��Ч��
		osg::ref_ptr<osg::StateSet> stateset1 = geode->getOrCreateStateSet();
		stateset1->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset1->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset1->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		stateset1->setAttribute(polyMode);
		//����Բ׶����ɫ�����ĸ�����0.25��ʾ��͸���ȣ�0��ʾ��ȫ͸����1��ʾ��ȫ��͸��
		shap->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.2));

		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight( osg::DegreesToRadians(0.0), osg::DegreesToRadians(102.1347), 150000, mtTemp);
		mtfly->setMatrix(mtTemp);
	}
}

//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ���ߣ��٣� ·������
osg::AnimationPath* OSGObject::CreateAirLinePath(osg::Vec4Array* ctrl)
{
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double shuiPingAngle;
	double chuiZhiAngle;
	double time = 0;

	osg::Matrix matrix;
	osg::Quat _rotation;

	//��ǰ��
	osg::Vec3d positionCur;
	//��һ��
	osg::Vec3d positionNext;

	for (osg::Vec4Array::iterator iter = ctrl->begin(); iter != ctrl->end(); iter++)
	{
		osg::Vec4Array::iterator iter2 = iter;

		iter2++;

		//��Ҫ�ж��ǲ����Ѿ�����
		if (iter2 == ctrl->end())
		{
			break;
		}

		//double x, y, z;
		/*csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), x, y, z);
		positionCur = osg::Vec3(x, y, z);*/
		positionCur = osg::Vec3(iter->x(), iter->y(), iter->z());
		/*csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter2->y()), osg::DegreesToRadians(iter2->x()), iter2->z(), x, y, z);
		positionNext = osg::Vec3(x, y, z);*/
		positionNext = osg::Vec3(iter2->x(), iter2->y(), iter2->z());

		//���ˮƽ�н�
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

		//��ֱ�н�
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

		//��ɻ��ı任����
		/*csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()), iter->z(), matrix);
		_rotation.makeRotate(0, osg::Vec3(1.0, 0.0, 0.0), chuiZhiAngle + osg::PI_2, osg::Vec3(0.0, 1.0, 0.0), shuiPingAngle , osg::Vec3(0.0, 0.0, 1.0));
		matrix.preMultRotate(_rotation);*/
		animationPath->insert(time, osg::AnimationPath::ControlPoint(positionCur));

		//����һ�����ʱ�������
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

	//���ٽڵ�
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
	//���ñ߿�
	textStyle->halo()->color() = osg::Vec4f(0.0, 0.0, 0.0, 1.0);
	textStyle->font() = "simsun.ttc";
	textStyle->size() = 20.0;
	textStyle->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	osg::Image* china = osgDB::readImageFile("../../china.png");
	osgEarth::Annotation::PlaceNode *pn = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint(geoSRS, 105.1, 30.3), china, "China", style);
	earthLabel->addChild(pn);
}

void OSGObject::connectMysql()
{
	MYSQL mysql;
	mysql_init(&mysql);
	//const char * i_query = "select * from inspectgps"; //��ѯ��䣬���Ǹ����в�ѯ,�������û��;
	const char * i_query = "select name,x,y,z from orbit";
	MYSQL_RES * result = NULL; //����������
	MYSQL_ROW row = NULL; //������ǽ�����е�һ�� 
	if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "199686", "satellite", 3306, NULL, 0))
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
		return;
	}
	else//���ӳɹ�������������ݿ⣬֮�����ز�����������Ƿ����������
	{	
		if (mysql_query(&mysql, i_query) != 0) //������ӳɹ�����ʼ��ѯ .�ɹ�����0
		{
			AfxMessageBox(_T("��ѯʧ��!"));
		}
		else
		{
			if ((result = mysql_store_result(&mysql)) == NULL) //�����ѯ�Ľ�� 
			{
				AfxMessageBox(_T("����ʧ��!"));
			}
			else
			{
				while ((row = mysql_fetch_row(result)) != NULL) //��ȡ������е����ݣ����ص�����һ�С���Ϊ��������ʱ����ǰ���α��ڵ�һ�С�֮ǰ�� 
				{							
					vertices->push_back(osg::Vec3d(atof(row[1]), atof(row[2]), atof(row[3])));					 
					//vaTemp->push_back(osg::Vec4f(atof(row[1]), atof(row[2]), atof(row[3]), 1000000));
				}
			}
		}
		mysql_free_result(result); //�ͷŽ���� 
		mysql_close(&mysql); //�ر����� 
	}

	//(HANDLE)_beginthread(&OSGObject::OrbitsThread,0,NULL);
}

void OSGObject::OrbitsThread()
{
	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
    osg::ref_ptr<osg::Geometry> linesgeom = new osg::Geometry();
	osg::Vec3dArray* vertices2 = new osg::Vec3dArray();
	//������ɫ
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
	linesgeom->setColorArray(colors);
	linesgeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	//���÷��ߡ�
	osgUtil::SmoothingVisitor::smooth(*(linesgeom.get()));//�Զ����ɷ���
	linesgeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//�����߿�
	osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
	width->setWidth(3.0);
	geode1->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);
	//��͸����
	geode1->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//�ر�Ĭ�Ϲ���
	geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//geode1->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	for (int i = 0; i < 1; i++)
	{		
		for (int j = 0; j < 361; j++) 
		{
			(*vertices2)[j] = vertices->at(j);
		}		
		//�������Ķ������鴫�ݸ����ζ���
		linesgeom->setVertexArray(vertices2);
		//����
		linesgeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINE_STRIP, 0, vertices2->size()));		
		//���㼸����ӵ���ز�����
		geode1->addDrawable(linesgeom);		
	}
	 mRoot->addChild(geode1);
	
	//theApp.bNeedModify = TRUE;
	//while (!theApp.bCanModify)Sleep(1);

	//theApp.bNeedModify = FALSE;
	//_endthread();
}
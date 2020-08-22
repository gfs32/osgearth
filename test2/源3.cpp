//
//#include <osgViewer/Viewer>
//#include <osgGA/TrackballManipulator>
//#include <osg/NodeCallback>
//#include <osg/Camera>
//#include <osg/Group>
//#include <osg/MatrixTransform>
//#include <osgDB/ReadFile>
//#include <osg/Notify>
//
//class RotateCB :public osg::NodeCallback
//{
//
//public:
//	RotateCB() :_angle(0.) {}
//	virtual void opreator(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform* mtLeft = dynamic_cast<osg::MatrixTransform*>(node);
//		osg::Matrix mR, mT;
//		mT.makeTranslate(-6., 0., 0.);
//		mR.makeRotate(_angle, osg::Vec3(0., 0., 1.));
//		mtLeft->setMatrix(mR * mT);
//		_angle += 0.01;
//		traverse(node, nv);
//		
//	}
//
//protected:
//	double _angle;
//};
//
//osg::ref_ptr<osg::Node> creatScence()
//{
//	osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("cow.osg");
//	if (!cow.valid())
//	{
//		osg::notify(osg::FATAL) << "Unable to load data file. Exiting." << std::endl;
//		return NULL;
//	}
//	cow->setDataVariance(osg::Object::STATIC);
//	osg::ref_ptr<osg::MatrixTransform> mtLeft = new osg::MatrixTransform;
//	mtLeft->setName("left Cow\nDYNAMIC");
//	mtLeft->setDataVariance(osg::Object::DYNAMIC);
//	mtLeft->setUpdateCallback(new RotateCB);
//	osg::Matrix m;
//	m.makeTranslate(-6.f, 0.f, 0.f);
//	mtLeft->setMatrix(m);
//	mtLeft->addChild(cow.get());
//	
//	osg::ref_ptr<osg::MatrixTransform> mtRight = new osg::MatrixTransform;
//	mtRight->setName("Right Cow\nSTATIC");
//	mtRight->setDataVariance(osg::Object::STATIC);
//	m.makeTranslate(6.f, 0.f, 0.f);
//	mtRight->setMatrix(m);
//	mtRight->addChild(cow.get());
//	
//
//	osg::ref_ptr<osg::Group> root = new osg::Group;
//	root->setName("Root Node");
//	root->setDataVariance(osg::Object::STATIC);
//	root->addChild(mtLeft.get());
//	root->addChild(mtRight.get());
//
//	return root.get();
//}
//
//int main(int, char**)
//{
//	osgViewer::Viewer viewer;
//	
//	viewer.setSceneData(creatScence().get());
//	if (!viewer.getSceneData())
//		return 1;
//	return viewer.run();
//}

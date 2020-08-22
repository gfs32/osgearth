#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osgDB/Registry>
#include <osgDB/WriteFile>
#include <osg/Notify>
#include <iostream>

using std::endl;

osg::ref_ptr<osg::Node> creatSceneGraph()
{
	//����һ�����漸����Ϣ�Ķ���
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	//�����ĸ����������
	osg::ref_ptr<osg::Vec3Array>v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, 1.f));
	v->push_back(osg::Vec3(-1.f, 0.f, 1.f));

	//����������ɫ������
	osg::ref_ptr<osg::Vec4Array>c = new osg::Vec4Array;
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 0.f, 0.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	//ΪΨһ�ķ��ߴ���һ������
	osg::ref_ptr<osg::Vec3Array>n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.f, 0.f));

	//�ɱ�������ݻ����ĸ�����Ķ����
	geom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4)
	);

	//��Geode����Ӽ�����Drawable������Geode
	osg::ref_ptr<osg::Geode>geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode.get();
}


int main(int, char**)
{
	osg::ref_ptr<osg::Node>root = creatSceneGraph();
	if (!root.valid())
	{
		osg::notify(osg::FATAL) << "Failed in creatSceneGraph()" << endl;
	}

	bool result = osgDB::writeNodeFile(*(root.get()), "Simple.osg");

	if (!result)
	{
		osg::notify(osg::FATAL) << "Failed in osgDB::writeNode()" << endl;

	}
	else
	{
		osg::notify(osg::FATAL) << "Successed in osgDB::writeNode()" << endl;

	}
}
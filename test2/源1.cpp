/*
#include <osgGA/GUIEventHandler>

#include <osgViewer/Viewer>

#include <osgViewer/ViewerEventHandlers>

#include <osgEarth/MapNode>

#include <osgEarth/Registry>

#include <osgEarth/ObjectIndex>

#include <osgEarthUtil/EarthManipulator>

#include <osgEarthUtil/ExampleResources>

#include <osgEarthUtil/Controls>

#include <osgEarthUtil/RTTPicker>

#include <osgEarthFeatures/Feature>

#include <osgEarthFeatures/FeatureIndex>



#define LC "[feature_query] "



using namespace osgEarth::Util;

using namespace osgEarth::Util::Controls;

using namespace osgEarth::Features;



//-----------------------------------------------------------------------




// Creates a simple user interface for the demo.


Container* createUI()

{

	VBox* vbox = new VBox();

	vbox->setVertAlign(Control::ALIGN_TOP);

	vbox->setHorizAlign(Control::ALIGN_RIGHT);

	vbox->addControl(new LabelControl("Feature Query Demo", Color::Yellow));

	vbox->addControl(new LabelControl("Click on a feature to see its attributes."));

	return vbox;

}



//-----------------------------------------------------------------------




// Query Callback that displays the targeted feature's attributes in a

// user interface grid control.




class ReadoutCallback : public RTTPicker::Callback

{

public:

	ReadoutCallback(ControlCanvas* container) : _lastFID(~0)

	{

		_grid = new Grid();

		_grid->setBackColor(osg::Vec4(0, 0, 0, 0.7f));

		container->addControl(_grid);

	}



	void onHit(ObjectID id)

	{

		FeatureIndex* index = Registry::objectIndex()->get<FeatureIndex>(id).get();

		Feature* feature = index ? index->getFeature(id) : 0L;

		if (feature && feature->getFID() != _lastFID)

		{

			_grid->clearControls();

			unsigned r = 0;



			_grid->setControl(0, r, new LabelControl("FID", Color::Red));

			_grid->setControl(1, r, new LabelControl(Stringify() << feature->getFID(), Color::White));

			++r;



			const AttributeTable& attrs = feature->getAttrs();

			for (AttributeTable::const_iterator i = attrs.begin(); i != attrs.end(); ++i, ++r)

			{

				_grid->setControl(0, r, new LabelControl(i->first, 14.0f, Color::Yellow));

				_grid->setControl(1, r, new LabelControl(i->second.getString(), 14.0f, Color::White));

			}

			if (!_grid->visible())

				_grid->setVisible(true);



			_lastFID = feature->getFID();

		}

	}



	void onMiss()

	{

		_grid->setVisible(false);

		_lastFID = 0u;

	}



	bool accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa)

	{

		return ea.getEventType() == ea.RELEASE; // click

	}



	Grid*     _grid;

	FeatureID _lastFID;

};



//------------------------------------------------------------------------



int main(int argc, char** argv)

{

	osg::ArgumentParser arguments(&argc, argv);



	// a basic OSG viewer

	osgViewer::Viewer viewer(arguments);



	// install our default manipulator (do this before using MapNodeHelper)

	viewer.setCameraManipulator(new EarthManipulator());



	// load an earth file, and support all or our example command-line options

	// and earth file <external> tags

	osg::Group* root = MapNodeHelper().load(arguments, &viewer, createUI());

	if (root)

	{

		viewer.setSceneData(root);



		MapNode* mapNode = MapNode::findMapNode(root);

		if (mapNode)

		{

			// Install the query tool.

			RTTPicker* picker = new RTTPicker();

			viewer.addEventHandler(picker);

			picker->addChild(mapNode);



			// Install a readout for feature metadata.

			ControlCanvas* canvas = ControlCanvas::getOrCreate(&viewer);

			picker->setDefaultCallback(new ReadoutCallback(canvas));

		}



		return viewer.run();

	}

	else

	{

		OE_NOTICE

			<< "\nUsage: " << argv[0] << " file.earth" << std::endl

			<< MapNodeHelper().usage() << std::endl;

	}

}
*/

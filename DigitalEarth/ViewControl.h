#ifndef VIEWCONTROL_H
#define VIEWCONTROL_H

#include<osgGA/TrackballManipulator>

class ViewSelect : public osgGA::TrackballManipulator
{
public:
	ViewSelect() :TrackballManipulator() {}

protected:
	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

private:
	void moveToView(const osgGA::GUIEventAdapter & ea);

};

#endif // VIEWCONTROL_H
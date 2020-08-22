
#ifndef _COMMEN_H_
#define _COMMEN_H_


#ifdef DEBUG
#pragma comment(lib,"osgd.lib")
#pragma comment(lib,"osgDBd.lib")
#pragma comment(lib,"osgViewerd.lib")
#pragma comment(lib,"OpenThreadsd.lib")
#else
#pragma comment(lib,"osg.lib")
#pragma comment(lib,"osgDB.lib")
#pragma comment(lib,"osgViewer.lib")
#pragma comment(lib,"OpenThreads.lib")
#pragma comment(lib,"osgGA.lib")
#endif 


#endif // !_COMMEN_H_

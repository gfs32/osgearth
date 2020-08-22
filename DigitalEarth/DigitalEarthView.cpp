﻿// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// DigitalEarthView.cpp: CDigitalEarthView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DigitalEarth.h"
#endif

#include "DigitalEarthDoc.h"
#include "DigitalEarthView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDigitalEarthView

IMPLEMENT_DYNCREATE(CDigitalEarthView, CView)

BEGIN_MESSAGE_MAP(CDigitalEarthView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//ON_UPDATE_COMMAND_UI(ID_EDIT2, &CDigitalEarthView::OnChinaBound)
	ON_COMMAND(ID_BUTTON2, &CDigitalEarthView::OnSetChinaBound)
	ON_COMMAND(ID_CHECK4, &CDigitalEarthView::OnShowChinaBound)
	ON_UPDATE_COMMAND_UI(ID_CHECK4, &CDigitalEarthView::OnUpdateShowChinaBound)
	ON_COMMAND(ID_EDIT2, &CDigitalEarthView::OnChinaBound)
	ON_COMMAND(ID_EDIT3, &CDigitalEarthView::OnFlytoLon)
	ON_COMMAND(ID_EDIT4, &CDigitalEarthView::OnFlytoLat)
	ON_COMMAND(ID_EDIT5, &CDigitalEarthView::OnFlytoHei)
	ON_COMMAND(ID_BUTTON3, &CDigitalEarthView::OnButtonFlyto)
	ON_COMMAND(ID_CHECK_start, &CDigitalEarthView::OnCheck2Start)
	ON_UPDATE_COMMAND_UI(ID_CHECK_start, &CDigitalEarthView::OnUpdateCheck2Start)
	ON_COMMAND(ID_CHECK_track, &CDigitalEarthView::OnCheck3)
	ON_UPDATE_COMMAND_UI(ID_CHECK_track, &CDigitalEarthView::OnUpdateCheck3Track)
	ON_COMMAND(ID_CHECK2, &CDigitalEarthView::OnCheck2)
	ON_UPDATE_COMMAND_UI(ID_CHECK2, &CDigitalEarthView::OnUpdateCheck2)
END_MESSAGE_MAP()

// CDigitalEarthView 构造/析构

CDigitalEarthView::CDigitalEarthView() noexcept
{
	// TODO: 在此处添加构造代码
	mOSG = 0;
	mThreadHandle = 0;
	isShowChinaBoundary = true;
	chinaBoundariesOpt = 1.0;
	/*flylat = 112;
	flylon = 33;
	flyhei = 400000;*/
	isStartFly = false;
	isTrack = false;
}

CDigitalEarthView::~CDigitalEarthView()
{
}

BOOL CDigitalEarthView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDigitalEarthView 绘图

void CDigitalEarthView::OnDraw(CDC* /*pDC*/)
{
	CDigitalEarthDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CDigitalEarthView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDigitalEarthView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDigitalEarthView 诊断

#ifdef _DEBUG
void CDigitalEarthView::AssertValid() const
{
	CView::AssertValid();
}

void CDigitalEarthView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDigitalEarthDoc* CDigitalEarthView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDigitalEarthDoc)));
	return (CDigitalEarthDoc*)m_pDocument;
}
#endif //_DEBUG


// CDigitalEarthView 消息处理程序


int CDigitalEarthView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	mOSG = new OSGObject(m_hWnd);
	
	return 0;
}


BOOL CDigitalEarthView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (0 == mOSG)
	{
		return CView::OnEraseBkgnd(pDC);
	}
	else
	{
		return FALSE;
	}

	return CView::OnEraseBkgnd(pDC);
}



void CDigitalEarthView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (mOSG != 0) delete mOSG;
	WaitForSingleObject(mThreadHandle, 1000);
}


void CDigitalEarthView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	mOSG->InitOSG();
	mThreadHandle = (HANDLE)_beginthread(&OSGObject::Render, 0, mOSG);
}




void CDigitalEarthView::OnSetChinaBound()
{
	CDigitalEarthApp* pApp = (CDigitalEarthApp*)AfxGetApp();
	CMainFrame* pWnd = (CMainFrame*)pApp->GetMainWnd();
	CMFCRibbonEdit* edit = dynamic_cast<CMFCRibbonEdit*>(pWnd->m_wndRibbonBar.FindByID(ID_EDIT2));

	if (edit)
	{
		CString str = edit->GetEditText();
		//std::string strTemp(str.GetString());
		std::string STDstr(CW2A(str.GetString()));
		double opt = std::atof(STDstr.c_str());
		if (opt < 0)
		{
			MessageBoxW(str, TEXT("世界你好！"), MB_OK | MB_ICONEXCLAMATION);
			//str.Format("%f", chinaBoundariesOpt);
			edit->SetEditText(str);

		}
		else
		{
			//chinaBoundariesOpt = opt;
			mOSG->setChinaBoundariesOpacity(opt);
		}
	}

}


void CDigitalEarthView::OnShowChinaBound()
{
	theApp.bNeedModify = TRUE;
	while (!theApp.bCanModify) Sleep(1);
	isShowChinaBoundary = !isShowChinaBoundary;
	if (isShowChinaBoundary)
	{
		mOSG->addChinaBoundaryes();
	}
	else
	{
		mOSG->rmvChinaBoundaryes();
	}
	theApp.bNeedModify = FALSE;
}


void CDigitalEarthView::OnUpdateShowChinaBound(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(isShowChinaBoundary);
}


void CDigitalEarthView::OnChinaBound()
{
	// TODO: 在此添加命令处理程序代码
}


void CDigitalEarthView::OnFlytoLon()
{
	// TODO: 在此添加命令处理程序代码
}


void CDigitalEarthView::OnFlytoLat()
{
	// TODO: 在此添加命令处理程序代码
}


void CDigitalEarthView::OnFlytoHei()
{
	// TODO: 在此添加命令处理程序代码
}


void CDigitalEarthView::OnButtonFlyto()
{
	// TODO: 在此添加命令处理程序代码
	CDigitalEarthApp* pApp = (CDigitalEarthApp*)AfxGetApp();
	CMainFrame* pWnd = (CMainFrame*)pApp->GetMainWnd();

	{
		//经度
		CMFCRibbonEdit*edit = dynamic_cast<CMFCRibbonEdit*>(pWnd->m_wndRibbonBar.FindByID(ID_EDIT3));

		if (edit)
		{
			/*CString str = edit->GetEditText();
			std::string strTemp(str.GetBuffer());
			double opt = std::atof(strTemp.c_str());*/

			CString str = edit->GetEditText();			
			std::string STDstr(CW2A(str.GetString()));
			double opt = std::atof(STDstr.c_str());

			if ((180 < opt) || (opt < -180))
			{
				//MessageBox("", "错误", MB_OK | MB_ICONERROR);
				MessageBoxW(str, TEXT("经度必须介于（-180，180）之间"), MB_OK | MB_ICONEXCLAMATION);
				//str.Format("%f", flylon);
				edit->SetEditText(str);
				return;
			}
			else
			{
				flylon = opt;
			}
		}
	}

	{
		//纬度
		CMFCRibbonEdit*edit = dynamic_cast<CMFCRibbonEdit*>(pWnd->m_wndRibbonBar.FindByID(ID_EDIT4));

		if (edit)
		{
			CString str = edit->GetEditText();
			std::string STDstr(CW2A(str.GetString()));
			double opt = std::atof(STDstr.c_str());
			if ((90 < opt) || (opt < -90))
			{
				//MessageBox("经度必须介于（-90，90）之间", "错误", MB_OK | MB_ICONERROR);
				//str.Format("%f", flylat);
				edit->SetEditText(str);
				return;
			}
			else
			{
				flylat = opt;
			}
		}

	}

	{
		//高度
		CMFCRibbonEdit*edit = dynamic_cast<CMFCRibbonEdit*>(pWnd->m_wndRibbonBar.FindByID(ID_EDIT5));

		if (edit)
		{
			CString str = edit->GetEditText();
			std::string STDstr(CW2A(str.GetString()));
			double opt = std::atof(STDstr.c_str());
			if (opt < 0)
			{
				//MessageBox("经度必须为正值", "错误", MB_OK | MB_ICONERROR);
				//str.Format("%f", flyhei);
				edit->SetEditText(str);
				return;
			}
			else
			{
				flyhei = opt;
			}
		}

	}

	mOSG->FlyTo(flylon, flylat, flyhei);
}


void CDigitalEarthView::OnCheck2Start()
{
	// TODO: 在此添加命令处理程序代码
	isStartFly = !isStartFly;
	isTrack = true;
	mOSG->DoPreLineNow();
}


void CDigitalEarthView::OnUpdateCheck2Start(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(isStartFly);
}


void CDigitalEarthView::OnCheck3()
{

}
void CDigitalEarthView::OnUpdateCheck3Track(CCmdUI *pCmdUI)
{

}


void CDigitalEarthView::OnCheck2()
{
	// TODO: 在此添加命令处理程序代码
	isTrack = !isTrack;
	mOSG->isTrackFly(isTrack);
}


void CDigitalEarthView::OnUpdateCheck2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (isStartFly == false)
	{
		pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
		isTrack = false;
	}

	else
	{
		pCmdUI->SetCheck(isTrack);
	}
}


// OpenGLView.cpp : implementation of the COpenGLView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "OpenGL.h"
#endif

#include "OpenGLDoc.h"
#include "OpenGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_BUTTON_SELECTION, &COpenGLView::OnButtonSelection)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SELECTION, &COpenGLView::OnUpdateButtonSelection)
END_MESSAGE_MAP()

// COpenGLView construction/destruction

COpenGLView::COpenGLView()
{
	// TODO: add construction code here
	m_bIsLeftMouse = false;
	m_bIsCenterMouse = false;
	m_bIsRightMouse = false;
	m_bIsSelection = false;
	m_bIsBoxCreated = false;
}

COpenGLView::~COpenGLView()
{
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

// COpenGLView drawing

void COpenGLView::OnDraw(CDC* pDC)
{
	COpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: add draw code for native data here
	//GetDocument()
	pDoc->m_instanceOGL.DrawScene(pDC);
	if (m_bIsSelection)	//selection mode
	{
		if (m_bIsLeftMouse)
		{
			if (!m_bIsBoxCreated)
				pDoc->m_instanceOGL.CreateBox(m_startPoint.x, m_startPoint.y, m_endPoint.x, m_endPoint.y);
			else
			{
				pDoc->m_instanceOGL.MoveBoxXY(createdPoint.x, createdPoint.y);
				pDoc->m_instanceOGL.SelectBox();
			}
		}
		else
			pDoc->m_instanceOGL.SelectBox();
		pDoc->m_instanceOGL.DrawBox();	//this contains the Select and Draw
	}
	else
		pDoc->m_instanceOGL.Clear();

	SwapBuffers(pDC->m_hDC);
}

// COpenGLView printing

BOOL COpenGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COpenGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void COpenGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLDoc* COpenGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLDoc)));
	return (COpenGLDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenGLView message handlers


int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CClientDC dc(this);
	GetDocument()->m_instanceOGL.CreateGLContext(&dc);
	GetDocument()->m_instanceOGL.InitScene("models/TableDesk-AgiSoft.obj");

	// TODO:  Add your specialized creation code here

	return 0;
}

void COpenGLView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	CClientDC dc(this);
	GetDocument()->m_instanceOGL.DestroyScene(&dc);
}


void COpenGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	CClientDC dc(this);
	GetDocument()->m_instanceOGL.Reshape(&dc, cx, cy);
	GetDocument()->m_windowSize.SetPoint(cx, cy);
}


BOOL COpenGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}


void COpenGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (nFlags == MK_LBUTTON)
	{
		m_bIsLeftMouse = true;
		if (m_bIsSelection)
		{
			//m_startPoint = m_endPoint = point;
			RECT rect;
			GetClientRect(&rect);
			m_startPoint.x = point.x;
			m_startPoint.y = rect.bottom - point.y;
			m_endPoint = m_startPoint;
		}
		else
		{
			GetDocument()->m_instanceOGL.MouseDown(point.x, point.y);
		}		
	}
	CView::OnLButtonDown(nFlags, point);
}


void COpenGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsSelection)
	{

		RECT rect;
		GetClientRect(&rect);
		m_endPoint.x = point.x;
		m_endPoint.y = rect.bottom - point.y;
		if (m_bIsBoxCreated)
		{
			createdPoint.x = point.x;
			createdPoint.y = rect.bottom - point.y;
		}
		Invalidate(false);
	}
	else
	{
		if (m_bIsLeftMouse)
			GetDocument()->m_instanceOGL.MouseMove(point.x, point.y, ROTATE);
		else if (m_bIsRightMouse)
			GetDocument()->m_instanceOGL.MouseMove(point.x, point.y, TRANSLATE);
		else if (m_bIsCenterMouse)
			GetDocument()->m_instanceOGL.MouseMove(point.x, point.y, SCALE);
	}
	
	Invalidate(false);
	CView::OnMouseMove(nFlags, point);
}

void COpenGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsSelection)
	{
		//add somthing about  m_endPoint
		//if (nFlags == )
		m_bIsBoxCreated = true;
	}

	if (m_bIsLeftMouse)
		m_bIsLeftMouse = false;	//it could be an if/else statement but its the same
	CView::OnLButtonUp(nFlags, point);
}


void COpenGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (nFlags == MK_RBUTTON)
	{
		m_bIsRightMouse = true;
		if (m_bIsSelection)
		{
		
		}
		else
			GetDocument()->m_instanceOGL.MouseDown(point.x, point.y);
	}
	CView::OnRButtonDown(nFlags, point);
}


void COpenGLView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsRightMouse)
		m_bIsRightMouse = false;
	CView::OnRButtonUp(nFlags, point);
}


void COpenGLView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (nFlags == MK_MBUTTON)
	{
		m_bIsCenterMouse = true;
		GetDocument()->m_instanceOGL.MouseDown(point.x, point.y);
	}
	CView::OnMButtonDown(nFlags, point);
}


void COpenGLView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsCenterMouse)
		m_bIsCenterMouse = false;
	CView::OnMButtonUp(nFlags, point);
}


void COpenGLView::OnButtonSelection()
{
	// TODO: Add your command handler code here
	m_bIsSelection = !m_bIsSelection;
}


void COpenGLView::OnUpdateButtonSelection(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bIsSelection);
	Invalidate(false);
}

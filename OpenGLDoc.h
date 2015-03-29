
// OpenGLDoc.h : interface of the COpenGLDoc class
//


#pragma once
#include "GLRenderer.h"
class COpenGLDoc : public CDocument
{
protected: // create from serialization only
	COpenGLDoc();
	DECLARE_DYNCREATE(COpenGLDoc)
public:
	CGLRenderer m_instanceOGL;
// Attributes
public:
	CPoint m_windowSize;
// Operations
public:
	void UnProjection(CPoint point, int width, int height);
// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~COpenGLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

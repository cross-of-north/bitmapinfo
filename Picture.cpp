// Picture.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "border.h"
#include "Picture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicture

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}


BEGIN_MESSAGE_MAP(CPicture, CStatic)
	//{{AFX_MSG_MAP(CPicture)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicture message handlers


void CPicture::PreSubclassWindow() 
   {
    c_Border.Create(this);
    CStatic::PreSubclassWindow();
   }

/****************************************************************************
*                              CPicture::ShowBB
* Inputs:
*       const CRect & r: Rectangle
* Result: void
*       
* Effect: 
*       Sets the window size
****************************************************************************/

void CPicture::ShowBB(const CRect &r)
    {
     c_Border.ShowBB(r);
    } // CPicture::ShowBB

/****************************************************************************
*                              CPicture::ShowBB
* Inputs:
*       BOOL show: TRUE to show, FALSE to hide
* Result: void
*       
* Effect: 
*       Shows/hides the window
****************************************************************************/

void CPicture::ShowBB(BOOL show)
    {
     c_Border.ShowBB(show);
    } // CPicture::ShowBB

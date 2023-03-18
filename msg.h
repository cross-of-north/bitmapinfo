
/****************************************************************************
*                                UWM_SHOW_FILE
* Inputs:
*       WPARAM: unused
*	LPARAM: (LPARAM)(LPCTSTR) filename
* Result: (LRESULT)(BOOL)
*       TRUE if successful
*	FALSE if successful
* Effect: 
*       Sent to the viewer to cause the file to be displayed
****************************************************************************/

#define UWM_SHOW_FILE_MSG _T("UWM_SHOW_FILE-{7EF5F2E0-2050-11d7-A083-00095B1A7BE1}")


/****************************************************************************
*                                 UWM_UPDATE_BB
* Inputs:
*       WPARAM: ignored
*	LPARAM: Ignored
* Result: LRESULT
*       Logically void, 0, always
* Effect: 
*       Tells the window to update the bounding box information
* Notes:
*	This is issued to the window in the OnIdle handler
****************************************************************************/

#define UWM_UPDATE_BB_MSG _T("UWM_UPDATE_BB-{7EF5F2E1-2050-11d7-A083-00095B1A7BE1}")



#define DECLARE_MESSAGE(x) static UINT x = ::RegisterWindowMessage(x##_MSG);


// WindowsHModular https://github.com/Leandros/WindowsHModular
#include "win32\windows_base.h"
#include "win32\misc.h"

typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;
typedef DWORD POINTER_INPUT_TYPE;
typedef UINT32 POINTER_FLAGS;

DECLARE_HANDLE(HTOUCHINPUT);

typedef enum tagPOINTER_BUTTON_CHANGE_TYPE {
    POINTER_CHANGE_NONE,
    POINTER_CHANGE_FIRSTBUTTON_DOWN,
    POINTER_CHANGE_FIRSTBUTTON_UP,
    POINTER_CHANGE_SECONDBUTTON_DOWN,
    POINTER_CHANGE_SECONDBUTTON_UP,
    POINTER_CHANGE_THIRDBUTTON_DOWN,
    POINTER_CHANGE_THIRDBUTTON_UP,
    POINTER_CHANGE_FOURTHBUTTON_DOWN,
    POINTER_CHANGE_FOURTHBUTTON_UP,
    POINTER_CHANGE_FIFTHBUTTON_DOWN,
    POINTER_CHANGE_FIFTHBUTTON_UP,
} POINTER_BUTTON_CHANGE_TYPE;

enum tagPOINTER_INPUT_TYPE {
    PT_POINTER  = 1,   // Generic pointer
    PT_TOUCH    = 2,   // Touch
    PT_PEN      = 3,   // Pen
    PT_MOUSE    = 4,   // Mouse
#if(WINVER >= 0x0603)
    PT_TOUCHPAD = 5,   // Touchpad
#endif /* WINVER >= 0x0603 */
};

typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

typedef struct _RECT {
    LONG        left;
    LONG        top;
    LONG        right;
    LONG        bottom;
} RECT, *PRECT, *LPRECT;

typedef struct tagPOINTER_INFO {
    POINTER_INPUT_TYPE    pointerType;
    UINT32          pointerId;
    UINT32          frameId;
    POINTER_FLAGS   pointerFlags;
    HANDLE          sourceDevice;
    HWND            hwndTarget;
    POINT           ptPixelLocation;
    POINT           ptHimetricLocation;
    POINT           ptPixelLocationRaw;
    POINT           ptHimetricLocationRaw;
    DWORD           dwTime;
    UINT32          historyCount;
    INT32           InputData;
    DWORD           dwKeyStates;
    UINT64          PerformanceCount;
    POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef struct tagTOUCHINPUT {
    LONG x;
    LONG y;
    HANDLE hSource;
    DWORD dwID;
    DWORD dwFlags;
    DWORD dwMask;
    DWORD dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD cxContact;
    DWORD cyContact;
} TOUCHINPUT, *PTOUCHINPUT;

typedef TOUCHINPUT const *PCTOUCHINPUT;

typedef UINT32 TOUCH_FLAGS;
#define TOUCH_FLAG_NONE                 0x00000000 // Default

typedef UINT32 TOUCH_MASK;
#define TOUCH_MASK_NONE                 0x00000000 // Default - none of the optional fields are valid
#define TOUCH_MASK_CONTACTAREA          0x00000001 // The rcContact field is valid
#define TOUCH_MASK_ORIENTATION          0x00000002 // The orientation field is valid
#define TOUCH_MASK_PRESSURE             0x00000004 // The pressure field is valid

typedef struct tagPOINTER_TOUCH_INFO {
    POINTER_INFO    pointerInfo;
    TOUCH_FLAGS     touchFlags;
    TOUCH_MASK      touchMask;
    RECT            rcContact;
    RECT            rcContactRaw;
    UINT32          orientation;
    UINT32          pressure;
} POINTER_TOUCH_INFO;

typedef UINT32 PEN_FLAGS;
#define PEN_FLAG_NONE                   0x00000000 // Default
#define PEN_FLAG_BARREL                 0x00000001 // The barrel button is pressed
#define PEN_FLAG_INVERTED               0x00000002 // The pen is inverted
#define PEN_FLAG_ERASER                 0x00000004 // The eraser button is pressed

typedef UINT32 PEN_MASK;
#define PEN_MASK_NONE                   0x00000000 // Default - none of the optional fields are valid
#define PEN_MASK_PRESSURE               0x00000001 // The pressure field is valid
#define PEN_MASK_ROTATION               0x00000002 // The rotation field is valid
#define PEN_MASK_TILT_X                 0x00000004 // The tiltX field is valid
#define PEN_MASK_TILT_Y                 0x00000008 // The tiltY field is valid

typedef struct tagPOINTER_PEN_INFO {
    POINTER_INFO    pointerInfo;
    PEN_FLAGS       penFlags;
    PEN_MASK        penMask;
    UINT32          pressure;
    UINT32          rotation;
    INT32           tiltX;
    INT32           tiltY;
} POINTER_PEN_INFO;


// Functions for Subclassing
typedef LRESULT CALLBACK SUBCLASSPROC(HWND, UINT, WPARAM, LPARAM, UINT_PTR , DWORD_PTR);
typedef BOOL    WINAPI type_SetWindowSubclass(HWND, SUBCLASSPROC, UINT_PTR, DWORD_PTR);
typedef LRESULT WINAPI type_DefSubclassProc(HWND, UINT, WPARAM, LPARAM);
typedef BOOL    WINAPI type_RemoveWindowSubclass(HWND, SUBCLASSPROC, UINT_PTR);

typedef BOOL    WINAPI type_GetTouchInputInfo(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
typedef BOOL    WINAPI type_CloseTouchInputHandle(HTOUCHINPUT);
typedef BOOL    WINAPI type_ScreenToClient(HWND,POINT*);
typedef BOOL    WINAPI type_GetPointerInfo(UINT32, POINTER_INFO*);
typedef BOOL    WINAPI type_GetPointerType(UINT32, POINTER_INPUT_TYPE*);
typedef BOOL    WINAPI type_GetPointerTouchInfo(UINT32, POINTER_TOUCH_INFO*);
typedef BOOL    WINAPI type_GetPointerPenInfo(UINT32, POINTER_PEN_INFO*);

typedef BOOL    WINAPI type_RegisterTouchWindow(HWND, ULONG);

/* DLL Functions: */
//HMODULE WINAPI LoadLibraryA(LPCSTR lpFileName);
//FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lProcName);

type_SetWindowSubclass *SetWindowSubclass;
type_DefSubclassProc *DefSubclassProc;
type_RemoveWindowSubclass *RemoveWindowSubclass;

type_GetTouchInputInfo *GetTouchInputInfo;
type_CloseTouchInputHandle *CloseTouchInputHandle;
type_ScreenToClient *ScreenToClient;
type_GetPointerInfo *GetPointerInfo;
type_GetPointerType *GetPointerType;
type_GetPointerTouchInfo *GetPointerTouchInfo;
type_GetPointerPenInfo *GetPointerPenInfo;
type_RegisterTouchWindow *RegisterTouchWindow;

#define TOUCH_COORD_TO_PIXEL(l)         ((l) / 100)
#define TOUCHEVENTF_UP              0x0004
#define GET_POINTERID_WPARAM(wParam)                (LOWORD(wParam))
#define POINTER_FLAG_INRANGE            0x00000002 // Pointer has not departed
#define POINTER_FLAG_INCONTACT          0x00000004 // Pointer is in contact
#define POINTER_FLAG_UP                 0x00040000 // Pointer transitioned from down state (broke contact)

#define WM_TOUCH                        0x0240
#define WM_POINTERDOWN                  0x0246
#define WM_POINTERUP                    0x0247
#define WM_POINTERUPDATE                0x0245
#define WM_POINTERCAPTURECHANGED        0x024C


// Storage for Touch Info
struct touch
{
    int TouchID;
    int PointX;
    int PointY;
};

// Could use MAX_TOUCH_COUNT from WinUser.h, but most devices will max out at 10 touch inputs anyway.
#define MAX_TOUCHES 10
touch TouchArray[MAX_TOUCHES] = { };

// Touch Info Stuff
void
ClearTouchArray()
{
    for (int i = 0; i < MAX_TOUCHES; i++)
    {
        TouchArray[i].TouchID = -1;
    } // END for i
}
// This function makes a random colour value for the touch

int
GetTouchIndex(int TouchID)
{
    int FirstEmptyIndex = -1;

    for (int i = 0; i < MAX_TOUCHES; i++)
    {
        if (TouchArray[i].TouchID == TouchID)
        {
            return i;
        }
        else if (FirstEmptyIndex == -1 &&  TouchArray[i].TouchID == -1)
        {
            FirstEmptyIndex = i;
        }
    } // END for i

    // TouchID not found in array, make a new entry
    if (FirstEmptyIndex >= 0)
    {
        TouchArray[FirstEmptyIndex].TouchID = TouchID;
        //TouchArray[FirstEmptyIndex].Color = MakeColor();
    }
    return (FirstEmptyIndex);
}
void
ReleaseTouchID(int TouchID)
{
    int TouchIDFound = 0;
    for (int i = 0; i < MAX_TOUCHES; i++)
    {
        if (TouchArray[i].TouchID == TouchID)
        {
            TouchArray[i].TouchID = -1;
            ++TouchIDFound;
        }
    } // END for i
    // TODO: Shuffle all touches back when one is removed
}

// WM_TOUCH handler. Windows 7 Only supports WM_TOUCH Events
void
HandleTouchEvent(HWND Window,
                LPARAM LParam,
                WPARAM WParam)
{
    UINT InputCount = LOWORD(WParam);
    PTOUCHINPUT Inputs = new TOUCHINPUT[InputCount];

    if (GetTouchInputInfo((HTOUCHINPUT)LParam, InputCount, Inputs, sizeof(TOUCHINPUT)))
    {
        for (int i = 0; i < (int)InputCount; i++)
        {
            TOUCHINPUT Touch = Inputs[i];

            if (Touch.dwID != 0)
            {
                POINT PtInput;
                PtInput.x = TOUCH_COORD_TO_PIXEL(Touch.x);
                PtInput.y = TOUCH_COORD_TO_PIXEL(Touch.y);
                ScreenToClient(Window, &PtInput);

                if (Touch.dwFlags & TOUCHEVENTF_UP)
                {
                    ReleaseTouchID(Touch.dwID);
                }
                else
                {
                    int Index = GetTouchIndex(Touch.dwID);
                    TouchArray[Index].PointX = PtInput.x;
                    TouchArray[Index].PointY = PtInput.y;
                    // TODO: Save Additional Info Here
                    //if (Touch.dwFlags & TOUCHINPUTMASKF_CONTACTAREA)
                    //{
                    //    TouchArray[Index].ContactX = TOUCH_COORD_TO_PIXEL(Touch.cxContact);
                    //    TouchArray[Index].ContactY = TOUCH_COORD_TO_PIXEL(Touch.cyContact);
                    //}
                }
            }
        } // END for i
        
    }
    CloseTouchInputHandle((HTOUCHINPUT)LParam);
    delete [] Inputs;
}

// WM_POINTER Handler.
int
HandlePointerEvent(HWND Window,
                   UINT Message,
                   LPARAM LParam,
                   WPARAM WParam)
{
    int PointerId = GET_POINTERID_WPARAM(WParam);

    // Store Touch Array Index
    int Index = -1;
    POINTER_INFO PointerInfo;
    if (GetPointerInfo(PointerId, &PointerInfo))
    {
        POINT PtInput;
        PtInput.x = PointerInfo.ptPixelLocation.x;
        PtInput.y = PointerInfo.ptPixelLocation.y;

        ScreenToClient(Window, &PtInput);

        if (PointerInfo.pointerFlags & POINTER_FLAG_INCONTACT)
        {
            Index = GetTouchIndex(PointerId);
            TouchArray[Index].PointX = PtInput.x;
            TouchArray[Index].PointY = PtInput.y;
        }
        else if (PointerInfo.pointerFlags & POINTER_FLAG_UP)
        {
            ReleaseTouchID(PointerId);
        }
        else if (PointerInfo.pointerFlags & POINTER_FLAG_INRANGE)
        {
            // Stylus is hovering over.
            // Todo: Can Save this additional info about the Stylus hovering over here but 
            //       would have to store and check for more state
        }

        POINTER_INPUT_TYPE PointerType;
        if (GetPointerType(PointerId, &PointerType))
        {
            switch (PointerInfo.pointerType)
            {
                case PT_MOUSE:
                {
                    //if (Index >= 0)
                    //{
                    //    // Could keep track of where the input came frome.
                    //    //TouchArray[Index].InputType = INPUT_TYPE_MOUSE;
                    //}
                } break;
                
                case PT_TOUCH:
                {
                    POINTER_TOUCH_INFO TouchInfo;
                    GetPointerTouchInfo(PointerId, &TouchInfo);
                    
                    //if (Index >= 0)
                    //{
                    //    // Could keep track of where the input came frome.
                    //    //TouchArray[Index].InputType = INPUT_TYPE_TOUCH;
                    //    // TODO: We could save Additional Touch Info here but all my devices just report a mask of 0 or useless data
                    //    if (TouchInfo.touchMask & TOUCH_MASK_PRESSURE)
                    //    {
                    //        TouchArray[Index].Pressure = TouchInfo.pressure;
                    //    }
                    //    //if (TouchInfo.touchMask & TOUCH_MASK_CONTACTAREA)
                    //    //    TouchInfo.rcContact;
                    //    //if (TouchInfo.touchMask & TOUCH_MASK_ORIENTATION)
                    //    //    TouchInfo.orientation;
                    //    //if (TouchInfo.touchMask & TOUCH_MASK_PRESSURE)
                    //    //    TouchInfo.pressure;
                    //}

                } break;

                case PT_PEN:
                {
                    POINTER_PEN_INFO PenInfo;
                    GetPointerPenInfo(PointerId, &PenInfo);

                    //if (Index >= 0)
                    //{
                    //    // Could keep track of where the input came frome.
                    //    //TouchArray[Index].InputType = INPUT_TYPE_PEN;
                    //    // TODO: We could save Additional Stylus Info here but all my devices just report a mask of 0
                    //    //       Probably better to just use something like easytab for stylus support.
                    //    if (PenInfo.penFlags & PEN_MASK_PRESSURE)
                    //    {
                    //    	//TouchArray[Index].Pressure = PenInfo.pressure;
                    //    }
                    //    //if (PenInfo.penFlags & PEN_MASK_ROTATION)
                    //    //  TouchArray[Index].angle = PenInfo.rotation;	 
                    //    //if (PenInfo.penFlags & PEN_MASK_TILT_X)
                    //    //  PenInfo.tiltX;		 
                    //    //if (PenInfo.penFlags & PEN_MASK_TILT_Y)
                    //    //  PenInfo.tiltY;
                    //}
                } break;

                default:
                {
                } break;
            } // END switch (PointerInfo.pointerType)
        } // END if (GetPointerType(PointerId, &PointerType))
    } // if (GetPointerInfo(PointerId, &PointerInfo))

    return 1;
}

LRESULT CALLBACK WindowProcSubclass(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam,  UINT_PTR IdSubclass, DWORD_PTR RefData)
{
    int IsWin7 = (int)RefData;
    if (IsWin7)
    {
        switch (Message)
        {
            case WM_TOUCH:
            {
                HandleTouchEvent(Window, LParam, WParam);
            } break;
        }
    }
    else
    {
        switch (Message)
        {
            case WM_TOUCH:
            {
                CloseTouchInputHandle((HTOUCHINPUT)LParam);
            } break;
            case WM_POINTERDOWN:
            case WM_POINTERUP:
            case WM_POINTERUPDATE:
            case WM_POINTERCAPTURECHANGED:
            {
                HandlePointerEvent(Window, Message, LParam, WParam);
            } break;
            default:
            {
            } break;
        } // END switch (switch_on)
    }
    return DefSubclassProc(Window, Message, WParam, LParam);
}

void
InitTouch(HWND NativeWindow, bool IsWin7)
{
    #define LoadFunction(Dll, Name) Name = (type_##Name *)GetProcAddress(Dll, #Name)

    HMODULE ComCtrDll = LoadLibraryA("comctl32.dll");
    if (ComCtrDll)
    {
        LoadFunction(ComCtrDll, SetWindowSubclass);
        LoadFunction(ComCtrDll, RemoveWindowSubclass);
        LoadFunction(ComCtrDll, DefSubclassProc);
    }
    HMODULE User32Dll = LoadLibraryA("User32.dll");
    {
        LoadFunction(User32Dll, GetTouchInputInfo);
        LoadFunction(User32Dll, CloseTouchInputHandle);
        LoadFunction(User32Dll, ScreenToClient);
        LoadFunction(User32Dll, GetPointerInfo);
        LoadFunction(User32Dll, GetPointerType);
        LoadFunction(User32Dll, GetPointerTouchInfo);
        LoadFunction(User32Dll, GetPointerPenInfo);
        LoadFunction(User32Dll, RegisterTouchWindow);
    }
    #undef GLFunction

    if (IsWin7)
    {
        // Register for Win7 WM_Touch events
        RegisterTouchWindow(NativeWindow, 0); 
    }
    //else
    //{
    //    // if we want to enable Mouse events in WM_Pointer messages
    //    EnableMouseInPointer(true);
    //}

    UINT_PTR SubclassID = 0;

        // Store what message proc to use in Subclass
        DWORD_PTR RefData  = (DWORD_PTR)IsWin7;

        bool resultsc = SetWindowSubclass(
            NativeWindow,
            WindowProcSubclass,
            SubclassID,
            RefData
        );

        // Initialize touch data
        ClearTouchArray();
}

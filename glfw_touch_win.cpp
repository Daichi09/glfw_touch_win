#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <stdio.h>
#include <math.h>

// Math and Drawing functions
struct v2
{
    float x, y;
};
struct v3
{
    float x, y, z;
};
union v4
{
    struct
    {
        float x, y, z, w;
    };
    struct
    {
        float r, g, b, a;
    };
};
inline v2
V2(float X, float Y)
{
    v2 Result;
    Result.x = X;
    Result.y = Y;
    return(Result);
}
inline v3
V3(float X, float Y, float Z)
{
    v3 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    return(Result);
}
inline v4
V4(float X, float Y, float Z, float W)
{
    v4 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
    return(Result);
}

// This function makes a random colour value for the touch
v4 MakeColor(){
    v4 Result = V4((float)rand()/(float)RAND_MAX,
        (float)rand()/(float)RAND_MAX,
        (float)rand()/(float)RAND_MAX,
        1.0f);
    return(Result);
}

inline void
OpenGLRectangle(v3 MinP, v3 MaxP, v4 Color)
{
    glBegin(GL_TRIANGLES);

    glColor4f(Color.r, Color.g, Color.b, Color.a);
    
    // Lower triangle
    glVertex3f(MinP.x, MinP.y, MinP.z);
    glVertex3f(MaxP.x, MinP.y, MinP.z);
    glVertex3f(MaxP.x, MaxP.y, MinP.z);
    
    // Upper triangle
    glVertex3f(MinP.x, MinP.y, MinP.z);
    glVertex3f(MaxP.x, MaxP.y, MinP.z);
    glVertex3f(MinP.x, MaxP.y, MinP.z);

    glEnd();
}
inline void
DrawRectRadius(float XPos, float YPos, float Radius, v4 Color)
{
    v3 Min = V3((float)XPos - Radius, (float)YPos - Radius, 0);
    v3 Max = V3((float)XPos + Radius, (float)YPos + Radius, 0);
    OpenGLRectangle(Min, Max, Color);
}

// Functions for Subclassing
typedef LRESULT (CALLBACK *SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
typedef BOOL (WINAPI *set_window_subclass)(HWND, SUBCLASSPROC, UINT_PTR, DWORD_PTR);
typedef LRESULT (WINAPI *def_subclass_proc)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (WINAPI *remove_window_subclass)(HWND, SUBCLASSPROC, UINT_PTR);

set_window_subclass    SetWindowSubclass = 0;
def_subclass_proc      DefSubclassProc = 0;
remove_window_subclass RemoveWindowSubclass = 0;

// Storage for Touch Info
struct touch
{
    v4 Color;
    int TouchID;
    int PointX;
    int PointY;
    //int Pressure;
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
        TouchArray[FirstEmptyIndex].Color = MakeColor();
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
                InvalidateRect(Window, NULL, FALSE);
            } break;
            default:
            {
            } break;
        } // END switch (switch_on)
    }
    return DefSubclassProc(Window, Message, WParam, LParam);
}

int main()
{
    if (glfwInit())
    {
        v2 Scale;

        // DPI Scaling
        GLFWmonitor *Monitor = glfwGetPrimaryMonitor();
        glfwGetMonitorContentScale(Monitor, &Scale.x, &Scale.y);

        GLFWwindow *Window = glfwCreateWindow((int)(800.0f * Scale.x), (int)(600.0f * Scale.y), "My Window", NULL, NULL);

        // Set touch event version
        int IsWin7 = false;

        HWND NativeWindow = glfwGetWin32Window(Window);
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

        // Load Subclassing Functions
        HMODULE comCtlDll = LoadLibraryA("comctl32.dll");
        if (comCtlDll)
        {
            SetWindowSubclass = (set_window_subclass)GetProcAddress(comCtlDll, "SetWindowSubclass");
            RemoveWindowSubclass = (remove_window_subclass)GetProcAddress(comCtlDll, "RemoveWindowSubclass");
            DefSubclassProc = (def_subclass_proc)GetProcAddress(comCtlDll, "DefSubclassProc");
        }
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

        if (Window)
        {
            glfwMakeContextCurrent(Window);

            glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);

            do {
                int WinWidth, WinHeight;
                glfwGetWindowSize(Window, &WinWidth, &WinHeight);
                glViewport(0, 0, WinWidth, WinHeight);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                float a = 2.0f/(float)WinWidth;
                float b = 2.0f/(float)WinHeight;
                float Proj[] =
                {
                    a,  0,  0,  0,
                    0,  b,  0,  0,
                    0,  0,  1,  0,
                   -1, -1,  0,  1
                };

                glMatrixMode(GL_PROJECTION);
                glLoadMatrixf(Proj);
                
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT);

                char buffer[512] = {};

                // Render all touches on screen
                for (int i = 0; i < MAX_TOUCHES; i++)
                {
                    if (TouchArray[i].TouchID != -1)
                    {
                        sprintf_s(buffer, "%sTouch[%d] = (%d, %d) ", buffer, i, TouchArray[i].PointX, TouchArray[i].PointY);

                        float PX = (float)TouchArray[i].PointX;
                        float PY = (float)TouchArray[i].PointY;
                        float Radius = 75.0f;

                        v4 Color = TouchArray[i].Color;
                        DrawRectRadius(PX, WinHeight - PY, Radius * Scale.x, Color);
                    }
                } // END for i
 
                // Too lazy to get font rendering going, so just output debug info to Window Title.
                glfwSetWindowTitle(Window, buffer);

                glfwSwapBuffers(Window);

                glfwPollEvents();
            } while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(Window) == 0);
        } // END if (Window)
        glfwDestroyWindow(Window);
    } // END if (glfwInit)
    return 0;
}
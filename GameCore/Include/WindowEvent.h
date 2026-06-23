// ウィンドウのイベント
// HACK: 今必要な分のみ定義
enum class WindowEventType
{
    None,
    Quit,
    MouseWheel,
    ControllerAdded,
    ControllerRemoved,
};
struct WindowEvent
{
    WindowEventType EventType;
    union
    {
        struct
        {
            int WheelX;
            int WheelY;
        } MouseWheel;

        struct
        {
            int ControllerID;
        } Controller;
    };
};
/**
 * Keil project example for GSM SIM800/900 for SMS and RTOS support
 *
 * @note      Check defines.h file for configuration settings!
 * @note      When using Nucleo F411 board, example has set 8MHz external HSE clock!
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 *
 * \par Description
 *
 * This examples shows how you can receive SMS and read it.
 * If you send SMS with specific content, it will do actions with LED on board:
 *
 * - LED ON: led will turn on,
 * - LED OFF: Led will turn off,
 * - LED TOGGLE: Led will toggle.
 *
 * After that, SMS with "OK" or "ERROR" should be returned to received number to confirm action
 *
 * \par Pinout for example (Nucleo STM32F411)
 *
\verbatim
GSM         STM32F4xx           DESCRIPTION
 
RX          PA9                 TX from STM to RX from GSM
TX          PA10                RX from STM
VCC         3.3V                Use external 3.3V regulator
GND         GND
RST         PA0
CTS         PA3                 RTS from ST to CTS from GSM
            BUTTON(PA0, PC13)   Discovery/Nucleo button, depends on configuration
            
            PA2                 TX for debug purpose (connect to PC) with 921600 bauds
\endverbatim
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_touch.h"
#include "tm_stm32_exti.h"
#include "cmsis_os.h"
#include "tm_stm32_general.h"

#include "gui.h"
#include "gui_window.h"
#include "gui_button.h"
#include "gui_led.h"
#include "gui_progbar.h"
#include "gui_graph.h"
#include "gui_edittext.h"
#include "gui_checkbox.h"
#include "gui_radio.h"
#include "gui_listbox.h"
#include "gui_textview.h"
#include "gui_dropdown.h"

#include "math.h"

#define COUNT_OF(x)     (sizeof(x) / sizeof((x)[0]))

TM_TOUCH_t TS;

GUI_HANDLE_p win1, handle;
GUI_GRAPH_DATA_p graphdata1, graphdata2, graphdata3, graphdata4;

#define ID_BASE             (GUI_ID_USER)
#define ID_BASE_WIN         (ID_BASE + 0x0100)
#define ID_BASE_BTN         (ID_BASE_WIN + 0x0100)
#define ID_BASE_TEXTWIEW    (ID_BASE_BTN + 0x0100)
#define ID_BASE_CHECKBOX    (ID_BASE_TEXTWIEW + 0x0100)
#define ID_BASE_LED         (ID_BASE_CHECKBOX + 0x0100)

/* List of window widget IDs */
#define ID_WIN_BTN          (ID_BASE_WIN + 0x01)
#define ID_WIN_EDIT         (ID_BASE_WIN + 0x02)
#define ID_WIN_RADIO        (ID_BASE_WIN + 0x03)
#define ID_WIN_CHECKBOX     (ID_BASE_WIN + 0x04)
#define ID_WIN_PROGBAR      (ID_BASE_WIN + 0x05)
#define ID_WIN_GRAPH        (ID_BASE_WIN + 0x06)
#define ID_WIN_LISTBOX      (ID_BASE_WIN + 0x07)
#define ID_WIN_LED          (ID_BASE_WIN + 0x08)
#define ID_WIN_TEXTVIEW     (ID_BASE_WIN + 0x09)
#define ID_WIN_DROPDOWN     (ID_BASE_WIN + 0x0A)

/* List of base buttons IDs */
#define ID_BTN_WIN_BTN      (ID_BASE_BTN + 0x01)
#define ID_BTN_WIN_EDIT     (ID_BASE_BTN + 0x02)
#define ID_BTN_WIN_RADIO    (ID_BASE_BTN + 0x03)
#define ID_BTN_WIN_CHECKBOX (ID_BASE_BTN + 0x04)
#define ID_BTN_WIN_PROGBAR  (ID_BASE_BTN + 0x05)
#define ID_BTN_WIN_GRAPH    (ID_BASE_BTN + 0x06)
#define ID_BTN_WIN_LISTBOX  (ID_BASE_BTN + 0x07)
#define ID_BTN_WIN_LED      (ID_BASE_BTN + 0x08)
#define ID_BTN_WIN_TEXTVIEW (ID_BASE_BTN + 0x09)
#define ID_BTN_WIN_DROPDOWN (ID_BASE_BTN + 0x0A)

#define ID_TEXTVIEW_1       (ID_BASE_TEXTWIEW + 0x01)

#define ID_CHECKBOX_LED     (ID_BASE_CHECKBOX + 0x01)

#define ID_LED_1            (ID_BASE_LED + 0x01)
#define ID_LED_2            (ID_BASE_LED + 0x02)
#define ID_LED_3            (ID_BASE_LED + 0x03)
#define ID_LED_4            (ID_BASE_LED + 0x04)

typedef struct {
    GUI_ID_t win_id;
    const GUI_Char* win_text;
} btn_user_data_t;

typedef struct {
    GUI_ID_t id;
    const GUI_Char* text;
    btn_user_data_t data;
} bulk_init_t;

#define RADIO_GROUP_HALIGN  0x01
#define RADIO_GROUP_VALIGN  0x02

bulk_init_t buttons[] = {
    {ID_BTN_WIN_BTN,        _T("Buttons"),      {ID_WIN_BTN, _T("Buttons")}},
    {ID_BTN_WIN_EDIT,       _T("Edit text"),    {ID_WIN_EDIT, _T("Edit text")}},
    {ID_BTN_WIN_RADIO,      _T("Radio box"),    {ID_WIN_RADIO, _T("Radio box")}},
    {ID_BTN_WIN_CHECKBOX,   _T("Check box"),    {ID_WIN_CHECKBOX, _T("Check box")}},
    {ID_BTN_WIN_PROGBAR,    _T("Progress bar"), {ID_WIN_PROGBAR, _T("Progress bar")}},
    {ID_BTN_WIN_GRAPH,      _T("Graph"),        {ID_WIN_GRAPH, _T("Graph")}},
    {ID_BTN_WIN_LISTBOX,    _T("List box"),     {ID_WIN_LISTBOX, _T("List box")}},
    {ID_BTN_WIN_LED,        _T("Led"),          {ID_WIN_LED, _T("Led")}},
    {ID_BTN_WIN_TEXTVIEW,   _T("Text view"),    {ID_WIN_TEXTVIEW, _T("Text view")}},
    {ID_BTN_WIN_DROPDOWN,   _T("Dropdown"),     {ID_WIN_DROPDOWN, _T("Dropdown")}},
};

char str[100];

extern GUI_Const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_Calibri_Bold_8;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Bold_18;
extern GUI_Const GUI_FONT_t GUI_Font_FontAwesome_Regular_30;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Narrow_Italic_22;

uint32_t time;

GUI_Char* listboxtexts[] = {
    _T("Item 0"),
    _T("Item 1"),
    _T("Item 2"),
    _T("Item 3"),
    _T("Item 4"),
    _T("Item 5"),
    _T("Item 6"),
    _T("Item 7"),
    _T("Item 8"),
    _T("Item 9"),
    _T("Item 10"),
    _T("Item 11"),
    _T("Item 12"),
};

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);

#define PI      3.14159265359f

static float len = 72, radius = 90;
float x, y;

int main(void) {
    GUI_STRING_UNICODE_t s;
    
    GUI_KeyboardData_t key;
    uint32_t state;
    
    TM_RCC_InitSystem();                                    /* Init system */
    HAL_Init();                                             /* Init HAL layer */
    TM_DISCO_LedInit();                                     /* Init leds */
    TM_DISCO_ButtonInit();                                  /* Init button */
    TM_DELAY_Init();                                        /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 115200);     /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("GUI; Compiled: %s %s, sizeof: %d\r\n", __DATE__, __TIME__, sizeof(char *) * 5);
    
    TM_GENERAL_DWTCounterEnable();
    
    GUI_Init();
    
    GUI_WIDGET_SetFontDefault(&GUI_Font_Arial_Narrow_Italic_22);    /* Set default font for widgets */
    
    win1 = GUI_WINDOW_GetDesktop();                         /* Get desktop window */
    
    for (state = 0; state < GUI_COUNT_OF(buttons); state++) {
        handle = GUI_BUTTON_Create(buttons[state].id, 5 + (state % 3) * 160, 5 + (state / 3) * 50, 150, 40, win1, button_callback, 0);
        GUI_WIDGET_SetText(handle, buttons[state].text);
        GUI_WIDGET_SetUserData(handle, &buttons[state].data);
    }
    
    __GUI_LINKEDLIST_PrintList(NULL);

    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = 1;
    TM_TOUCH_Init(NULL, &TS);
    
    GUI_STRING_UNICODE_Init(&s);
  
//    time = TM_DELAY_Time();
    state = 0;
	while (1) {
        GUI_Process();
        
        if ((TM_DELAY_Time() - time) >= 50) {
            time = TM_DELAY_Time();
            
            //__GUI_TIMER_Start(edit1->Timer);
            
//            if (state % 2) {
//                GUI_WINDOW_SetActive(win2);
//            } else {
//                GUI_WINDOW_SetActive(win3);
//            }
//            __GUI_LINKEDLIST_PrintList(NULL);
//            state++;
//            GUI_PROGBAR_SetValue(prog1, state % 100);
//            
//            x = cos((float)i * (PI / 180.0f));
//            y = sin((float)i * (PI / 180.0f));
//            GUI_GRAPH_DATA_AddValue(graphdata2, x * radius / 3, y * radius / 4);
//            i += 360.0f / len;
        }
        
        while (!TM_USART_BufferEmpty(DISCO_USART)) {
            GUI_Char ch = TM_USART_Getc(DISCO_USART);
            __GUI_DEBUG("Key: %c (%2X)\r\n", ch, ch);
            switch (GUI_STRING_UNICODE_Decode(&s, ch)) {
                case UNICODE_OK:
                    key.Keys[s.t - 1] = ch;
                    GUI_INPUT_KeyAdd(&key);
                    key.Keys[0] = 0;
                    GUI_INPUT_KeyAdd(&key);
                    break;
                case UNICODE_PROGRESS:
                    key.Keys[s.t - s.r - 1] = ch;
                default:
                    break;
            }
        }
	}
}

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_Init) {           /* Window has been just initialized */
        switch (GUI_WIDGET_GetId(h)) {  /* Button callbacks */
            case ID_WIN_BTN: {
                handle = GUI_BUTTON_Create(0, 10, 10, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Button 1"));
                handle = GUI_BUTTON_Create(0, 10, 60, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Button 2"));
                break;
            }
            case ID_WIN_CHECKBOX: {     /* Check box */
                handle = GUI_CHECKBOX_Create(ID_CHECKBOX_LED, 10, 10, 400, 40, h, checkbox_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Check box 1"));
                handle = GUI_CHECKBOX_Create(1, 10, 60, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Check box 2"));
                break;
            }
            case ID_WIN_RADIO: {        /* Radio box */
                uint8_t i;
                for (i = 0; i < 4; i++) {
                    handle = GUI_RADIO_Create(10, 10, 10 + (i * 30), 200, 25, h, 0, 0);
                    GUI_WIDGET_SetText(handle, _T("Radio box"));
                    GUI_RADIO_SetGroup(handle, i % 1);
                    GUI_RADIO_SetValue(handle, i);
                    GUI_RADIO_SetDisabled(handle, i / 2);
                }
                break;
            }
            case ID_WIN_LISTBOX: {      /* List box */
                uint8_t i;
                handle = GUI_LISTBOX_Create(1, 10, 10, 190, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_LISTBOX_AddString(handle, listboxtexts[i]);
                }
                GUI_LISTBOX_SetSliderAuto(handle, 0);
                GUI_LISTBOX_SetSliderVisibility(handle, 1);
                
                handle = GUI_LISTBOX_Create(1, 210, 10, 200, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_LISTBOX_AddString(handle, listboxtexts[i]);
                }
                GUI_LISTBOX_SetSliderAuto(handle, 0);
                GUI_LISTBOX_SetSliderVisibility(handle, 0);
                break;
            }
            case ID_WIN_GRAPH: {        /* Graph window */
                uint16_t i;
                handle = GUI_GRAPH_Create(0, 10, 10, 400, 220, h, 0, 0);

                GUI_GRAPH_SetMinX(handle, -100);
                GUI_GRAPH_SetMaxX(handle, 100);
                GUI_GRAPH_SetMinY(handle, -100);
                GUI_GRAPH_SetMaxY(handle, 100);
                GUI_WIDGET_Invalidate(handle);
                GUI_GRAPH_ZoomReset(handle);
                GUI_GRAPH_Zoom(handle, 0.1, 0.5, 0.5);

                if (!graphdata1) {
                    graphdata1 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_XY, len);
                    graphdata1->Color = GUI_COLOR_RED;
                }
                if (!graphdata2) {
                    graphdata2 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_YT, len / 2);
                    graphdata2->Color = GUI_COLOR_GREEN;
                }

                for (i = 0; i <= 360; i += 360 / len) {
                    x = cos((float)i * (PI / 180.0f));
                    y = sin((float)i * (PI / 180.0f));
                    GUI_GRAPH_DATA_AddValue(graphdata1, x * radius, y * radius);
                    GUI_GRAPH_DATA_AddValue(graphdata2, x * radius / 3, y * radius / 4);
                }
                GUI_GRAPH_AttachData(handle, graphdata1);
                GUI_GRAPH_AttachData(handle, graphdata2);
                break;
            }
            case ID_WIN_EDIT: {         /* Edit text */
                handle = GUI_EDITTEXT_Create(1, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_AllocTextMemory(handle, 255);
                GUI_WIDGET_SetText(handle, _T("Edit text"));
                break;
            }
            case ID_WIN_PROGBAR: {      /* Progress bar */
                handle = GUI_PROGBAR_Create(2, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Progbar"));
                
                handle = GUI_PROGBAR_Create(2, 10, 100, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Progbar"));
                GUI_PROGBAR_EnablePercentages(handle);
                break;
            }
            case ID_WIN_LED: {          /* Leds */
                handle = GUI_LED_Create(ID_LED_1, 10, 10, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                GUI_LED_Set(handle, 1);
                handle = GUI_LED_Create(ID_LED_2, 10, 40, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                
                handle = GUI_LED_Create(ID_LED_3, 10, 70, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                GUI_LED_Set(handle, 1);
                handle = GUI_LED_Create(ID_LED_4, 10, 100, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                
                handle = GUI_TEXTVIEW_Create(0, 40, 10, 400, 1000, h, 0, 0);
                GUI_WIDGET_SetFont(handle, &GUI_Font_Arial_Bold_18);
                GUI_WIDGET_SetText(handle, _T("\"LED\" are widgets used to indicate some status or any other situation. Press blue button on discovery board to see LED in happen\r\n"));
                break;
            }
            case ID_WIN_TEXTVIEW: {     /* Text view */
                handle = GUI_TEXTVIEW_Create(ID_TEXTVIEW_1, 10, 10, 300, 180, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Text view with automatic new line detector and support for different aligns.\r\n\r\nHowever, I can also manually jump to new line! Just like Word works ;)"));
                
                handle = GUI_RADIO_Create(0, 10, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align left"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_LEFT);
                
                handle = GUI_RADIO_Create(0, 120, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align center"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_CENTER);
                
                handle = GUI_RADIO_Create(0, 260, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align right"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_RIGHT);
                
                
                handle = GUI_RADIO_Create(0, 320, 10, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align top"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_TOP);
                
                handle = GUI_RADIO_Create(0, 320, 50, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align center"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_CENTER);
                
                handle = GUI_RADIO_Create(0, 320, 90, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align bottom"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_BOTTOM);
                break;
            }
            case ID_WIN_DROPDOWN: {     /* Dropdown */
                uint8_t i;
                handle = GUI_DROPDOWN_Create(0, 10, 10, 200, 40, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_DROPDOWN_AddString(handle, listboxtexts[i]);
                }
                GUI_DROPDOWN_SetSliderAuto(handle, 0);
                GUI_DROPDOWN_SetSliderVisibility(handle, 1);
                
                handle = GUI_DROPDOWN_Create(0, 220, 180, 200, 40, h, 0, 0);
                GUI_DROPDOWN_SetOpenDirection(handle, GUI_DROPDOWN_OPENDIR_UP);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_DROPDOWN_AddString(handle, listboxtexts[i]);
                }
                break;
            }
            default:
                break;  
        }
    }
    return res;
}

uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_SelectionChanged) {
        uint32_t group = GUI_RADIO_GetGroup(h);
        if (group == RADIO_GROUP_VALIGN || group == RADIO_GROUP_HALIGN) {
            GUI_HANDLE_p textview = GUI_WIDGET_GetById(ID_TEXTVIEW_1);
            if (textview) {
                if (group == RADIO_GROUP_VALIGN) {
                    GUI_TEXTVIEW_SetVAlign(textview, (GUI_TEXTVIEW_VALIGN_t)GUI_RADIO_GetValue(h));
                } else if (group == RADIO_GROUP_HALIGN) {
                    GUI_TEXTVIEW_SetHAlign(textview, (GUI_TEXTVIEW_HALIGN_t)GUI_RADIO_GetValue(h));
                }
            }
        }
    }
    return ret;
}

uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    return ret;
}

uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_ValueChanged) {
        if (GUI_WIDGET_GetId(h) == ID_CHECKBOX_LED) {
            if (GUI_CHECKBOX_IsChecked(h)) {
                TM_DISCO_LedOn(LED_ALL);
            } else {
                TM_DISCO_LedOff(LED_ALL);
            }
        }
    }
    return ret;
}

uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            break;
        }
        case GUI_WC_Click: {
            switch (GUI_WIDGET_GetId(h)) {
                case ID_BTN_WIN_BTN:
                case ID_BTN_WIN_CHECKBOX:
                case ID_BTN_WIN_DROPDOWN:
                case ID_BTN_WIN_EDIT:
                case ID_BTN_WIN_GRAPH:
                case ID_BTN_WIN_LED:
                case ID_BTN_WIN_LISTBOX:
                case ID_BTN_WIN_PROGBAR:
                case ID_BTN_WIN_RADIO:
                case ID_BTN_WIN_TEXTVIEW:  {
                    btn_user_data_t* data = GUI_WIDGET_GetUserData(h);
                    if (data) {
                        GUI_HANDLE_p tmp;
                        if ((tmp = GUI_WIDGET_GetById(data->win_id)) != 0) {
                            GUI_WIDGET_Show(tmp);
                            GUI_WIDGET_PutOnFront(tmp);
                        } else {
                            //tmp = GUI_WINDOW_CreateChild(data->win_id, 5, 5, 470, 262, GUI_WINDOW_GetDesktop(), window_callback, NULL);
                            tmp = GUI_WINDOW_CreateChild(data->win_id, 100, 20, 150, 100, GUI_WINDOW_GetDesktop(), window_callback, NULL);
                            GUI_WIDGET_SetText(tmp, data->win_text);
                            GUI_WIDGET_PutOnFront(tmp);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return res;
}  

/* 1ms handler */
void TM_DELAY_1msHandler() {
    //osSystickHandler();                             /* Kernel systick handler processing */
    
    GUI_UpdateTime(1);
}

/* printf handler */
int fputc(int ch, FILE* fil) {
    TM_USART_Putc(DISCO_USART, ch);
    return ch;
}

/**
 * Handle EXTI interrupt routine
 */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
    static GUI_TouchData_t p = {0}, t = {0};
    if (GPIO_Pin == GPIO_PIN_13) {
        uint8_t i, update = 0, diffx, diffy;
        TM_TOUCH_Read(&TS);                         /* Read touch data */
        
        memset((void *)&t, 0x00, sizeof(t));
        t.Status = TS.NumPresses ? GUI_TouchState_PRESSED : GUI_TouchState_RELEASED;
        t.Count = TS.NumPresses > GUI_TOUCH_MAX_PRESSES ? GUI_TOUCH_MAX_PRESSES : TS.NumPresses;
        for (i = 0; i < t.Count; i++) {
            t.X[i] = TS.X[i];
            t.Y[i] = TS.Y[i];
        }
        if (t.Count != p.Count) {
            update = 1;
        } else {
            for (i = 0; i < t.Count; i++) {
                diffx = __GUI_ABS(p.X[i] - t.X[i]);
                diffy = __GUI_ABS(p.Y[i] - t.Y[i]);
                if (diffx > 2 || diffy > 2) {
                    update = 1;
                    break;
                }
            }
        }
        
        /* Check differences */
        if (update || t.Status == GUI_TouchState_RELEASED) {
            GUI_INPUT_TouchAdd(&t);
            memcpy(&p, &t, sizeof(p));
        }
    }
}

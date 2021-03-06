/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI LISTBOX widget
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_LISTBOX_H
#define GUI_LISTBOX_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_LISTBOX Listbox
 * \brief           LISTBOX widget
 * \{
 */
    
/**
 * \brief           List of listbox colors
 */
typedef enum GUI_LISTBOX_COLOR_t {
    GUI_LISTBOX_COLOR_BG = 0x00,            /*!< Background color */
    GUI_LISTBOX_COLOR_TEXT,                 /*!< Text color index */
    GUI_LISTBOX_COLOR_SEL_FOC,              /*!< Text color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC,            /*!< Text color of selected item when widget is not in focus */
    GUI_LISTBOX_COLOR_SEL_FOC_BG,           /*!< Background color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC_BG,         /*!< Background color of selected item when widget is not in focus */
} GUI_LISTBOX_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_LISTBOX_SLIDER_ON      0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTBOX_SLIDER_AUTO    0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           LISTBOX string item object
 */
typedef struct GUI_LISTBOX_ITEM_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_Char* Text;                         /*!< Text entry */
} GUI_LISTBOX_ITEM_t;
    
/**
 * \brief           LISTBOX object structure
 */
typedef struct GUI_LISTBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t Selected;                       /*!< Selected text index */
    int16_t VisibleStartIndex;              /*!< Index in array of string on top of visible area of widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root of linked list entries */
    
    GUI_Dim_t SliderWidth;                  /*!< Slider width in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_LISTBOX_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new LISTBOX widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_LISTBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to listbox
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_LISTBOX_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t GUI_LISTBOX_SetColor(GUI_HANDLE_p h, GUI_LISTBOX_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Add a new string to list box
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to add to list. Only pointer is saved to memory!
 * \retval          1: String added to the end
 * \retval          0: String not added
 */
uint8_t GUI_LISTBOX_AddString(GUI_HANDLE_p h, const GUI_Char* text);

/**
 * \brief           Delete first string from list
 * \param[in,out]   h: Widget handle
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_LISTBOX_DeleteString
 * \sa              GUI_LISTBOX_DeleteLastString
 */
uint8_t GUI_LISTBOX_DeleteFirstString(GUI_HANDLE_p h);

/**
 * \brief           Delete last string from list
 * \param[in,out]   h: Widget handle
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_LISTBOX_DeleteString
 * \sa              GUI_LISTBOX_DeleteFirstString
 */
uint8_t GUI_LISTBOX_DeleteLastString(GUI_HANDLE_p h);

/**
 * \brief           Delete specific entry from list
 * \param[in,out]   h: Widget handle
 * \param[in]       index: List index (position) to delete
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_LISTBOX_DeleteFirstString
 * \sa              GUI_LISTBOX_DeleteLastString
 */
uint8_t GUI_LISTBOX_DeleteString(GUI_HANDLE_p h, uint16_t index);

/**
 * \brief           Set string value to already added string index
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index (position) on list to set/change text
 * \param[in]       *text: Pointer to text to add to list. Only pointer is saved to memory!
 * \retval          1: String changed
 * \retval          0: String not changed
 */
uint8_t GUI_LISTBOX_SetString(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text);

/**
 * \brief           Set selected value
 * \param[in,out]   h: Widget handle
 * \param[in]       selection: Set to -1 to invalidate selection or 0 - count-1 for specific selection 
 * \retval          1: Selection changed
 * \retval          0: Selection not changed
 */
uint8_t GUI_LISTBOX_SetSelection(GUI_HANDLE_p h, int16_t selection);

/**
 * \brief           Get selected value
 * \param[in,out]   h: Widget handle
 * \retval          Selection number or -1 if no selection
 */
int16_t GUI_LISTBOX_GetSelection(GUI_HANDLE_p h);

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \retval          1: Set OK
 * \retval          0: Set ERROR
 * \sa              GUI_LISTBOX_SetSliderVisibility
 */
uint8_t GUI_LISTBOX_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode);

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \retval          1: Set to desired value
 * \retval          0: Not set to desired value
 * \sa              GUI_LISTBOX_SetSliderAuto
 */
uint8_t GUI_LISTBOX_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible);

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \retval          1: Scroll successful
 * \retval          0: Scroll not successful
 */
uint8_t GUI_LISTBOX_Scroll(GUI_HANDLE_p h, int16_t step);

/**
 * \}
 */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

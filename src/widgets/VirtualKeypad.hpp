// keypad widget by https://github.com/hippyau
// widget found at https://github.com/ocornut/imgui/issues/4066

#pragma once

#include <map>
#include <sstream>
// #include "imgui.h"

namespace ImGui
{

    static std::map<const char *, int> g_KeypadApplyMap; // map of labels to return values
    static char *g_KeypadCurrentLabel;                   // only one instance of Keypad is open at any one time
    static std::string *g_KeypadEditStrPtr;              // pointer to string to edit
    static std::string g_KeypadEditStrRestore;           // stored value for undo

    // Draw a 4x5 button matrix entry keypad edits a *value std::string,
    // scaled to the current content region height with square buttons
    int InputKeypad(const char *label, bool *p_visible, std::string *value)
    {
        int ret = 0;

        if (p_visible && !*p_visible)
            return ret;
        if (!value || !label)
            return ret;

        ImVec2 csize = GetContentRegionAvail();
        int n = (csize.y / 5); // height / 5 button rows

        ImGuiStyle &style = GetStyle();

        if (BeginChild(label, ImVec2((n * 4) + style.WindowPadding.x, n * 5), true))
        {

            csize = GetContentRegionAvail();         // now inside this child
            n = (csize.y / 5) - style.ItemSpacing.y; // button size
            ImVec2 bsize(n, n);                      // buttons are square

            PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
            static std::string k = "";
            if (Button("ESC", bsize))
            {
                k = "X";
            }
            SameLine();
            if (Button("/", bsize))
            {
                k = "/";
            }
            SameLine();
            if (Button("*", bsize))
            {
                k = "*";
            }
            SameLine();
            if (Button("-", bsize))
            {
                k = "-";
            }
            if (Button("7", bsize))
            {
                k = "7";
            }
            SameLine();
            if (Button("8", bsize))
            {
                k = "8";
            }
            SameLine();
            if (Button("9", bsize))
            {
                k = "9";
            }
            SameLine();
            if (Button("+", bsize))
            {
                k = "+";
            }
            if (Button("4", bsize))
            {
                k = "4";
            }
            SameLine();
            if (Button("5", bsize))
            {
                k = "5";
            }
            SameLine();
            if (Button("6", bsize))
            {
                k = "6";
            }
            SameLine();
            if (Button("<-", bsize))
            {
                k = "B";
            }
            if (Button("1", bsize))
            {
                k = "1";
            }
            SameLine();
            if (Button("2", bsize))
            {
                k = "2";
            }
            SameLine();
            if (Button("3", bsize))
            {
                k = "3";
            }
            SameLine();
            if (Button("CLR", bsize))
            {
                k = "C";
            }
            if (Button("0", bsize))
            {
                k = "0";
            }
            SameLine();
            if (Button("0", bsize))
            {
                k = "0";
            }
            SameLine();
            if (Button(".", bsize))
            {
                k = ".";
            }
            SameLine();
            if (Button("=", bsize))
            {
                k = "E";
            }
            PopStyleVar();

            // logic
            if (k != "")
            {
                if (k != "E" && k != "X" && k != "B" && k != "C")
                {
                    value->append(k); // add k to the string
                }
                else
                {
                    if (k == "E")
                    {            // enter
                        ret = 1; // value has been accepted
                    }
                    else if (k == "B")
                    { // remove one char from the string
                        std::string tvalue = value->substr(0, value->length() - 1);
                        value->swap(tvalue);
                    }
                    else if (k == "C")
                    {
                        value->clear();
                    }
                    else if (k == "X")
                    {             // cancel
                        ret = -1; //  restore old value
                    }
                }
                if (ret)
                    *p_visible = false;
            }
            k = "";
            EndChild();
        }
        g_KeypadApplyMap[label] = ret; // store results in map
        return ret;
    }

    // The widget takes a label, and a *std::string
    // returns 1 if the new value should accepted, enter pressed
    // returns 0 if nothing happened
    // return -1 if cancel pressed, previous value has been restored
    int KeypadEditString(const char *label, std::string *value)
    {
        if (!label || !value)
            return 0;

        // Text(label);
        // SameLine();
        InputText(label, value->data(), value->capacity());

        if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (value != g_KeypadEditStrPtr)
            {
                g_KeypadEditStrRestore = value->c_str();
                g_KeypadEditStrPtr = value;
                g_KeypadApplyMap[label] = 0;
                g_KeypadCurrentLabel = (char *)label;
            }
            OpenPopup("KeypadX");
        }

        if (g_KeypadApplyMap[label] == 1)
        {
            g_KeypadApplyMap[label] = 0;
            return 1;
        }
        else if (g_KeypadApplyMap[label] == -1)
        {
            g_KeypadApplyMap[label] = 0;
            return -1;
        }
        return 0;
    }

    // Show the popup keypad box as required.
    void PopupKeypad(void)
    {
        // Always center this window when appearing
        ImVec2 center = GetMainViewport()->GetCenter();
        SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        SetNextWindowSize(ImVec2(300, 400));

        if (BeginPopupModal("KeypadX", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (g_KeypadEditStrPtr != nullptr)
            {
                bool gShowKeypad = true;
                Text(g_KeypadEditStrPtr->c_str());
                int r = InputKeypad("Keypad Input", &gShowKeypad, g_KeypadEditStrPtr);
                if (r == -1)
                {
                    // undo - restore previous value
                    g_KeypadEditStrPtr->swap(g_KeypadEditStrRestore);
                    g_KeypadApplyMap[g_KeypadCurrentLabel] = -1;
                }
                else if (r == 1)
                {
                    // set - we should apply the new value
                    g_KeypadApplyMap[g_KeypadCurrentLabel] = 1;
                }
                if (!gShowKeypad)
                    CloseCurrentPopup();
            }
            EndPopup();
        }
    }

} // Namespace ImGui
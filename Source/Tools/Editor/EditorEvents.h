//
// Copyright (c) 2018 Rokas Kupstys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once


#include <Urho3D/Core/Object.h>
#include "EditorEventsPrivate.h"


namespace Urho3D
{

/// Event sent during construction of toolbar buttons. Subscribe to it to add new buttons.
URHO3D_EVENT(E_EDITORTOOLBARBUTTONS, EditorToolbarButtons)
{
    URHO3D_PARAM(P_SCENE, Scene);                     // Scene pointer.
}

/// Event sent when node selection in scene view changes.
URHO3D_EVENT(E_EDITORSELECTIONCHANGED, EditorSelectionChanged)
{
    URHO3D_PARAM(P_SCENE, Scene);                     // Scene pointer.
}

/// Event sent when scene has it's rendering settings modified.
URHO3D_EVENT(E_EDITORSCENEEFFECTSCHANGED, EditorSceneEffectsChanged)
{
    URHO3D_PARAM(P_SCENE, Scene);                     // Scene pointer.
}

/// Event sent when rendering top menu bar of editor.
URHO3D_EVENT(E_EDITORAPPLICATIONMENU, EditorApplicationMenu)
{
}

/// Event sent when editor is about to save a project.
URHO3D_EVENT(E_EDITORPROJECTSAVING, EditorProjectSaving)
{
    URHO3D_PARAM(P_VALUE, Value);                     // Raw pointer to JSONValue.
}

/// Event sent when editor is about to load a new project.
URHO3D_EVENT(E_EDITORPROJECTLOADING, EditorProjectLoading)
{
    URHO3D_PARAM(P_VALUE, Value);                     // Raw pointer to JSONValue.
}

}

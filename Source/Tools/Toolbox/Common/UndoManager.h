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

#include "ToolboxAPI.h"
#include <Urho3D/Core/Object.h>
#include <Urho3D/Graphics/Drawable.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>


namespace Urho3D
{

class AttributeInspector;
class Gizmo;
class Scene;

namespace Undo
{

class URHO3D_TOOLBOX_API EditAction : public RefCounted
{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

class URHO3D_TOOLBOX_API CreateNodeAction : public EditAction
{
    unsigned nodeID;
    unsigned parentID;
    XMLFile nodeData;
    WeakPtr<Scene> editorScene;

public:
    explicit CreateNodeAction(Node* node)
        : nodeData(node->GetContext())
        , editorScene(node->GetScene())
    {
        nodeID = node->GetID();
        parentID = node->GetParent()->GetID();
        XMLElement rootElem = nodeData.CreateRoot("node");
        node->SaveXML(rootElem);
    }

    void Undo() override
    {
        Node* parent = editorScene->GetNode(parentID);
        Node* node = editorScene->GetNode(nodeID);
        if (parent != nullptr && node != nullptr)
        {
            parent->RemoveChild(node);
        }
    }

    void Redo() override
    {
        Node* parent = editorScene->GetNode(parentID);
        if (parent != nullptr)
        {
            Node* node = parent->CreateChild("", nodeID < FIRST_LOCAL_ID ? REPLICATED : LOCAL, nodeID);
            node->LoadXML(nodeData.GetRoot());
            // FocusNode(node);
        }
    }
};

class URHO3D_TOOLBOX_API DeleteNodeAction : public EditAction
{
    unsigned nodeID;
    unsigned parentID;
    XMLFile nodeData;
    WeakPtr<Scene> editorScene;

public:
    explicit DeleteNodeAction(Node* node)
        : nodeData(node->GetContext())
        , editorScene(node->GetScene())
    {
        nodeID = node->GetID();
        parentID = node->GetParent()->GetID();
        XMLElement rootElem = nodeData.CreateRoot("node");
        node->SaveXML(rootElem);
        rootElem.SetUInt("parentIndex", node->GetParent()->GetChildren().IndexOf(SharedPtr<Node>(node)));
    }

    void Undo() override
    {
        Node* parent = editorScene->GetNode(parentID);
        if (parent != nullptr)
        {
            SharedPtr<Node> node(new Node(parent->GetContext()));
            node->SetID(nodeID);
            if (node->LoadXML(nodeData.GetRoot()))
            {
                // FocusNode(node);
                parent->AddChild(node, nodeData.GetRoot().GetUInt("parentIndex"));
            }
        }
    }

    void Redo() override
    {
        Node* parent = editorScene->GetNode(parentID);
        Node* node = editorScene->GetNode(nodeID);
        if (parent != nullptr && node != nullptr)
        {
            parent->RemoveChild(node);
        }
    }
};

class URHO3D_TOOLBOX_API ReparentNodeAction : public EditAction
{
    unsigned nodeID;
    unsigned oldParentID;
    unsigned newParentID;
    PODVector<unsigned> nodeList; // 2 uints get inserted per node (node, node->GetParent())
    bool multiple;
    WeakPtr<Scene> editorScene;

public:
    ReparentNodeAction(Node* node, Node* newParent)
        : editorScene(node->GetScene())
    {
        multiple = false;
        nodeID = node->GetID();
        oldParentID = node->GetParent()->GetID();
        newParentID = newParent->GetID();
    }

    ReparentNodeAction(const Vector<Node*>& nodes, Node* newParent)
        : editorScene(newParent->GetScene())
    {
        multiple = true;
        newParentID = newParent->GetID();
        for(unsigned i = 0; i < nodes.Size(); ++i)
        {
            Node* node = nodes[i];
            nodeList.Push(node->GetID());
            nodeList.Push(node->GetParent()->GetID());
        }
    }

    void Undo() override
    {
        if (multiple)
        {
            for (unsigned i = 0; i < nodeList.Size(); i+=2)
            {
                unsigned nodeID_ = nodeList[i];
                unsigned oldParentID_ = nodeList[i+1];
                Node* parent = editorScene->GetNode(oldParentID_);
                Node* node = editorScene->GetNode(nodeID_);
                if (parent != nullptr && node != nullptr)
                node->SetParent(parent);
            }
        }
        else
        {
            Node* parent = editorScene->GetNode(oldParentID);
            Node* node = editorScene->GetNode(nodeID);
            if (parent != nullptr && node != nullptr)
            node->SetParent(parent);
        }
    }

    void Redo() override
    {
        if (multiple)
        {
            Node* parent = editorScene->GetNode(newParentID);
            if (parent == nullptr)
                return;

            for (unsigned i = 0; i < nodeList.Size(); i+=2)
            {
                unsigned nodeID_ = nodeList[i];
                Node* node = editorScene->GetNode(nodeID_);
                if (node != nullptr)
                node->SetParent(parent);
            }
        }
        else
        {
            Node* parent = editorScene->GetNode(newParentID);
            Node* node = editorScene->GetNode(nodeID);
            if (parent != nullptr && node != nullptr)
            node->SetParent(parent);
        }
    }
};

class URHO3D_TOOLBOX_API CreateComponentAction : public EditAction
{
    unsigned nodeID;
    unsigned componentID;
    XMLFile componentData;
    WeakPtr<Scene> editorScene;

public:
    explicit CreateComponentAction(Component* component)
        : editorScene(component->GetScene())
        , componentData(component->GetContext())
    {
        componentID = component->GetID();
        nodeID = component->GetNode()->GetID();
        XMLElement rootElem = componentData.CreateRoot("component");
        component->SaveXML(rootElem);
    }

    void Undo() override
    {
        Node* node = editorScene->GetNode(nodeID);
        Component* component = editorScene->GetComponent(componentID);
        if (node != nullptr && component != nullptr)
            node->RemoveComponent(component);
    }

    void Redo() override
    {
        Node* node = editorScene->GetNode(nodeID);
        if (node != nullptr)
        {
            Component* component = node->CreateComponent(
                componentData.GetRoot().GetAttribute("type"),
                componentID < FIRST_LOCAL_ID ? REPLICATED : LOCAL, componentID);
            component->LoadXML(componentData.GetRoot());
            component->ApplyAttributes();
            // FocusComponent(component);
        }
    }

};

class URHO3D_TOOLBOX_API DeleteComponentAction : public EditAction
{
    unsigned nodeID;
    unsigned componentID;
    XMLFile componentData;
    WeakPtr<Scene> editorScene;

public:
    DeleteComponentAction(Component* component)
        : componentData(component->GetContext())
        , editorScene(component->GetScene())
    {
        componentID = component->GetID();
        nodeID = component->GetNode()->GetID();
        XMLElement rootElem = componentData.CreateRoot("component");
        component->SaveXML(rootElem);
    }

    void Undo() override
    {
        Node* node = editorScene->GetNode(nodeID);
        if (node != nullptr)
        {
            Component* component = node->CreateComponent(
                componentData.GetRoot().GetAttribute("type"), componentID < FIRST_LOCAL_ID ? REPLICATED : LOCAL, componentID);
            if (component->LoadXML(componentData.GetRoot()))
            {
                component->ApplyAttributes();
                // FocusComponent(component);
            }
        }
    }

    void Redo() override
    {
        Node* node = editorScene->GetNode(nodeID);
        Component* component = editorScene->GetComponent(componentID);
        if (node != nullptr && component != nullptr)
        {
            node->RemoveComponent(component);
        }
    }
};

static unsigned GetID(Serializable* serializable)
{
    if (auto node = dynamic_cast<Node*>(serializable))
        return node->GetID();

    if (auto component = dynamic_cast<Component*>(serializable))
        return component->GetID();

    if (auto element = dynamic_cast<UIElement*>(serializable))
    {
        static unsigned uiElementIdIndex = 1;
        unsigned id = element->GetVar("UIElementID").GetUInt();
        if (!id)
        {
            id = uiElementIdIndex++;
            element->SetVar("UIElementID", id);
        }
        return id;
    }

    return M_MAX_UNSIGNED;
};

class URHO3D_TOOLBOX_API EditAttributeAction : public EditAction
{
    unsigned targetID;
    String attrName;
    Variant undoValue;
    Variant redoValue;
    StringHash targetType;
    WeakPtr<Scene> editorScene;
    WeakPtr<UIElement> root;

public:
    EditAttributeAction(Serializable* target, const String& name, const Variant& oldValue)
    {
        attrName = name;
        undoValue = oldValue;
        redoValue = target->GetAttribute(attrName);
        targetID = GetID(target);

        if (auto node = dynamic_cast<Node*>(target))
        {
            editorScene = node->GetScene();
            targetType = Node::GetTypeStatic();
        }
        else if (auto component = dynamic_cast<Component*>(target))
        {
            editorScene = component->GetScene();
            targetType = Component::GetTypeStatic();
        }
        else if (auto element = dynamic_cast<UIElement*>(target))
        {
            root = element->GetRoot();
            targetType = UIElement::GetTypeStatic();
        }
    }

    Serializable* GetTarget()
    {
        if (targetType == Node::GetTypeStatic())
            return editorScene->GetNode(targetID);
        else if (targetType == Component::GetTypeStatic())
            return editorScene->GetComponent(targetID);
        else if (targetType == UIElement::GetTypeStatic())
            return root->GetChild("UIElementID", targetID, true);

        return nullptr;
    }

    void Undo() override
    {
        Serializable* target = GetTarget();
        if (target != nullptr)
        {
            target->SetAttribute(attrName, undoValue);
            target->ApplyAttributes();
        }
    }

    void Redo() override
    {
        Serializable* target = GetTarget();
        if (target != nullptr)
        {
            target->SetAttribute(attrName, redoValue);
            target->ApplyAttributes();
        }
    }
};

class URHO3D_TOOLBOX_API CreateUIElementAction : public EditAction
{
    Variant elementID;
    Variant parentID;
    XMLFile elementData;
    SharedPtr<XMLFile> styleFile;
    WeakPtr<UIElement> root;

public:
    explicit CreateUIElementAction(UIElement* element)
        : elementData(element->GetContext())
    {
        root = element->GetRoot();
        elementID = GetID(element);
        parentID = GetID(element->GetParent());
        XMLElement rootElem = elementData.CreateRoot("element");
        element->SaveXML(rootElem);
        rootElem.SetUInt("index", element->GetParent()->FindChild(element));
        styleFile = element->GetDefaultStyle();
    }

    void Undo() override
    {
        UIElement* parent = root->GetChild("UIElementID", parentID, true);
        UIElement* element = root->GetChild("UIElementID", elementID, true);
        if (parent != nullptr && element != nullptr)
            parent->RemoveChild(element);
    }

    void Redo() override
    {
        UIElement* parent = root->GetChild("UIElementID", parentID, true);
        if (parent != nullptr)
            parent->LoadChildXML(elementData.GetRoot(), styleFile);
    }
};

class URHO3D_TOOLBOX_API DeleteUIElementAction : public EditAction
{
    Variant elementID;
    Variant parentID;
    XMLFile elementData;
    SharedPtr<XMLFile> styleFile;
    WeakPtr<UIElement> root;

public:
    explicit DeleteUIElementAction(UIElement* element)
        : elementData(element->GetContext())
    {
        root = element->GetRoot();
        elementID = GetID(element);
        parentID = GetID(element->GetParent());
        XMLElement rootElem = elementData.CreateRoot("element");
        element->SaveXML(rootElem);
        rootElem.SetUInt("index", element->GetParent()->FindChild(element));
        styleFile = SharedPtr<XMLFile>(element->GetDefaultStyle());
    }

    void Undo() override
    {
        UIElement* parent = root->GetChild("UIElementID", parentID, true);
        if (parent != nullptr)
            parent->LoadChildXML(elementData.GetRoot(), styleFile);
    }

    void Redo() override
    {
        UIElement* parent = root->GetChild("UIElementID", parentID, true);
        UIElement* element = root->GetChild("UIElementID", elementID, true);
        if (parent != nullptr && element != nullptr)
            parent->RemoveChild(element);
    }
};

class URHO3D_TOOLBOX_API ReparentUIElementAction : public EditAction
{
    Variant elementID;
    Variant oldParentID;
    unsigned oldChildIndex;
    Variant newParentID;
    WeakPtr<UIElement> root;

public:
    ReparentUIElementAction(UIElement* element, UIElement* newParent)
    : root(element->GetRoot())
    {
        elementID = GetID(element);
        oldParentID = GetID(element->GetParent());
        oldChildIndex = element->GetParent()->FindChild(element);
        newParentID = GetID(newParent);
    }

    void Undo() override
    {
        UIElement* parent = root->GetChild("UIElementID", oldParentID, true);
        UIElement* element = root->GetChild("UIElementID", elementID, true);
        if (parent != nullptr && element != nullptr)
            element->SetParent(parent, oldChildIndex);
    }

    void Redo() override
    {
        UIElement* parent = root->GetChild("UIElementID", newParentID, true);
        UIElement* element = root->GetChild("UIElementID", elementID, true);
        if (parent != nullptr && element != nullptr)
            element->SetParent(parent);
    }
};

class URHO3D_TOOLBOX_API ApplyUIElementStyleAction : public EditAction
{
    Variant elementID;
    Variant parentID;
    XMLFile elementData;
    XMLFile* styleFile;
    String elementOldStyle;
    String elementNewStyle;
    WeakPtr<UIElement> root;

public:
    ApplyUIElementStyleAction(UIElement* element, const String& newStyle)
        : elementData(element->GetContext())
    {
        root = element->GetRoot();
        elementID = GetID(element);
        parentID = GetID(element->GetParent());
        XMLElement rootElem = elementData.CreateRoot("element");
        element->SaveXML(rootElem);
        rootElem.SetUInt("index", element->GetParent()->FindChild(element));
        styleFile = element->GetDefaultStyle();
        elementOldStyle = element->GetAppliedStyle();
        elementNewStyle = newStyle;
    }

    void ApplyStyle(const String& style)
    {
        UIElement* parent = root->GetChild("UIElementID", parentID, true);
        UIElement* element = root->GetChild("UIElementID", elementID, true);
        if (parent != nullptr && element != nullptr)
        {
            // Apply the style in the XML data
            elementData.GetRoot().SetAttribute("style", style);
            parent->RemoveChild(element);
            parent->LoadChildXML(elementData.GetRoot(), styleFile);
        }
    }

    void Undo() override
    {
        ApplyStyle(elementOldStyle);
    }

    void Redo() override
    {
        ApplyStyle(elementNewStyle);
    }
};

class URHO3D_TOOLBOX_API EditUIStyleAction : public EditAction
{
    XMLFile oldStyle;
    XMLFile newStyle;
    unsigned elementId;
    WeakPtr<UIElement> root;

public:
    EditUIStyleAction(UIElement* element, XMLElement& styleElement, const Variant& newValue)
        : oldStyle(element->GetContext())
        , newStyle(element->GetContext())
    {
        root = element->GetRoot();
        elementId = GetID(element);
        oldStyle.CreateRoot("style").AppendChild(element->GetDefaultStyle()->GetRoot(), true);
        if (newValue.IsEmpty())
            styleElement.Remove();
        else
            styleElement.SetVariantValue(newValue);
        newStyle.CreateRoot("style").AppendChild(element->GetDefaultStyle()->GetRoot(), true);
    }

    void Undo() override
    {
        UIElement* element = root->GetChild("UIElementID", elementId, true);
        XMLElement root = element->GetDefaultStyle()->GetRoot();
        root.RemoveChildren();
        for (auto child = oldStyle.GetRoot().GetChild(); !child.IsNull(); child = child.GetNext())
            root.AppendChild(child, true);
    }

    void Redo() override
    {
        UIElement* element = root->GetChild("UIElementID", elementId, true);
        XMLElement root = element->GetDefaultStyle()->GetRoot();
        root.RemoveChildren();
        for (auto child = newStyle.GetRoot().GetChild(); !child.IsNull(); child = child.GetNext())
            root.AppendChild(child, true);
    }
};

using StateCollection = Vector<SharedPtr<EditAction>>;

class URHO3D_TOOLBOX_API Manager : public Object
{
URHO3D_OBJECT(Manager, Object);
public:
    /// Construct.
    explicit Manager(Context* ctx);
    /// Go back in the state history.
    void Undo();
    /// Go forward in the state history.
    void Redo();
    /// Clear all tracked state.
    void Clear();

    /// Track changes performed by this scene.
    void Connect(Scene* scene);
    /// Track changes performed by this attribute inspector.
    void Connect(AttributeInspector* inspector);
    /// Track changes performed to UI hierarchy of this root element.
    void Connect(UIElement* root);
    /// Track changes performed by this gizmo.
    void Connect(Gizmo* gizmo);

    template<typename T, typename... Args>
    void Track(Args... args)
    {
        if (trackingEnabled_)
            currentFrameStates_.Push(SharedPtr<T>(new T(args...)));
    };

    /// Enables or disables tracking changes.
    void SetTrackingEnabled(bool enabled) { trackingEnabled_ = enabled; }
    /// Return true if manager is tracking undoable changes.
    bool IsTrackingEnabled() const { return trackingEnabled_; }

protected:
    /// State stack
    Vector<StateCollection> stack_;
    /// Current state index.
    int32_t index_ = 0;
    /// Flag indicating that state tracking is suspended. For example when undo manager is restoring states.
    bool trackingEnabled_ = true;
    /// All actions performed on current frame. They will be applied together.
    StateCollection currentFrameStates_;
};

class URHO3D_TOOLBOX_API SetTrackingScoped
{
public:
    /// Set undo manager tracking in this scope. Restore to the old value on scope exit.
    explicit SetTrackingScoped(Manager& manager, bool tracking)
        : manager_(manager)
        , tracking_(manager.IsTrackingEnabled())
    {
        manager.SetTrackingEnabled(tracking);
    }

    ~SetTrackingScoped()
    {
        manager_.SetTrackingEnabled(tracking_);
    }

protected:
    /// Undo manager which is being operated upon.
    Manager& manager_;
    /// Old tracking value.
    bool tracking_;
};

}

}

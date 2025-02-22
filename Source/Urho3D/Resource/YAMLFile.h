//
// Copyright (c) 2008-2018 the Urho3D project.
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

#include "Urho3D/Resource/Resource.h"
#include "Urho3D/Resource/JSONValue.h"

namespace Urho3D
{

/// JSON document resource.
class URHO3D_API YAMLFile : public Resource
{
    URHO3D_OBJECT(YAMLFile, Resource);

public:
    /// Construct.
    explicit YAMLFile(Context* context);
    /// Destruct.
    ~YAMLFile() override = default;
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    bool BeginLoad(Deserializer& source) override;
    /// Save resource with default indentation (two spaces). Return true if successful.
    bool Save(Serializer& dest) const override;
    /// Save resource with user-defined indentation (number of spaces). Return true if successful.
    bool Save(Serializer& dest, int indendation) const;

    /// Deserialize from a string. Return true if successful.
    bool FromString(const String& source);

    /// Return root value.
    JSONValue& GetRoot() { return root_; }
    /// Return root value.
    const JSONValue& GetRoot() const { return root_; }

    /// Return true if parsing json string into JSONValue succeeds.
    static bool ParseYAML(const String& yaml, JSONValue& value, bool reportError = true);
private:
    /// JSON root value.
    JSONValue root_;
};

}

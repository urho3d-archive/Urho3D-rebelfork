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

#include "Urho3DClassWrappers.hpp"
#include "CSharp.h"

extern "C"
{

EXPORT_API MarshalAllocator::Block* Urho3D__Context__GetObjectCategories(Context* context)
{
    if (context == nullptr)
        return nullptr;
    return CSharpConverter<Urho3D::Vector<Urho3D::String>>::ToCSharp(context->GetObjectCategories().Keys());
}

EXPORT_API MarshalAllocator::Block* Urho3D__Context__GetObjectsByCategory(Context* context, MarshalAllocator::Block* category)
{
    StringVector result;
    const auto& factories = context->GetObjectFactories();
    auto it = context->GetObjectCategories().Find(CSharpConverter<Urho3D::String>::FromCSharp(category));
    if (it != context->GetObjectCategories().End())
    {
        for (const StringHash& type : it->second_)
        {
            auto jt = factories.Find(type);
            if (jt != factories.End())
                result.Push(jt->second_->GetTypeName());
        }
        return CSharpConverter<StringVector>::ToCSharp(result);
    }
    else
        return nullptr;
}

// Urho3D::Application::engineParameters_
EXPORT_API Urho3D::VariantMap* get_Urho3D__Application_engineParameters_ref(Wrappers::Application* instance)
{
    return &instance->__get_engineParameters_();
}

}

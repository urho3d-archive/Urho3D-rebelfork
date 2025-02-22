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


#include "Pass/CppPass.h"
#include "Printer/CSharpPrinter.h"

namespace Urho3D
{

class GenerateCSharpApiPass : public CppApiPass
{
    public:
    explicit GenerateCSharpApiPass() { };

    void Start() override;
    bool Visit(MetaEntity* entity, cppast::visitor_info info) override;
    void Stop() override;

    static std::string MapToCS(const cppast::cpp_type& type, const std::string& expression);
    static std::string ToCSType(const cppast::cpp_type& type,bool disallowReferences=false);

protected:
    std::string MapToPInvoke(const cppast::cpp_type& type, const std::string& expression);
    std::string FormatCSParameterList(const std::vector<std::shared_ptr<MetaEntity>>& parameters);
    std::string ConvertDefaultValueToCS(MetaEntity* user, std::string value, const cppast::cpp_type& type, bool allowComplex);
    void PrintParameterHandlingCodePre(const std::vector<std::shared_ptr<MetaEntity>>& parameters);
    void PrintParameterHandlingCodePost(const std::vector<std::shared_ptr<MetaEntity>>& parameters);
    void PrintInstanceDisposedCheck(const std::string& objectName);
    std::string MapParameterList(const std::vector<std::shared_ptr<MetaEntity>>& parameters);

    CSharpPrinter printer_;
    CSharpPrinter interface_;
};

}

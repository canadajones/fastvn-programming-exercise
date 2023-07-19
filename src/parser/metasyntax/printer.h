#ifndef METASYNTAX_PRINTER_HEADER
#define METASYNTAX_PRINTER_HEADER

#include <ostream>

#include "ast.h"

namespace metasyntax { namespace ast
{

    const int tabsize = 4;

    struct line_printer
    {
        typedef void result_type;

        line_printer(std::ostream& out, int indent = 0)
          : out(out), indent(indent) {}

        void operator()(const List& ast) const
        {
            out << '(' << std::endl;
            for (auto const& entry : ast.entries)
            {
                tab(indent+tabsize);
                out << entry.first.stringify() << " = ";
                boost::apply_visitor(line_printer(out, indent+tabsize), entry.second);
            }
            tab(indent);
            out << ')' << std::endl;
        }

        void operator()(const Object& obj) const {
            out << '{' << std::endl;
            for (auto const& entry : obj.values)
            {
                tab(indent+tabsize);
                out << '"' << entry.first << "\" : ";
                boost::apply_visitor(line_printer(out, indent+tabsize), entry.second);
            }
            tab(indent);
            out << '}' << std::endl;
        }

        void operator()(const std::string& text) const  {
            out << '"' << text << '"' << std::endl;
        }

        void operator()(const double& num) const {
            out << num << std::endl;
        }

         void operator()(const Array& arr) const {
            out << '[' << std::endl;
            for (auto const& elem : arr.values)
            {
                tab(indent+tabsize);
                boost::apply_visitor(line_printer(out, indent+tabsize), elem);
                out << ',';
            }
            tab(indent);
            out << ']' << std::endl;
        }


        void tab(int spaces) const
        {
            for (int i = 0; i < spaces; ++i)
                out << ' ';
        }

        std::ostream& out;
        int indent;
    };
}}
#endif
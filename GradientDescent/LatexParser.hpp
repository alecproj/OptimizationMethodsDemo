#ifndef LATEXPARSER_HPP_
#define LATEXPARSER_HPP_

#include "exprtk.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace LatexLib {

    class LatexParser {
    public:
        static bool parse(const string &latexExpr,
                          const vector<string> &varNames,
                          exprtk::expression<double> &expression,
                          vector<double> &varRefs)
        {
            using symbol_table_t = exprtk::symbol_table<double>;
            using parser_t = exprtk::parser<double>;

            // На данном этапе мы просто копируем строку LaTeX в exprStr
            const string& exprStr = latexExpr;

            // --- Создание таблицы переменных ---
            symbol_table_t symbol_table;
            varRefs.resize(varNames.size(), 0.0);
            for (size_t i = 0; i < varNames.size(); ++i)
                symbol_table.add_variable(varNames[i], varRefs[i]);
            symbol_table.add_constants();
            expression.register_symbol_table(symbol_table);

            // --- Компиляция ---
            parser_t parser;
            if (!parser.compile(exprStr, expression)) {
                cerr << "Error parsing:\n" << parser.error() << endl;
                return false;
            }

            cout << "Successful: " << exprStr << endl;
            return true;
        }
    };

} // namespace LatexLib

#endif // LATEXPARSER_HPP_

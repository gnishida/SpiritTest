﻿#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace client {
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

	struct rhs_statement {
		std::string op;
		double param_value;
		std::string successor_shape;
	};

    struct cga_rule {
        std::string lhs;                           // tag name
		rhs_statement rhs;
    };
    //]
}

// We need to tell fusion about our mini_xml struct
// to make it a first-class fusion citizen
//[tutorial_xml1_adapt_structures
BOOST_FUSION_ADAPT_STRUCT(
    client::rhs_statement,
    (std::string, op)
	(double, param_value)
	(std::string, successor_shape)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::cga_rule,
    (std::string, lhs)
    (client::rhs_statement, rhs)
)
//]

namespace client {
    // 自前で実装するgrammarは、qi::grammarを継承させる。
	// qi::grammarの2番目の引数には、結果を格納するオブジェクトを指定する。
	// base_type()の引数が、axiomに相当する。
    template <typename Iterator>
    struct cga_grammar : qi::grammar<Iterator, cga_rule(), ascii::space_type> {
        cga_grammar() : cga_grammar::base_type(start) {
            using qi::lit;
            using qi::lexeme;
			using qi::double_;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;
			using boost::spirit::attr;

            using phoenix::at_c;
            using phoenix::push_back;

			start =	lhs	>> "-->" >> rhs;

			rhs = op >> "(" >> param_value >> ")" >> successor_shape;

			lhs = char_("a-zA-Z") >> *(char_("a-zA-Z"));
			op = char_("a-zA-Z") >> *(char_("a-zA-Z"));
			param_value = double_;
			successor_shape = char_("a-zA-Z") >> *(char_("a-zA-Z")) | attr("");
        }

        qi::rule<Iterator, cga_rule(), ascii::space_type> start;
        qi::rule<Iterator, std::string(), ascii::space_type> lhs;
		qi::rule<Iterator, rhs_statement(), ascii::space_type> rhs;
        qi::rule<Iterator, std::string(), ascii::space_type> op;
        qi::rule<Iterator, double(), ascii::space_type> param_value;
        qi::rule<Iterator, std::string(), ascii::space_type> successor_shape;
    };
    //]
}

int main(int argc, char **argv) {
    char const* filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in) {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef client::cga_grammar<std::string::const_iterator> cga_grammar;
    cga_grammar g; // Our grammar
    client::cga_rule cga_rule; // Our tree

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();

	/// EmployeeParserと同様、3番目の引数がgrammarだ。つまり、mini_xml_grammarを使ってparseする。
	// 4番目の引数はskipする文字。つまり、空白をskipする
	// 5番目の引数に結果が返却される。
    bool r = phrase_parse(iter, end, g, space, cga_rule);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
		std::cout << cga_rule.lhs << " --> " << cga_rule.rhs.op << "(" << cga_rule.rhs.param_value << ") " << cga_rule.rhs.successor_shape << std::endl;
        return 0;
    } else {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << context << "...\"\n";
        std::cout << "-------------------------\n";
        return 1;
    }
}